# SWAP Heartbeat

## Purpose

This is a simple heartbeat library that sends out a packet at a specified interval with higher precision than Python's built-in `time.sleep()` function. It is meant for the SWAP project.

## Usage

```python
import swap_heartbeat

# Start the heartbeat

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
```

## Installation

```bash
pip install git+https://github.com/NET-BYU/swap-heartbeat.git
```