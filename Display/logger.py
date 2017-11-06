import sys
import time
import datetime
from influxdb import InfluxDBClient
import UDPclient # C++ module

# SENSOR LIST:
# 1 : Simulated BPS  - State of Charge
# 2 : BNO055 - Orientation Sensor
# 3 : TSL2561T - Luminosity Sensor
# 4 : Simulated - GPS

# value labels for each sensor
sensorID = {
                1 : ["SOC0", "SOC1", "SOC2", "SOC3"],
                2 : ["ANG0", "ANG1", "ACC0", "ACC1", "ACC2",  "GYR0", "GYR1", "GYR2", "MAG0", "MAG1", "MAG2"],
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

    def writeToCSV(self, today):   # modify for new json message format
        for i in range(0, len(self.sensorData)):
            if self.sensorData[i] == "None":
                continue
            else:
                WRITE_PATH = "CSV/" + today + "_SID" + self.sensorData[i][1]
                try:
                    CSV = open(WRITE_PATH, 'r')
                    CSV.close()
                except IOError: # if file does not exist, create file and add header row
                    CSV = open(WRITE_PATH, 'w')
                    header = "TIMESTAMP, "
                    sensorLabels = sensorID.get(int(self.sensorData[i][1]))
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
        today = datetime.datetime.now().strftime("%Y_%m_%d")
        example = UDP_Packet(["None", "1234567890,2,360,360,270,270,270,180,180,180,90,90,90", "None", "None"])
        example.writeToCSV(today)
        return
        while (1):
            print "WAITING FOR PACKET"
            packet = UDP_Packet(UDPclient.pollUDPclient())
            packet.writeToCSV(today)
            packet.log(client, session, runNo, interval)    # log to InfluxDB
	    packet.clearData()     # log on InfluxDB
        UDPclient.closeUDPclient()

if __name__ == "__main__":
    main()
