# SWAP Heartbeat

## Purpose

This is a simple heartbeat library that sends out a packet at a specified interval with higher precision than Python's built-in `time.sleep()` function. It is meant for the SWAP project.

## Usage

```python
from swap_heartbeat import swap_heartbeat
from multiprocessing import Process
from time import sleep

# See the packets flow with the following command:
#
# sudo tshark -Y 'ip.src==192.168.0.121 && ip.dst==192.168.0.246'


def start_heartbeat():
    swap_heartbeat.swap_init()

    swap_heartbeat.run_packet_transmission(
        b"eno1",
        b"\xDE\xAD\xBE\xEF\xDE\xAD",
        b"\xB8\x27\xEB\x3D\xC6\x84",
        b"192.168.0.121",
        b"192.168.0.246",
        666,   # Source port
        80,    # Destination port
        1000,  # Interval in microseconds
    )

swp_hb_proc = Process(target=start_heartbeat)

sleep(60)

swap_heartbeat.stop_packet_transmission()

swap_heartbeat.swap_cleanup()
```

## Installation

```bash
pip install git+https://github.com/NET-BYU/swap-heartbeat.git
```