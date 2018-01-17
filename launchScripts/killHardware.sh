#!/bin/bash

#while IFS='' read -r line || [[ -n "$line" ]]; do
#    kill -s SIGINT "$(($line))"
#done < /tmp/hardware.pid

`/sbin/ip link set can0 down`

rm /tmp/hardware.pid
