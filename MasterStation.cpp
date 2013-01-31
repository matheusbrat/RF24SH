/*
 * MasterStation.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: x-warrior
 */

#include "MasterStation.h"

MasterStation::MasterStation() {
	GenericStation();
	level = 0;
	id = 1;
	nextId = 2;
}

MasterStation::~MasterStation() {
	// TODO Auto-generated destructor stub
}

uint8_t MasterStation::getParent(PMessage p) {
	// THIS IS FOR TESTING PURPOSE AS WELL!
	// TODO: ADD SUPPORT TO MAX NODES VERIFICATION
	//if (childPipes[0] != 0x00) {
	//	return childNodes[0][0];
	//}
	return p.id_dest;
}

void MasterStation::receivedWhoListen(PMessage p) {
	long wait = millis();
	while ((millis() - wait) < (id * 100)) {
		delay(5);
	}
	PMessage c =
			PMessage(PMessage::PROTOCOL, PMessage::I_LISTEN, (uint8_t) 0x00,
					(uint8_t) id, 0x00, (uint8_t) level, (uint8_t) 0x00);
	write(c);
}

void MasterStation::receivedIListen(PMessage p) {
	// MASTER DOESN'T DO ANYTHING WHEN RECEIVE THIS!
}

void MasterStation::receivedAskConfig(PMessage p) {
	if (p.id_dest == id) {
		Serial.println("sending set_config");
		uint8_t parent = getParent(p);
		PMessage r = PMessage(PMessage::PROTOCOL, PMessage::SET_CONFIG,
				(uint8_t) 0x00, (uint8_t) id, nextId, (uint8_t) level, parent);
		if (parent == id) {
			Serial.print("NEXT ID ");
			Serial.println(nextId);
			registerPipe(findOpenPipe(), nextId);
		} else {
			registerIndirecChild(parent, nextId);
		}
		write(r);
		nextId++;
		print();
	}
}
void MasterStation::receivedSetConfig(PMessage p) {
	// MASTER DOESN'T DO ANYTHING WHEN RECEIVE THIS!
}

