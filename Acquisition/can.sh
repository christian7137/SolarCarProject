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
sudo sh -c "echo 'dtparam=i2c_arm=on' >> /boot/config.txt"
sudo sh -c "echo 'dtoverlay=i2c-rtc,ds3231' >> /boot/config.txt"
sudo sh -c "echo 'dtoverlay=mcp2515-can0' >> /boot/config.txt"
sudo sh -c "echo 'dtparam=oscillator=16000000' >> /boot/config.txt"
sudo sh -c "echo 'dtparam=interrupt=13' >> /boot/config.txt"
sudo sh -c "echo 'dtparam=spimaxfrequency=100000' >> /boot/config.txt"
# To use serial port /dev/ttyAMM0 for BN05
sudo sh -c "echo 'dtoverlay=pi3-disable-bt' >> /boot/config.txt"
sudo sh -c "echo 'enable_uart=1' >> /boot/config.txt"

cd CAN
make
cd ..

echo "Rebooting . . ."
sudo reboot

# If not using the svtcan program, it is sometimes necessary to enable CAN after rebooting. The svtcan program automatically does this.
# sudo ifconfig can0 down		# To disconnect CAN
# sudo ip link set can0 type can bitrate 100000 triple-sampling off restart-ms 100	# To configure CAN
# sudo ifconfig can0 up			# To reconnect CAN