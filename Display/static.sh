#!/bin/bash

# Configure to set IP
sudo echo "interface eth0" >> /etc/dhcpcd.conf
sudo echo "static ip_address=192.168.0.111" >> /etc/dhcpcd.conf

# Reboot to apply changes
echo ". . . Ethernet IP is 192.168.0.111"
sudo ifconfig eth0 down
sudo ifconfig eth0 up
#
