import sys
import time
import datetime
from influxdb import InfluxDBClient
import UDPclient # C++ module

#import json_message

sensorGroupData = {
    # SENSOR LIST:
    # TSL2561T - Luminosity Sensor, Range of 0.1 to 40,000+ in Lux
    # BNO055 - Orientation Sensor, Range of -180 to +180 in degrees (turning clock-wise increases values)
    # BNO055 - Orientation Sensor, Range of -90 to +90 in degrees (increasing with increasing inclination)
    # BNO055 - Orientation Sensor, Range of 0 to 360 in degrees (turning clockwise increases values)
    # Simulated BPS  - State of Charge, Range of ??
    # Simulated - GPS Latitude, Range of ??
    # Simulated - GPS Longitude, Range of ??
    "OH" : ["LUMINOSITY", "ORI_PITCH", "ORI_ROLL", "ORI_YAW", "SOC", "GPS_LAT", "GPS_LONG"]
}

class UDP_Packet:
    timestamp = 0
    sensorGroup = ""
    sensorValues = []

    def __init__(self, timestamp, sensorGroup, sensorValues):
        assert isinstance(timestamp, int)
        assert isinstance(sensorGroup, str) and sensorGroup in sensorGroupData
        assert isinstance(sensorValues, list) and len(sensorValues) == len(sensorGroupData[sensorGroup])
        self.timestamp = timestamp   # Epoch timestamp in ms
        self.sensorGroup = sensorGroup
        for i in range(0, len(sensorValues)):
            self.sensorValues.append(float(sensorValues[i]))

    def writeToSD(self):
        return

    def writeToCSV(self):
        WRITE_PATH = "test_" + self.sensorGroup + ".csv"
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

    def log(self, client, session, runNo, interval):
        sensorValueDict = {}
        for i in range(0, len(sensorGroupData[self.sensorGroup])):
            sensorValueDict[sensorGroupData[self.sensorGroup][i]] = self.sensorValues[i]

        json_body = [
            {
                "measurement": session,
                "tags": {
                    "run": runNo,
                },
                "time": self.timestamp,
                "fields": sensorValueDict
            }
        ]
        # Write JSON to InfluxDB
        client.write_points(json_body)
        # Wait for next sample
        time.sleep(interval)

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
        session = "dev"
        now = datetime.datetime.now()
        runNo = now.strftime("%Y%m%d%H%M")
    print "Session: ", session
    print "runNo: ", runNo

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
        timestamp = 12345678
        sensorGroup = "OH"
        sensorValues = ["0.0", "1.0", "2.0", "3.0", "4.0", "5.0", "6.0"]
        entry = UDP_Packet(int(timestamp), str(sensorGroup), list(sensorValues))
        print entry.timestamp
        # Write to CSV
        entry.writeToCSV()
        
        # Log on InfluxDB
        entry.log(client, session, runNo, interval)

if __name__ == "__main__":
    main()