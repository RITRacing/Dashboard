#!/bin/bash
# This script will launch the frontend display.

echo "Launching the frontend"

xinit &


echo $! >> /tmp/frontend.pid
DISPLAY=:0 /home/dash/f26dash/frontend/dash_bin &
echo $! >> /tmp/frontend.pid

sleep 2


DISPLAY=:0 unclutter -idle 0 & #remove cursor
echo $! >> /tmp/frontend.pid 
DISPLAY=:0 xset s off #disable screensaver
DISPLAY=:0 xset -dpms #disable energy star features
DISPLAY=:0 xset s noblank 

echo "Frontend launched"
