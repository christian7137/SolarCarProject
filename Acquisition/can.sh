#!/bin/bash

# install can-utils
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' can-utils|grep "install ok installed")
echo Checking for can-utils: $PKG_OK
if [ "" == "$PKG_OK" ]; then
	echo "can-utils is not installed. Installing . . ."
	sudo apt-get install can-utils
	#echo "Done!"
fi

# Purge fake-hwclock
sudo apt-get purge fake-hwclock

# Append configuration settings to config.txt
sudo echo "dtparam=i2c_arm=on" >> /boot/config.txt
sudo echo "dtoverlay=i2c-rtc,ds3231" >> /boot/config.txt
sudo echo "dtoverlay=mcp2515-can0" >> /boot/config.txt
sudo echo "dtparam=oscillator=16000000" >> /boot/config.txt
sudo echo "dtparam=interrupt=13" >> /boot/config.txt
sudo echo "dtparam=spimaxfrequency=100000" >> /boot/config.txt
# To use serial port /dev/ttyAMM0 for BN05
sudo echo "dtoverlay=pi3-disable-bt" >> /boot/config.txt
sudo echo "enable_uart=1" >> /boot/config.txt

echo "Rebooting . . ."
sudo reboot