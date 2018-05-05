#!/bin/bash

DISPLAY=:0 unclutter -idle 0 & #get rid of mouse
echo $! >> /tmp/frontend.pid #echo printout to this process

DISPLAY=:0 xset s off #turn off the screensaver
DISPLAY=:0 xset -dpms #disable energy star features
DISPLAY=:0 xset s noblank #set the screensaver to a pattern
