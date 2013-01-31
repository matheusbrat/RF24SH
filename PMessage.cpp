/*
 * PMessage.h
 *
 *  Created on: Jan 28, 2013
 *      Author: x-warrior
 */

#include "PMessage.h"

PMessage::PMessage(PMessage::TYPE _pmsg, PMessage::CMD _t, uint8_t _id_dest, uint8_t _id_from, uint8_t _value, uint8_t _value2,
		uint8_t _value3) {
	proto = 0;
	proto = SET_MSG_PROT(proto, _pmsg);
	proto = SET_MSG_TYPE(proto, _t);
	id_dest = _id_dest;
	id_from = _id_from;
	value = _value;
	value2 = _value2;
	value3 = _value3;
}

PMessage::PMessage(uint8_t b[6]) {
	proto = 0;
	proto = b[0];
	id_dest = b[1];
	id_from = b[2];
	value = b[3];
	value2 = b[4];
	value3 = b[5];
}

bool PMessage::is_protocol() {
	if (PMessage::PROTOCOL == GET_MSG_PROT(proto))
		return true;
	return false;
}

int PMessage::get_type() {
	return GET_MSG_TYPE(proto);
}

void PMessage::print() {
	Serial.print("TYPE ");
	Serial.println(get_type(), HEX);
	Serial.print("CMD ");
	Serial.println(is_protocol(), HEX);
	Serial.print("DEST ");
	Serial.println(id_dest, HEX);	
	Serial.print("FROM ");
	Serial.println(id_from, HEX);	
	Serial.print("VALUE ");
	Serial.println(value, HEX);	
	Serial.print("VALUE2 ");
	Serial.println(value2, HEX);	
	Serial.print("VALUE3 ");
	Serial.println(value3, HEX);	
}


PMessage::~PMessage() {
	// TODO Auto-generated destructor stub
}


