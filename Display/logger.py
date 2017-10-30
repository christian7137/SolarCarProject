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
                0 : ["LUX"], 
                1 : ["ORI0", "ORI1", "OR2" "ACC0", "ACC1", "ACC2", "MAG0", "MAG1", "MAG2"],
                2 : ["SOC"],
                3 : ["LAT", "LONG"]
            }

class UDP_Packet:
    
    sensorData = []

    def __init__(self, sensorData):
        assert isinstance(sensorData, list)
        for i in range(0, len(sensorData)):
            if sensorData[i] == "None":
                self.SensorData[i] = "None"
                continue
            else:
                self.sensorData[i] = sensorData[i].split(',')

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
                sensorLog = {}
                for j in range(0, len(self.sensorData[i])):
                    sensorLog[sensorID[self.sensorData[i][1]][j]] = self.sensorData[i][j + 2]
                json_body = [
                    {
                        "measurement": session,
                        "tags": {
                            "run": runNo,
                        },
                        "time": self.sensorData[i][0]
                        "fields": sensorLog
                    }
                ]
                # Write JSON to InfluxDB
                client.write_points(json_body)

    def clearData(self):    # edit for json format
        self.sensorData[:] = []

def setUpInfluxDB():
    # Set these variables, influxDB should be localhost on Pi
    host = "localhost"
    port = 8086
    user = "root"
    password = "root"

    # The database we created
    dbname = "testDB"
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
            packet = UDP_Packet(UDPclient.pollUDPclient())
            packet.writeToCSV()
            packet.log(client, session, runNo, interval)    # write to CSV
            packet.clearData()     # log on InfluxDB
        UDPclient.closeUDPclient()

if __name__ == "__main__":
    main()