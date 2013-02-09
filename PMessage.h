/*
 * PMessage.h
 *
 *  Created on: Jan 28, 2013
 *      Author: Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
 */

#ifndef PMESSAGE_H_
#define PMESSAGE_H_

#include "config.h"
#if defined(ARDUINO) || defined(__KV20DX128__)
#include <Arduino.h>
#endif

#define GET_MSG_PROT(V) (V >> 7)
#define SET_MSG_PROT(I, V) I | (V << 7)
#define GET_MSG_TYPE(V) (V & 0x70) >> 4
#define SET_MSG_TYPE(I, V) I | (V << 4)
#define GET_MSG_DEST(V) (V & 0x08) >> 3
#define SET_MSG_DEST(I, V) I | (V << 3)
#define GET_MSG_EXTRA(V) (V & 0x04) >> 2
#define SET_MSG_EXTRA(I, V) I | (V << 2)

class PMessage {
public:
	enum TYPE { TUSER, PROTOCOL };
	enum CMD { WHO_LISTEN, I_LISTEN, ASK_CONFIG, SET_CONFIG, RES1, RES2, RES3, CUSER };
	#define BROADCAST 0xFF

	PMessage();
	~PMessage();
	uint8_t proto, id_dest, id_from, value, value2, value3;

	PMessage(TYPE _pmsg, CMD _t, uint8_t id_dest, uint8_t id_from, uint8_t _value, uint8_t _value2, uint8_t _value3);
	PMessage(uint8_t b[6]);
	bool is_protocol();
	int get_type();
	void print();
};

#endif /* PLMESSAGE_H_ */

