# SolarCarProject
Solar Car Project

## Documentation

### Doxygen Helpful Links
http://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/doxygen-howto.html

### Running display.sh
In the shell,
*	git clone https://github.com/christian7137/SolarCarProject.git
    * Alternately, just load display.sh onto the RPi3.
*	cd SolarCarProject
*	chmod +x display.sh
*	dos2unix display.sh
    * sudo apt-get install dos2unix (if not installed, temporary)
*	./display.sh

At this point, InfluxDB and Grafana should be installed and running upon boot. Next, configure InfluxDB settings.
*	We need to test this first, I haven’t been able to recreate my settings successfully yet.
*	We might even be able to do this in the script.

### Manually installing InfluxDB on the Raspberry Pi 2 or 3
These steps will install InfluxDB and Grafana on a Raspberry Pi 2 or 3. Please note that the older Raspberry Pi 1 is not suited for this due to its ARMv6 architecture.
The following instructions expect a Debian/Raspbian based OS, and all boxed text should be entered into the shell.

Before we can start, it's necessary to install an apt extension for https-capable repositories:

`sudo apt-get update && sudo apt-get install apt-transport-https`


#### InfluxDB
InfluxDB can be installed through the official repository. Add the key to your system, then add the repository as an apt source, then install InfluxDB using the following shell entries:

```
wget -O - https://repos.influxdata.com/influxdb.key | sudo apt-key add -
echo "deb https://repos.influxdata.com/debian jessie stable" | sudo tee /etc/apt/sources.list.d/influxdb.list
sudo apt-get update && sudo apt-get install influxdb
```

After the installation was successful, you can enable and start InfluxDB as any other program:

```
sudo systemctcl daemon-reload
sudo systemctl enable influxdb.service
sudo systemctl start influxdb.service
```

#### Grafana
Grafana does not officially support the ARM architecture, but binaries have been provided to fix this issue.
Install Grafana using the following shell entries:

```
echo "deb https://dl.bintray.com/fg2it/deb jessie main" | sudo tee /etc/apt/sources.list.d/grafana-fg2it.list 
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 379CE192D401AB61
sudo apt-get update && sudo apt-get install grafana
```

Again, enable and start the program through systemctl:

```
sudo systemctl daemon-reload
sudo systemctl enable grafana-server.service
sudo systemctl start grafana-server.service
```
