target:svcan

#PROFILE=on
#
# To generate run profile:
#   1. set PROFILE=on
#   2. have program exit normally in main loop after some time period.
#        There is a commented statement in SolVehicle.cpp main loop that can be used.
#   3. change #undef DAEMONMODE at top of svserver.cpp so that
#        the program does not run as server, otherwise will it not exit properly for gprof. 
#   4. run with ./svserver.
#   5. After program exists, there should be a gmon.out file. 
#   6. process file run:
#       gprof svserver gmon.out > "the name of results file"
#
# Reverse all steps for program to run normally as daemon.
#
mach:=  $(shell hostname)
me:= $(shell whoami)
CPP= g++
ifeq ($(PROFILE),on)
OPTS= -std=gnu++0x -lrt -pg  -D$(mach) -D$(me) -Wall
else
OPTS= -std=gnu++0x -g -pthread  -D$(mach) -D$(me) -Wall
endif
CMDOPTS=

headers= SVT_CAN.h \
         SV_stdlibs.h \
         SV_gpio.h \
         SV_error.h \
                 
sources= main.cpp \
         SVT_CAN.cpp \
         SV_gpio.cpp \
         SV_error.cpp \
     
objects=  \
         main.o \
         SVT_CAN.o \
         SV_gpio.o \
         SV_error.o 
        
SVT_CAN.o: SVT_CAN.cpp SVT_CAN.h
	$(CPP) $(OPTS) $(CMDOPTS) -c SVT_CAN.cpp	

SV_error.o: SV_error.h SV_error.cpp
	$(CPP) $(OPTS) $(CMDOPTS) -c SV_error.cpp			

main.o: main.cpp 
	$(CPP) $(OPTS) $(CMDOPTS) -c main.cpp
		
svcan: $(objects) $(headers)
	$(CPP) $(OPTS) $(CMDOPTS) -o svcan $(objects)
	
SV_gpio.o:  SV_gpio.h SV_gpio.cpp
	$(CPP) $(OPTS) $(CMDOPTS) -c SV_gpio.cpp	
		
clean:
	rm *.o svcan

