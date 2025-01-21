# Import C types
from libc.stdint cimport int32_t

# Declare the external C function with new parameters
cdef extern from "packet_sender.h":
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
    )
    void stop_transmission()
    
    void init_shared_memory()

def run_packet_transmission(
    bytes iface, 
    bytes src_mac, 
    bytes dst_mac, 
    bytes src_ip, 
    bytes dst_ip, 
    int src_port, 
    int dst_port, 
    int interval, 
    int num_packets
):
    """
    Runs the packet transmission with configurable parameters.

    Args:
        iface (bytes): Network interface name (e.g., b"eno1")
        src_mac (bytes): Source MAC address (e.g., b"\xDE\xAD\xBE\xEF\xDE\xAD")
        dst_mac (bytes): Destination MAC address (e.g., b"\xB8\x27\xEB\x3D\xC6\x84")
        src_ip (bytes): Source IP address (e.g., b"192.168.0.121")
        dst_ip (bytes): Destination IP address (e.g., b"192.168.0.246")
        src_port (int): Source port number
        dst_port (int): Destination port number
        interval (int): Packet sending interval in microseconds
        num_packets (int): Number of packets to send
    """
    cdef const char *c_iface = iface
    cdef const char *c_src_mac = src_mac
    cdef const char *c_dst_mac = dst_mac
    cdef const char *c_src_ip = src_ip
    cdef const char *c_dst_ip = dst_ip

    result = start_packet_transmission(
        c_iface, c_src_mac, c_dst_mac, c_src_ip, c_dst_ip,
        src_port, dst_port, interval, num_packets
    )
    
    if result != 0:
        raise RuntimeError("Packet transmission failed")

def stop_packet_transmission():
    """Stops the ongoing packet transmission"""
    stop_transmission()
    
def swap_init():
    """Initializes the shared memory for the swap mechanism"""
    init_shared_memory()