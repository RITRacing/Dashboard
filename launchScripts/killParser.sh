#!/bin/bash

while IFS='' read -r line || [[ -n "$line" ]]; do
    kill -s SIGINT "$(($line))"
done < /tmp/parser.pid

rm /tmp/parser.pid
