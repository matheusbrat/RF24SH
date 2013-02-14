#include "../../PMessage.h"
#include "../../Station.h"
#include "../../GenericStation.h"
#include "../../MasterStation.h"

#include <iostream>

#define MASTER

using namespace std;

GenericStation * p;

PMessage d[5];
bool details = false;
unsigned long control = millis();

void loop();

int main(int argc, char **argv) {
	__start_timer();
	PRINT(255, HEX);
//	cout << d[0].is_protocol() << endl;
//	p->radio.printDetails();
	cout << "TEste" << endl;
	#ifdef MASTER
	p = new MasterStation();
	#else 
	p = new Station();
	#endif
	p->radio.printDetails();
	loop();
}

void loop() {
	while(1) {
                if((millis() - control) > 5000) {
                        control = millis();
                        ((MasterStation*) p)->testMessage();
                }
                p->update(d);
	}
}

