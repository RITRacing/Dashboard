#!/bin/bash

while IFS='' read -r line || [[ -n "$line" ]]; do
    kill -s SIGINT "$(($line))"
done < /tmp/frontend.pid

kill `pgrep dash.bin` &> /dev/null

rm /tmp/frontend.pid
