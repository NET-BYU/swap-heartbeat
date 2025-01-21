from swap_heartbeat import swap_heartbeat
from time import time

start = time()

swap_heartbeat.run_packet_transmission(
    b"eno1", 
    b"\xDE\xAD\xBE\xEF\xDE\xAD", 
    b"\xB8\x27\xEB\x3D\xC6\x84", 
    b"192.168.0.121", 
    b"192.168.0.246", 
    666,   # Source port
    80,    # Destination port
    1000,  # Interval in microseconds
    60000  # Number of packets to send
)

end = time()

print("Time taken: ", end - start)