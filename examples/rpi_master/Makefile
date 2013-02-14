all: 
	cd ../../librf24/rpi; make all
	g++ -L../../librf24/rpi/ -lrf24 ../../config.h ../../GenericStation.h ../../MasterStation.h ../../PMessage.h ../../GenericStation.cpp ../../MasterStation.cpp ../../PMessage.cpp rpi_master.cpp -o rpi_master

