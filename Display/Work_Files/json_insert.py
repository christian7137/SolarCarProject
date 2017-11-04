import json
import math
import requests
import sys
import datetime

from time import sleep
from influxdb import InfluxDBClient

IP = "localhost"        # The IP of the machine hosting your influxdb instance
PORT = 8086				# The port for InfluxDB, defaults to 8086
DB = "my_db"               # The database to write to, has to exist
USER = "admin"             # The influxdb user to authenticate with
PASSWORD = "password123"  # The password of that user
TIME = 0.02                  # Delay in seconds between two consecutive updates
STATUS_MOD = 50            # The interval between printing number of points

n = 0
client = InfluxDBClient(IP, PORT, USER, PASSWORD, DB)
client.create_database(DB)
client.switch_database(DB)
while True:
	for d in range(0, 360):
                TIMESTAMP = datetime.datetime.now() # Time associated with data inserted
		json_body = [
			{
				"measurement": "cpu_load_short",
				"tags": {
					"host": "server01",
					"region": "us-west"
				},
				"time": (int(TIMESTAMP.strftime('%s'))) * 1000000000,
				"fields" : {
					"sine_wave_test%d" % d: math.sin(math.radians(d))
				}
			}
		]
		client.write_points(json_body)
		result = client.query('select sine_wave_test from cpu_load_short;')
		# print("Result: {0}".format(result))
		n += 1
		sleep(TIME)
		if n % STATUS_MOD == 0:
			print '%d points inserted.' % n
			
