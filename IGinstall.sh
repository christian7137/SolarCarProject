#!/bin/bash

# install grafana
echo "Installing Grafana"
wget https://github.com/fg2it/grafana-on-raspberry/releases/download/v4.4.3/grafana_4.4.3_armhf.deb
sudo dpkg -i grafana_4.4.3_armhf.deb
sudo apt-get install -f
# make it run at boot
echo "Enabling Run at Boot"
sudo systemctl enable grafana-server
sudo systemctl start grafana-server
# install influxdb
echo "Installing InfluxDB"
wget https://dl.influxdata.com/influxdb/releases/influxdb-1.3.5_linux_armhf.tar.gz
tar xvfz influxdb-1.3.5_linux_armhf.tar.gz
# Need to finish installing InfluxDB and start it at boot
# Best Guess -- finish influx installation
echo "Readying InfluxDB"
cd influxdb-1.3.5_linux_armhf
./configure
make
sudo make install
# make it run at boot
echo "Enabling Run at Boot"
sudo systemctl enable influx
sudo systemctl start influx


# Need to do this prior to running script: chmod -x /path/to/IGinstall.sh