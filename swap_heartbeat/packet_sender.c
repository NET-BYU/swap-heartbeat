#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define SHM_NAME "packet_transmission_shm"
#define SHM_SIZE sizeof(int)

int *stop_flag; // Stop flag

// Function prototypes
int start_packet_transmission(const char *iface, const char *src_mac,
                              const char *dst_mac, const char *src_ip,
                              const char *dst_ip, int src_port, int dst_port,
                              int interval);

void init_shared_memory();

// Function to initialize shared memory
void init_shared_memory() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }

    stop_flag = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (stop_flag == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Initialize the shared memory
    *stop_flag = 0;
}

unsigned short checksum(void *b, int len) {
  unsigned short *buf = b;
  unsigned int sum = 0;
  unsigned short result;

  for (sum = 0; len > 1; len -= 2)
    sum += *buf++;
  if (len == 1)
    sum += *(unsigned char *)buf;
  sum = (sum >> 16) + (sum & 0xFFFF);
  sum += (sum >> 16);
  result = ~sum;
  return result;
}

unsigned short tcp_checksum(struct iphdr *iph, struct tcphdr *tcph) {
    struct pseudo_header {
        u_int32_t source_address;
        u_int32_t dest_address;
        u_int8_t placeholder;
        u_int8_t protocol;
        u_int16_t tcp_length;
    } psh;

    int total_len = sizeof(struct pseudo_header) + sizeof(struct tcphdr);
    char *buffer = (char *)malloc(total_len);

    // Set up the pseudo header
    psh.source_address = iph->saddr;
    psh.dest_address = iph->daddr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr));

    // Copy the pseudo header and TCP header into a buffer
    memcpy(buffer, &psh, sizeof(struct pseudo_header));
    memcpy(buffer + sizeof(struct pseudo_header), tcph, sizeof(struct tcphdr));

    // Calculate the checksum
    unsigned short checksum_result = checksum((unsigned short *)buffer, total_len);

    free(buffer);
    return checksum_result;
}

void create_packet(char *packet, uint32_t seq_num, const char *src_mac,
                   const char *dst_mac, const char *src_ip, const char *dst_ip,
                   int src_port, int dst_port) {
  struct ethhdr *eth = (struct ethhdr *)packet;
  struct iphdr *iph = (struct iphdr *)(packet + sizeof(struct ethhdr));
  struct tcphdr *tcph =
      (struct tcphdr *)(packet + sizeof(struct ethhdr) + sizeof(struct iphdr));

  memcpy(eth->h_source, src_mac, 6);
  memcpy(eth->h_dest, dst_mac, 6);
  eth->h_proto = htons(ETH_P_IP);

  iph->ihl = 5;
  iph->version = 4;
  iph->tos = 0;
  iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));
  iph->id = htonl(54321);
  iph->frag_off = 0;
  iph->ttl = 64;
  iph->protocol = IPPROTO_TCP;
  iph->saddr = inet_addr(src_ip);
  iph->daddr = inet_addr(dst_ip);
  iph->check = 0;
  iph->check = checksum(iph, sizeof(struct iphdr));

  tcph->source = htons(src_port);
  tcph->dest = htons(dst_port);
  tcph->seq = htonl(seq_num);
  tcph->ack_seq = 0;
  tcph->doff = 5;
  tcph->syn = 1;
  tcph->window = htons(5840);
  tcph->urg_ptr = 0;
  tcph->check = tcp_checksum(iph, tcph);
}

int start_packet_transmission(const char *iface, const char *src_mac,
                              const char *dst_mac, const char *src_ip,
                              const char *dst_ip, int src_port, int dst_port,
                              int interval) {
  int sock;
  struct sockaddr_ll sa;
  char packet[sizeof(struct ethhdr) + sizeof(struct iphdr) +
              sizeof(struct tcphdr)];
  uint32_t seq_num = 0;
  struct timespec ts;

  sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (sock < 0) {
    perror("Socket creation failed");
    return -1;
  }

  memset(&sa, 0, sizeof(struct sockaddr_ll));
  sa.sll_ifindex = if_nametoindex(iface);
  sa.sll_protocol = htons(ETH_P_IP);

  clock_gettime(CLOCK_MONOTONIC, &ts);
  long long start_time = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;

  while (*stop_flag == 0) {

    memset(packet, 0, sizeof(packet));

    clock_gettime(CLOCK_MONOTONIC, &ts);
    long long current_time = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
    long long time_diff = current_time - start_time;

    if (time_diff < (seq_num * interval)) {
      usleep((seq_num * interval) - time_diff);
    }

    create_packet(packet, seq_num++, src_mac, dst_mac, src_ip, dst_ip, src_port,
                  dst_port);
    if (sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&sa,
               sizeof(struct sockaddr_ll)) < 0) {
      perror("Packet send failed");
    }
  }

  munmap(stop_flag, SHM_SIZE);

  close(sock);
  return 0;
}
