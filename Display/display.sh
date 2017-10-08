#!/bin/bash

# install apt-transport-https
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' apt-transport-https|grep "install ok installed")
echo Checking for apt-transport-https: $PKG_OK
if [ "" == "$PKG_OK" ]; then
	echo "apt-transport-https is not installed. Installing . . ."
	sudo apt-get install apt-transport-https
	echo "Done!"
fi

# install InfluxDB
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' influxdb|grep "install ok installed")
echo Checking for InfluxDB: $PKG_OK
if [ "" == "$PKG_OK" ]; then
	echo "InfluxDB is not installed. Installing InfluxDB . . ."
	wget -O - https://repos.influxdata.com/influxdb.key | sudo apt-key add -
	echo "deb https://repos.influxdata.com/debian jessie stable" | sudo tee /etc/apt/sources.list.d/influxdb.list
	sudo apt-get update && sudo apt-get install influxdb
	
	# enable run at boot?
	echo "Enabling InfluxDB at Boot . . ."
	sudo systemctl daemon-reload
	sudo systemctl enable influxdb.service
	sudo systemctl start influxdb.service
	echo "Done!"
fi

# install Grafana
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' grafana|grep "install ok installed")
echo Checking for Grafana: $PKG_OK
if [ "" == "$PKG_OK" ]; then
	echo "Grafana is not installed. Installing Grafana . . ."
	echo "deb https://dl.bintray.com/fg2it/deb jessie main" | sudo tee /etc/apt/sources.list.d/grafana-fg2it.list 
	sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 379CE192D401AB61
	sudo apt-get update && sudo apt-get install grafana
	
	# enable run at boot?
	echo "Enabling Grafana at Boot . . ."
	sudo systemctl daemon-reload
	sudo systemctl enable grafana-server.service
	sudo systemctl start grafana-server.service
	echo "Done!"
fi

influx
expect ">"
send CREATE DATABASE my_db
expect ">"
send CREATE USER admin WITH PASSWORD 'password123' WITH ALL PRIVILEGES
expect ">"
send CREATE USER grafana WITH PASSWORD 'letmeread123'
expect ">"
send GRANT READ ON my_db TO grafana
expect ">"
send exit

echo "Script is done!"
# Before being able to run script, need to: chmod +x /path/to/display.sh
