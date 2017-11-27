import sys
import time
import random
import datetime
from influxdb import InfluxDBClient

class UDP_Packet:
    #DATA FORMAT (OPEN HOUSE): TIMESTAMP    LUMINOSITY  ORI_PITCH	ORI_ROLL	ORI_YAW	SOC	GPS_LAT	GPS_LONG
    timestamp = 0   # Epoch timestamp
    luminosity = 0.0    # TSL2561T - Luminosity Sensor, Range of 0.1 to 40,000+ in Lux
    ori_pitch = 0.0 # BNO055 - Orientation Sensor, Range of -180 to +180 in degrees (turning clock-wise increases values)
    ori_roll = 0.0  # BNO055 - Orientation Sensor, Range of -90 to +90 in degrees (increasing with increasing inclination)
    ori_yaw = 0.0   # BNO055 - Orientation Sensor, Range of 0 to 360 in degrees (turning clockwise increases values)
    soc = 0.0        # Simulated - State of Charge, Range of ??
    gps_lat = 0.0    # Simulated - GPS Latitude, Range of ??
    gps_long = 0.0   # Simulated - GPS Longitude, Range of ??

    def __init__(self, packet):
        assert isinstance(packet, list)
        assert len(packet) == 8
        self.timestamp = packet[0]   # in ??
        self.luminosity = packet[1]  # in lux
        self.ori_pitch = packet[2]   # in degrees
        self.ori_roll = packet[3]    # in degrees
        self.ori_yaw = packet[4]     # in degrees
        self.soc = packet[5]         # in ??
        self.gps_lat = packet[6]     # in ??
        self.gps_long = packet[7]    # in ??

    def writeToCSV(self, writeFile):
        CSV = open(writeFile, 'a')  # open writeFile in append mode
        line = self.timestamp + ", " + self.luminosity + ", " + self.ori_pitch + ", " + self.ori_roll + ", " + self.ori_yaw + ", " + self.soc + ", " + self.gps_lat + ", " + self.gps_long + "\n"
        CSV.write(line)
        print("WROTE TO " + writeFile + "\n")
        CSV.close()
        return

    def log(self, client, session, runNo, interval):
        json_body = [
            {
                "measurement": session,
                "tags": {
                    "run": runNo,
                },
                "time": self.timestamp,
                "fields": {
                    "luminosity": self.luminosity,
                    "ori_pitch": self.ori_pitch,
                    "ori_roll": self.ori_roll,
                    "ori_yaw": self.ori_yaw,
                    "soc": self.soc,
                    "gps_lat": self.gps_lat,
                    "gps_long": self.gps_long
                }
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
    #client, session, runNo, interval = setUpInfluxDB()

    #
    packet = "0\t1\t2\t3\t4\t5\t6\t7"
    print packet.split('\t')
    entry = UDP_Packet(packet.split('\t'))
    print entry.timestamp
    # Write to CSV
    WRITE_PATH = "test.csv"
    entry.writeToCSV(WRITE_PATH)
    #
    # # Log on InfluxDB
    # entry.log(client, session, runNo, interval)

if __name__ == "__main__":
    main()