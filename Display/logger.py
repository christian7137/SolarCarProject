import sys
import time
import datetime
from influxdb import InfluxDBClient
import UDPclient # C++ module

# SENSOR LIST:
# TSL2561T - Luminosity Sensor, Range of 0.1 to 40,000+ in Lux
# BNO055 - Orientation Sensor, Range of -180 to +180 in degrees (turning clock-wise increases values)
# BNO055 - Orientation Sensor, Range of -90 to +90 in degrees (increasing with increasing inclination)
# BNO055 - Orientation Sensor, Range of 0 to 360 in degrees (turning clockwise increases values)
# Simulated BPS  - State of Charge, Range of ??
# Simulated - GPS Latitude, Range of ??
# Simulated - GPS Longitude, Range of ??

sensorID = {
		1 : ["SOC0", "SOC1", "SOC2", "SOC3"],
                2 : ["ANG0", "ANG1", "ORI0", "ORI1", "ORI2",  "ACC0", "ACC1", "ACC2", "MAG0", "MAG1", "MAG2"],
                3 : ["LUX"],
                4 : ["LAT", "LONG"]
            }

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

    def writeToCSV(self):   # modify for new json message format
        WRITE_PATH = "test.csv"
        try:
            CSV = open(WRITE_PATH, 'r')
            CSV.close()
        except IOError:
            CSV = open(WRITE_PATH, 'w')
            header = "TIMESTAMP, "
            for i in range(0, len(sensorGroupData[self.sensorGroup])):
                header += sensorGroupData[self.sensorGroup][i] + ", "
            header+= "\n"
            CSV.write(header)
            CSV.close()
        CSV = open(WRITE_PATH, 'a')  # open writeFile in append mode
        line = str(self.timestamp) + ", "
        for i in range(0, len(self.sensorValues)):
            line += str(self.sensorValues[i]) + ", "
        line += "\n"
        CSV.write(line)
        print("WROTE TO " + WRITE_PATH + "\n")
        CSV.close()
        return

    def log(self, client, session, runNo, interval):    # edit for JSON class
        for i in range(0, len(self.sensorData)):
            if (self.sensorData[i] == "None"):
                continue
            else:
		print "TIMESTAMP: "
		print self.sensorData[i][0]
                sensorLog = {}
                for j in range(2, len(self.sensorData[i])):
                    sensorLog[sensorID.get(int(self.sensorData[i][1]))[j - 2]] = self.sensorData[i][j]
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
    # Set up InfluxDB Server
    client, session, runNo, interval = setUpInfluxDB()
    print "SUCCESSFULLY SET UP INFLUXDB CLIENT\n"
    # Set up UDP Client
    if (not UDPclient.setUpUDPclient()):
        print "FAILED TO SET UP UDP CLIENT\n"
    else:
        print "SUCCESSFULLY SET UP UDP CLIENT\n"
        while (1):
            print "WAITING FOR PACKET"
            packet = UDP_Packet(UDPclient.pollUDPclient())
            #packet.writeToCSV()
            packet.log(client, session, runNo, interval)    # log to InfluxDB
	    packet.clearData()     # log on InfluxDB
        UDPclient.closeUDPclient()

if __name__ == "__main__":
    main()
