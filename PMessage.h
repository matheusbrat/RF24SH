/*
 * PMessage.h
 *
 *  Created on: Jan 28, 2013
 *      Author: x-warrior
 */

#ifndef PMESSAGE_H_
#define PMESSAGE_H_

#include <Arduino.h>

#define GET_MSG_PROT(V) (V >> 7)
#define SET_MSG_PROT(I, V) I | (V << 7)
#define GET_MSG_TYPE(V) (V & 0x60) >> 5
#define SET_MSG_TYPE(I, V) I | (V << 5)
#define PROTO_PIPE 0x7878787878LL
#define PROTO_PIPE_PREFIX 0xABABABAB


class PMessage {
public:
	enum TYPE { USER, PROTOCOL };
	enum CMD { WHO_LISTEN, I_LISTEN, ASK_CONFIG, SET_CONFIG };
	#define BROADCAST 0xFF

	PMessage();
	~PMessage();
	uint8_t proto, id_dest, id_from, value, level, total;

	PMessage(TYPE _pmsg, CMD _t, uint8_t id_dest, uint8_t id_from, uint8_t _value, uint8_t _level, uint8_t _total);
	PMessage(uint8_t b[6]);
	bool is_protocol();
	int get_type();
	void print();
};

#endif /* PLMESSAGE_H_ */
