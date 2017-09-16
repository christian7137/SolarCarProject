# SolarCarProject
Solar Car Project

## Running display.sh
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

