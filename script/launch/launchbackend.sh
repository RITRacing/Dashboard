#!/bin/bash
# This enables the CAN socket and starts the backend

echo "Launching backend"
sleep 1
/sbin/ip link set can0 up type can bitrate 500000

#python3 /home/dash/f26dash/backend/src/dashboard.py /home/dash/f26dash/backend/tests/endcut.csv &
/home/dash/f26dash/backend/dashboard -m vehicle &

echo "Backend launched"
