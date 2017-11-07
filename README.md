## Solar Car Data Acquisition and Display Project

### Doxygen Helpful Links
http://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/doxygen-howto.html

### Running display.sh
In the shell,
*	git clone https://github.com/christian7137/SolarCarProject.git
*	cd SolarCarProject/Display
* chmod +x display.sh
* ./display.sh

At this point, InfluxDB and Grafana should be installed and running upon boot. Next, configure InfluxDB settings. To do this, proceed on to <b>Setting up an InfluxDB Database</b>. 

### Setting up an InfluxDB database
After successful installation of InfluxDB, connect to the InfluxDB command line interface and create users and databases for our needs. The following steps will create one database `my_db` and an `admin` user as well as a `grafana` user. The grafana user will only be allowed to read data from `my_db`. Be sure to store your selected passwords.

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
