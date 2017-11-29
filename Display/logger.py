import sys
import os
import time
import datetime
from influxdb import InfluxDBClient
import UDPclient # C++ module
import geohash

# Sensor label dictionary generated using setUpSensorDict() and config_pit.txt
sensorDict = {}

class UDP_Packet:
	
	sensorData = []

	# Constructor
	def __init__(self, sensorData):
		assert isinstance(sensorData, list)
		for i in range(0, len(sensorData)):
			if sensorData[i] == "None":
				self.sensorData.append("None")
				continue
			else:
				self.sensorData.append(sensorData[i].split(','))

	# Writes data to sensor-specific CSV files
	def writeToCSV(self, today):
		for i in range(0, len(self.sensorData)):
			if self.sensorData[i] == "None":	# skip if sensor has no data
				continue
			else:
				WRITE_PATH = "CSV/" + today + "_SID" + self.sensorData[i][1] + ".csv"
				try:
					CSV = open(WRITE_PATH, 'r')
					CSV.close()
				except IOError: # if file does not exist, create file and add header row
					CSV = open(WRITE_PATH, 'w')
					header = "TIMESTAMP, "
					sensorLabels = sensorDict.get(self.sensorData[i][1])
					for j in range(0, len(sensorLabels)):
						header += sensorLabels[j] + ", "
					header+= "\n"
					CSV.write(header)
					print("CREATED " + WRITE_PATH + "\n")
					CSV.close()
				CSV = open(WRITE_PATH, 'a')  # open writeFile in append mode
				line = str(self.sensorData[i][0]) + ", "
				for j in range(2, len(self.sensorData[i])):
					line += str(self.sensorData[i][j]) + ", "
				line += "\n"
				CSV.write(line)
				print("WROTE TO " + WRITE_PATH + "\n")
				CSV.close()

	# Sends data to InfluxDB
	def log(self, client, session, runNo, interval):    # edit for JSON class
		for i in range(0, len(self.sensorData)):
			if (self.sensorData[i] == "None"):	# skip if sensor has no data
				continue
			else:
				print "TIMESTAMP: " + self.sensorData[i][0]
				tags = {"run": runNo}
				fields = {}
				if i == 3: # special insertion for GPS data
					rawLatitude = self.sensorData[i][2]	# assuming value is North of equator
					assert(len(rawLatitude) >= 4)	# assert valid latitude
					minutesIndex = rawLatitude.find('.') - 2
					if (minutesIndex < 0):	# no decimal found
						minutesIndex = 2
					latitude = float(rawLatitude[0:minutesIndex]) + float(rawLatitude[minutesIndex:]) / 60
					#print "LAT: " + str(rawLatitude) + " | " + str(rawLatitude[0:minutesIndex]) + " | " + str(rawLatitude[minutesIndex:]) + " | " + str(latitude) + " (" + str(minutesIndex) + ")"
					rawLongitude = self.sensorData[i][3]	# assuming value is West of Prime Meridian (hence -1 multiplication)
					assert(len(rawLongitude) >= 4)	# assert valid longitude
					minutesIndex = rawLongitude.find('.') - 2
					if (minutesIndex < 0):	# no decimal found
						minutesIndex = 2	
					longitude = -1 * (float(rawLongitude[0:minutesIndex]) + float(rawLongitude[minutesIndex:]) / 60)
					geohashValue = str(geohash.encode(latitude, longitude))
					#print "LONG: " + str(rawLongitude) + " | " + str(rawLongitude[0:minutesIndex]) + " | " + str(rawLongitude[minutesIndex:]) + " | " + str(longitude) + " (" + str(minutesIndex) + ")"
					#print "GEO:" + geohashValue
					tags["geohash"] = geohashValue
					fields["metric"] = 1
				else:
					for j in range(2, len(self.sensorData[i])):
						if (self.sensorData[i][j] == ""):	# skip if sensor value is empty
							continue
						else:
							fields[sensorDict.get(self.sensorData[i][1])[j - 2]] = self.sensorData[i][j]	# create dictionary for JSON body
				json_body = [
								{
									"measurement": session,
									"tags": tags,
									"time": time.ctime(float(self.sensorData[i][0])),
									"fields": fields
								}
							]
				# Write JSON to InfluxDB
				client.write_points(json_body)
				print "LOGGED TO INFLUXDB\n"	

	# Clears sensorData list for reuse
	def clearData(self):
		self.sensorData[:] = [] # empty list
		print "CLEARED UDP PACKET"
		print "-------------------------------------\n"

# Creates sensor label dictionary
def setUpSensorDict():
	READ_FILE = "config_pit.txt"
	f = open(READ_FILE, 'r')
	for line in f:
		if line == "\n" or line[0] == "#":    # skips empty or commented lines
			continue
		else:
			sensor = line.rstrip().split('\t')	# remove trailing whitespace and splits by tab character
			sensor = filter(None, sensor)	# remove all empty entries in list
			sensorDict[sensor[0]] = sensor[1:]
	f.close()
	return sensorDict

def setUpInfluxDB():
	# Set these variables, influxDB should be localhost on Pi
	host = "localhost"
	port = 8086
	user = "admin"
	password = "password123"

	# The database we created
	dbname = "my_db"
	# Sample period (s)
	interval = 5
	# Allow user to set session and runno via args otherwise auto-generate
	if len(sys.argv) > 1:
		if (len(sys.argv) < 3):
			print "Must define session and runNo!!"
			sys.exit()
		else:
			session = sys.argv[1]
			runNo = sys.argv[2]
	else:
		session = "UTSVT"
		now = datetime.datetime.now()
		runNo = now.strftime("%Y%m%d%H%M")
	print "Session: ", session
	print "RunNo: ", runNo

	# Create the InfluxDB object
	client = InfluxDBClient(host, port, user, password, dbname)
	return client, session, runNo, interval

def main():
	# Config Dictionary
	setUpSensorDict()
	# Make sure CSV directory exists
	if not os.path.exists("CSV"):
		os.makedirs("CSV")
		print "CREATED CSV DIRECTORY\n"
	print "SUCCESSFULLY SET UP SENSOR DICTIONARY\n"
	# Set up InfluxDB Server
	client, session, runNo, interval = setUpInfluxDB()
	print "SUCCESSFULLY SET UP INFLUXDB CLIENT\n"
	# Set up UDP Client
	if (not UDPclient.setUpUDPclient()):
		print "FAILED TO SET UP UDP CLIENT\n"
	else:
		print "SUCCESSFULLY SET UP UDP CLIENT\n"
		today = datetime.datetime.now().strftime("%Y_%m_%d")
		while (1):
			print "WAITING FOR PACKET"
			packet = UDP_Packet(UDPclient.pollUDPclient())
			#packet = UDP_Packet(["None", "None", "None", "12345,4,3008.992,9738.0774"])	# test geohash value
			print "RECEIVED PACKET\n"
			packet.writeToCSV(today)
			packet.log(client, session, runNo, interval)    # log to InfluxDB
			packet.clearData()     # log on InfluxDB
		UDPclient.closeUDPclient()

if __name__ == "__main__":
	main()