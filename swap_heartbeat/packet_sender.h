// packet_sender.h
#ifndef PACKET_SENDER_H
#define PACKET_SENDER_H

int start_packet_transmission(
    const char *iface, 
    const char *src_mac, 
    const char *dst_mac, 
    const char *src_ip, 
    const char *dst_ip, 
    int src_port, 
    int dst_port, 
    int interval, 
    int num_packets
);

#endif
