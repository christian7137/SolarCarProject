#!/bin/bash

# install grafana
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' grafana|grep "install ok installed")
echo Checking for grafana: $PKG_OK
if [ "" == "$PKG_OK" ]; then
	echo "Grafana is not installed. Installing Grafana . . ."
	wget https://github.com/fg2it/grafana-on-raspberry/releases/download/v4.4.3/grafana_4.4.3_armhf.deb
	sudo dpkg -i grafana_4.4.3_armhf.deb
	sudo apt-get install -f

	# make it run at boot
	echo "Enabling Grafana at Boot . . ."
	sudo systemctl enable grafana-server
	sudo systemctl start grafana-server
	echo "Done!"
fi

# install grafana
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' influxdb|grep "install ok installed")
echo Checking for influxdb: $PKG_OK
if [ "" == "$PKG_OK" ]; then
	echo "InfluxDB is not installed. Installing InfluxDB . . ."
	echo "Installing InfluxDB"
	wget https://dl.influxdata.com/influxdb/releases/influxdb-1.3.5_linux_armhf.tar.gz
	tar xvfz influxdb-1.3.5_linux_armhf.tar.gz

	# Configure Influx and Finish Installing ... Maybe.
	echo "Readying InfluxDB . . ."
	cd influxdb-1.3.5_linux_armhf
	./configure
	make
	sudo make install
	
	# make it run at boot
	echo "Enabling InfluxDB at Boot . . ."
	sudo systemctl enable influx
	sudo systemctl start influx
	echo "Done!"
fi

# Need to do this prior to running script: chmod -x /path/to/IGinstall.sh