#!/bin/bash

wpa_supplicant -B -iwlan0 -c/etc/wpa_supplicant/wpa_supplicant.conf
dhcpcd wlan0
dhcpcd eth0
