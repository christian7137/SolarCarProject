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

### Setting up an InfluxDB database
After successful installation of InfluxDB, connect to the InfluxDB command line interface and create users and databases for our needs. You may alternatively also use the web frontend to do so. The following steps will create one database `my_db` and an `admin` user as well as a `grafana` user. The grafana user will only be allowed to read data from `my_db`. Be sure to store your selected passwords.

```
$ influx
Connected to http://localhost:8086 version 0.13
InfluxDB shell version: 0.13
> CREATE DATABASE my_db
> CREATE USER admin WITH PASSWORD 'password123' WITH ALL PRIVILEGES
> CREATE USER grafana WITH PASSWORD 'letmeread123'
> GRANT READ ON my_db TO grafana
> exit
```

### Setting up the Grafana interface
After the successful of Grafana, you should be able to reach the Grafana dashboard at `http://monitoring-host:3000`, where `monitoring-host` is the IP of the device hosting the Grafana service. The default login is `admin` with password `admin`.

Logged in to the Grafana dashboard, go to "Data Sources" and create a new source pointing to your InfluxDB database, providing the credentials that were chosen earlier. With this, you should be able to display the data stored in InfluxDB using Grafana's graphs and tables on an arranged dashboard.
