/*
 * GenericStation.cpp
 *
 *  Created on: Jan 30, 2013
 *      Author: x-warrior
 */

#include "GenericStation.h"

GenericStation::GenericStation() :
		radio(9, 10) {
	// FIX NUMBER FOUR MAGIC NUMBER
	for (int i = 0; i < 4; i++) {
		childPipes[i] = 0;
		childNodes[i] = 0;
		childNodesSize[i] = 0;
	}
	parentPipe = 0;
	id = 0;
	level = 0;

	radio.begin();
	radio.setRetries(15, 15);
	radio.setPayloadSize(sizeof(PMessage));
	radio.openReadingPipe(0, PROTO_PIPE);
	radio.startListening();
	radio.printDetails();
}

uint8_t GenericStation::findOpenPipe() {
	//TODO: CHECK!
	for (uint8_t i = 0; i < 4; i++) {
		if (childPipes[i] == 0x00) {
			return i;
		}
	}
	return 0xFF;
}

void GenericStation::registerIndirecChild(uint8_t parent, uint8_t id) {
	uint8_t pipe = findChildPipe(parent);
	uint8_t * list = (uint8_t*) realloc(childNodes[pipe],
			(sizeof(uint8_t) * (childNodesSize[pipe] + 1)));
	list[childNodesSize[pipe]] = id;
	childNodes[pipe] = list;
	childNodesSize[pipe] = childNodesSize[pipe] + 1;
}

bool GenericStation::write(PMessage p) {
	delay(10);
	radio.stopListening();
	radio.openWritingPipe(PROTO_PIPE);
	short attempts = 5;
	boolean ok = false;
	do {
		ok = radio.write(&p, sizeof(PMessage));
	} while (!ok && --attempts);
	Serial.println("SENDING");
	p.print();
	radio.startListening();
	return ok;
}

void GenericStation::print() {
	Serial.print("ID ");
	Serial.println(id);
	Serial.print("PARENT ");
	Serial.println(parentPipe);
	for (int i = 0; i < 4; i++) {
		Serial.print("(Indirect?) Child of Pipe: ");
		Serial.println(i);
		for (int x = 0; x < childNodesSize[i]; x++) {
			Serial.print(childNodes[i][x], HEX);
		}
		Serial.println("");
	}
}

void GenericStation::read() {
	boolean done = false;
	uint8_t buffer[sizeof(PMessage)];
	PMessage c = 0;
	if (radio.available()) {
		while (!done) {
			done = radio.read(&buffer, sizeof(PMessage));
			c = (PMessage) buffer;
		}
		Serial.println("RECEIVING");
		c.print();
		processRead(c);
	}
}
void GenericStation::processRead(PMessage p) {
	if (p.is_protocol()) {
		switch (p.get_type()) {
		case PMessage::WHO_LISTEN:
			receivedWhoListen(p);
			break;
		case PMessage::I_LISTEN:
			receivedIListen(p);
			break;
		case PMessage::ASK_CONFIG:
			receivedAskConfig(p);
			break;
		case PMessage::SET_CONFIG:
			receivedSetConfig(p);
			break;
		}
	}
}

bool GenericStation::indirectChild(uint8_t id) {
	for (int i = 0; i < 4; i++) {
		for (int x = 0; x < childNodesSize[i]; x++) {
			uint8_t v = childNodes[i][x];
			if (id == v) {
				return true;
			}
		}
	}
	return false;
}

void GenericStation::registerPipe(uint8_t pipeNumber, uint8_t id) {
	childPipes[pipeNumber] = id;
	if (childNodesSize[pipeNumber] == 0) {
		uint8_t * list = (uint8_t*) malloc(
				(sizeof(uint8_t) * (childNodesSize[pipeNumber] + 1)));
		list[childNodesSize[pipeNumber]] = id;
		childNodes[pipeNumber] = list;
		childNodesSize[pipeNumber] = childNodesSize[pipeNumber] + 1;
	} else {
		uint8_t * list = (uint8_t*) realloc(childNodes[pipeNumber],
				(sizeof(uint8_t) * (childNodesSize[pipeNumber] + 1)));
		list[childNodesSize[pipeNumber]] = id;
		childNodes[pipeNumber] = list;
		childNodesSize[pipeNumber] = childNodesSize[pipeNumber] + 1;
	}
}

uint8_t GenericStation::findChildPipe(uint8_t id) {
	for (int i = 0; i < 4; i++) {
		for (int x = 0; x < childNodesSize[i]; x++) {
			uint8_t v = childNodes[i][x];
			if (id == v) {
				return i;
			}
		}
	}
	return 0xFF;
}

void GenericStation::receivedWhoListen(PMessage p) {
	// Class that extend this must implement!
}

void GenericStation::receivedIListen(PMessage p) {
	// Class that extend this must implement!
}

void GenericStation::receivedAskConfig(PMessage p) {
	// Class that extend this must implement!
}

void GenericStation::receivedSetConfig(PMessage p) {
	// Class that extend this must implement!
}

GenericStation::~GenericStation() {
}
