#!/bin/bash
# This enables the CAN socket and starts the python 3 backend

echo "Launching backend"
sleep 1
sudo /sbin/ip link set can0 up type can bitrate 500000

python3 /home/dash/f26dash/backend/src/dashboard.py /home/dash/f26dash/backend/tests/endcut.csv &
echo $! >> /tmp/backend.pid

echo "Backend launched"
