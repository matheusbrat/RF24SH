#include "../../PMessage.h"
#include "../../GenericStation.h"
#include "../../MasterStation.h"

#include <iostream>

using namespace std;

MasterStation * p;

PMessage d[5];
bool details = false;
unsigned long control = millis();

void loop();

int main(int argc, char **argv) {
	__start_timer();
	p = new MasterStation(sizeof(PMessage));
	loop();
}

void loop() {
	while(1) {
                if((millis() - control) > 5000) {
                        control = millis();
                        p->testMessage();
                }
                p->update(d);
	}
}

