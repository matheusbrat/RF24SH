/*
 * MasterStation.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: x-warrior
 */

#include "MasterStation.h"

MasterStation::MasterStation() {
	GenericStation();
	for (int i = 0; i < 10; i++) {
		levels[i] = 0xFF;
		for (int x = 0; x < 5; x++) {
			treeTalkTo[i][x] = 0;
		}
	}
	levels[1] = 0;
	level = 0;
	id = 1;
	nextId = 2;
}

uint8_t MasterStation::freePipes(uint8_t id) {
	uint8_t temp = 0;
	for (int i = 0; i < 5; i++) {
		if (treeTalkTo[id][i] == 0)
			temp++;
	}
	return temp;
}
uint8_t MasterStation::getLevel(uint8_t id) {
	return levels[id];
}

MasterStation::~MasterStation() {
	// TODO Auto-generated destructor stub
}

uint8_t MasterStation::getParent(PMessage p) {
	uint8_t parent = 0x00;
	uint16_t weight = 0xFFFF;
	for (int i = 0; i < 5; i++) {
		if (treeTalkTo[nextId][i] != 0) {
			uint8_t nFreePipe = freePipes(treeTalkTo[nextId][i]);
			if (nFreePipe > 0) {
				uint16_t temp = nFreePipe + getLevel(treeTalkTo[nextId][i]);
				if (weight >= temp) {
					parent = treeTalkTo[nextId][i];
					weight = temp;
				}
			}
		}
	}
	return parent;
	// THIS IS FOR TESTING PURPOSE AS WELL!
	// TODO: ADD SUPPORT TO MAX NODES VERIFICATION
	//if (childPipes[0] != 0x00) {
	//	return childNodes[0][0];
	//}
	//return p.id_dest;
}

void MasterStation::receivedWhoListen(PMessage p) {
	unsigned long wait = millis();
	while ((millis() - wait) < (unsigned int) (id * 100)) {
		delay(5);
	}
	PMessage c = PMessage(PMessage::PROTOCOL, PMessage::I_LISTEN,
			(uint8_t) 0x00, (uint8_t) id, 0x00, (uint8_t) level,
			(uint8_t) 0x00);
	write(c);
}

void MasterStation::receivedIListen(PMessage p) {
	// MASTER DOESN'T DO ANYTHING WHEN RECEIVE THIS!
}

void MasterStation::receivedAskConfig(PMessage p) {
	if (p.id_dest == id) {
		treeTalkTo[nextId][0] = p.id_dest;
		treeTalkTo[nextId][1] = p.id_from;
		treeTalkTo[nextId][2] = p.value;
		treeTalkTo[nextId][3] = p.value2;
		treeTalkTo[nextId][4] = p.value3;
		Serial.println("sending set_config");
		uint8_t parent = getParent(p);
		PMessage r = PMessage(PMessage::PROTOCOL, PMessage::SET_CONFIG,
				(uint8_t) 0x00, (uint8_t) id, nextId,
				(uint8_t) (getLevel(parent) + 1), parent);
		levels[nextId] = getLevel(parent) + 1;
		if (parent == id) {
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

