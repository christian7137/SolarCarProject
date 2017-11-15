import sys
import os
import time
import datetime
from influxdb import InfluxDBClient
import UDPclient # C++ module

sensorDict = {}

class UDP_Packet:
    
    sensorData = []

    def __init__(self, sensorData):
        assert isinstance(sensorData, list)
	for i in range(0, len(sensorData)):
            if sensorData[i] == "None":
                self.sensorData.append("None")
                continue
            else:
                self.sensorData.append(sensorData[i].split(','))

    def writeToCSV(self, today):   # modify for new json message format
        for i in range(0, len(self.sensorData)):
            if self.sensorData[i] == "None":
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

    def log(self, client, session, runNo, interval):    # edit for JSON class
        for i in range(0, len(self.sensorData)):
            if (self.sensorData[i] == "None"):
                continue
            else:
        		print "TIMESTAMP: "
        		print self.sensorData[i][0]
                        sensorLog = {}
                        for j in range(2, len(self.sensorData[i])):
                        	if (self.sensorData[i][j] == ""):
					continue
				else:
					sensorLog[sensorDict.get(self.sensorData[i][1])[j - 2]] = self.sensorData[i][j]
        		json_body = [
                            {
                                "measurement": session,
                                "tags": {
                                    "run": runNo,
                                },
                                "time": time.ctime(float(self.sensorData[i][0])),
                                "fields": sensorLog
                            }
                        ]
                        # Write JSON to InfluxDB
                        client.write_points(json_body)
        		print("LOGGED TO INFLUXDB\n")	

    def clearData(self):
        print("CLEARED UDP PACKET\n")
	self.sensorData[:] = [] # clear sensorData

def setUpSensorDict():
    READ_FILE = "config_pit.txt"
    f = open(READ_FILE, 'r')
    for line in f:
        if line == "\n" or line[0] == "#":    # empty or commented line
            continue
        else:
            sensor = line.rstrip().split('\t')
            sensor = filter(None, sensor)
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
    packet = UDP_Packet(["None", "100,2,0,1,2,3,4,5,6,7,8,9,10", "None", "None"])
    today = datetime.datetime.now().strftime("%Y_%m_%d")
    # Make sure CSV directory exists
    if not os.path.exists("CSV"):
        os.makedirs("CSV")
        print "CREATED CSV DIRECTORY\n"
    packet.writeToCSV(today)
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
            packet.writeToCSV(today)
            packet.log(client, session, runNo, interval)    # log to InfluxDB
	    packet.clearData()     # log on InfluxDB
        UDPclient.closeUDPclient()

if __name__ == "__main__":
    main()
