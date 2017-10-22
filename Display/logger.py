import time
import sys
import random
import datetime
from influxdb import InfluxDBClient

# Set this variables, influxDB should be localhost on Pi
host = "localhost"
port = 8086
user = "admin"
password = "password123"

# The database we created
dbname = "my_db"
# Sample period (s)
interval = 1

# Allow user to set session and runno via args otherwise auto-generate
if len(sys.argv) > 1:
    if (len(sys.argv) < 3):
        print "Must define session and runNo!!"
        sys.exit()
    else:
        session = sys.argv[1]
        runNo = sys.argv[2]
else:
    session = "test_session"
    now = datetime.datetime.now()
    runNo = now.strftime("%Y%m%d%H%M")

print "Session: ", session
print "runNo: ", runNo

# Create the InfluxDB object
client = InfluxDBClient(host, port, user, password, dbname)

# Run until keyboard out
try:
    while True:
        # This generates a random value for testing purposes.
        vsense = random.randint(0,100)
        
        #print vsense
        iso = time.ctime()	# Time stamp here.

        json_body = [
        {
          "measurement": session,
              "tags": {
                  "run": runNo,
                  },
              "time": iso,
              "fields": {
                  "vsense1" : vsense + 1,
				  "vsense2" : vsense + 2,
				  "vsense3" : vsense + 3,
				  "vsense4" : vsense + 4,
				  "vsense5" : vsense + 5,
				  "vsense6" : vsense + 6,
				  "vsense7" : vsense + 7,
				  "vsense8" : vsense + 8,
				  "vsense9" : vsense + 9,
				  "vsense10" : vsense + 10,
				  "vsense11" : vsense + 11,
				  "vsense12" : vsense + 12,
				  "vsense13" : vsense + 13,
				  "vsense14" : vsense + 14,
				  "vsense15" : vsense + 15,
				  "vsense16" : vsense + 16,
				  "vsense17" : vsense + 17,
				  "vsense18" : vsense + 18,
				  "vsense19" : vsense + 19,
				  "vsense20" : vsense + 20
              }
          }
        ]

        # Write JSON to InfluxDB
        client.write_points(json_body)
        # Wait for next sample
        time.sleep(interval)

except KeyboardInterrupt:
    pass