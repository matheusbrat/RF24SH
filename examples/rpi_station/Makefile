all: 
	cd ../../librf24/rpi; make all
	g++ -L../../librf24/rpi/ -lrf24 ../../config.h ../../GenericStation.h ../../Station.h ../../PMessage.h ../../GenericStation.cpp ../../Station.cpp ../../PMessage.cpp rpi_station.cpp -o rpi_station

