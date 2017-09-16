#!/bin/bash

# install Grafana
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' grafana|grep "install ok installed")
echo Checking for Grafana: $PKG_OK
if [ "" == "$PKG_OK" ]; then
	echo "Grafana is not installed. Installing Grafana . . ."
	wget https://github.com/fg2it/grafana-on-raspberry/releases/download/v4.4.3/grafana_4.4.3_armhf.deb
	sudo dpkg -i grafana_4.4.3_armhf.deb
	sudo apt-get install -f
	
	# enable run at boot?
	echo "Enabling Grafana at Boot . . ."
	sudo systemctl enable grafana-server
	sudo systemctl start grafana-server
	echo "Done!"
fi

# install InfluxDB
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' influxdb|grep "install ok installed")
echo Checking for InfluxDB: $PKG_OK
if [ "" == "$PKG_OK" ]; then
	echo "InfluxDB is not installed. Installing InfluxDB . . ."
	wget https://dl.influxdata.com/influxdb/releases/influxdb-1.3.5_linux_armhf.tar.gz
	tar xvfz influxdb-1.3.5_linux_armhf.tar.gz
	
	# configure influxdb and finish installing (Maybe)
	echo "Readying InfluxDB . . ."
	cd influxdb-1.3.5_linux_armhf
	./configure
	make
	sudo make install
	
	# enable run at boot?
	echo "Enabling InfluxDB at Boot . . ."
	sudo systemctl enable influx
	sudo systemctl start influx
	echo "Done!"
fi

echo "Script is done!"
# Before being able to run script, need to: chmod +x /path/to/display.sh
