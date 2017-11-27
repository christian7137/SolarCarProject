# Solar Car Data Acquisition and Display Project

## Display System Documentation

### Configuring a Raspberry Pi
In the Raspberry Pi terminal,
*	git clone https://github.com/christian7137/SolarCarProject.git
*	cd SolarCarProject
* 	chmod +x sysinit
* 	./sysinit {pit|car}

If the 'pit' argument is used, InfluxDB and Grafana should be installed and running upon boot. Next, configure InfluxDB settings. To do this, proceed on to <b>Setting up an InfluxDB Database</b>. 

### Setting up an InfluxDB database
After successful installation of InfluxDB, connect to the InfluxDB command line interface and create users and databases as needed. The following steps will create one database `my_db` and an `admin` user as well as a `grafana` user. The grafana user will only be allowed to read data from `my_db`. This configuration is required to use the existing display-side python script.

```
$ influx
Connected to http://localhost:8086 version 1.3.7
InfluxDB shell version: 1.3.7
> CREATE DATABASE my_db
> CREATE USER admin WITH PASSWORD 'password123' WITH ALL PRIVILEGES
> CREATE USER grafana WITH PASSWORD 'letmeread123'
> GRANT READ ON my_db TO grafana
> exit
```

### Setting up the Grafana interface
After the successful installation of Grafana, you should be able to reach the Grafana dashboard at `http://monitoring-host:3000`, where `monitoring-host` is the IP of the device hosting the Grafana service. In the existing code, this IP is 192.168.0.111. The default login is `admin` with password `admin`.

Logged in to the Grafana dashboard, go to "Data Sources" and create a new source pointing to your InfluxDB database, providing the credentials that were chosen earlier.

### Setting InfluxDB as a Grafana data source
To query InfluxDB for data from the in-car acquisition system, you must go to "Data Sources" in the Grafana settings and point to the existing InfluxDB database.  
*	Name the data source a clear, descriptive name. During development, the name "UTSVT" was used.
*	Set Type to "InfluxDB".  

In HTTP Settings,
*	Set URL to "http://localhost:8086".
*	Set Access to "proxy".

In HTTP Auth,
*	Set Database to "my_db" unless changed when setting up the InfluxDB database.
*	Set User to "grafana".
*	Set Password to "letmeread123".

No other data source seetings are needed to successfully query InfluxDB.

### Using the display side logger
To actively look for UDP packets on the wireless network, you must run a script. In the 'Display' directory of this repository, run the shell script 'run.sh'. This script will search for and log UDP packets sent to the device's IP address.  

Before powering off the device, be sure to run 'quit.sh' in the 'Display' directory.  

Christian's note: It's probably better to make quit and run command line arguments of the base directory script.

### Adding users to Grafana
As a Grafana admin, go to `http://monitoring-host:3000/org/users`, where `monitoring-host` is the IP of the device hosting the Grafana service. In this menu, you can add or remove users as needed.

### Adding sensors to the display side logger
Make changes to 'config_pit.txt'. 

Christian's note: We need to finalize the format for this config file.

### Accessing display side data logs
After running the display side logger for the first time, the 'Display' directory of this repository will contain a directory 'CSV' with the stored logs of the UDP packet data. Each file contains the date of the logger run and the type of sensor data logged. The sensor ID matches the ID set in 'config_pit.txt'.

### Grafana and InfluxDB documentation
To read more about Grafana and InfluxDB, go to the official documentation for the software at:

https://docs.influxdata.com/influxdb/v1.3/
http://docs.grafana.org/

### Doxygen Helpful Links
http://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/doxygen-howto.html
