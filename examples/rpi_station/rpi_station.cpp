#include "../../PMessage.h"
#include "../../Station.h"
#include "../../GenericStation.h"

#include <iostream>

using namespace std;

Station * p;

PMessage d[5];
bool details = false;
unsigned long control = millis();

void loop();

int main(int argc, char **argv) {
	__start_timer();
	p = new Station(sizeof(PMessage));
	p->begin();
	loop();
}

void loop() {
	while(1) {
	    if(p->update(d) > 0) {
	      d[0].print();
	    }
	}
}

