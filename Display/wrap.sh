#!/bin/bash

# run in the same directory as all needed files.
# chmod +x wrap.sh
# sudo ./wrap.sh

# install python-dev
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' python-dev|grep "install ok installed")
echo Checking for python-dev: $PKG_OK
if [ "" == "$PKG_OK" ]; then
	echo "python-dev is not installed. Installing . . ."
	sudo apt-get install python dev
	echo ". . . installation done!"
fi

sudo rm -rf build
sudo python setup.py build 
sudo python setup.py install 

echo ". . . wrapping done!"
