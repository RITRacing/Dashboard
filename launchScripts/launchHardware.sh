#!/bin/bash
# This script will launch the CAN data server.

echo "Launching the Hardware"
sleep 1
`/sbin/ip link set can0 up type can bitrate 250000`

cd /home/dash/F25_Dash/
python3 hardware.py &
echo $! >> /tmp/hardware.pid

echo "Server launched"
