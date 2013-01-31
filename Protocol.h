/*
 * Protocol.h
 *
 *  Created on: Jan 28, 2013
 *      Author: x-warrior
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "RF24.h"
#include "PMessage.h"

class Protocol {
private:
	RF24 radio; // CS, CE PINS
public:
	enum FLAGS { NOTHING, RETRANSMIT, WAITING };
	Protocol();
	virtual ~Protocol();
	//ALL
	uint8_t childPipes[4];
	uint8_t childIds[4];
	uint8_t parentPipe;
	uint8_t parentId;
	uint8_t * childNodes[4];
	uint8_t childNodesSize[4];
	uint8_t id;
	uint8_t level;
	void read();
	bool write(PMessage p);
	void print();
	bool indirectChild(uint8_t id);
	void registerPipe(uint8_t pipeNumber, uint8_t id);
	void registerIndirecChild(uint8_t parent, uint8_t id);
	uint8_t findOpenPipe();
	uint8_t findChildPipe(uint8_t id);

	// MASTER
	uint8_t nextId;
	uint8_t getParent(PMessage p);

	// SLAVE
	uint8_t flag;
	uint8_t forward;
	uint8_t idSelection[5];
	uint8_t levelSelection[5];
	uint8_t control;

	// DOESN'T NEED
	bool is_direct_child(uint8_t child);
	uint8_t get_next_free_pipe();

	// THINK
	void who_is_listening();
	void i_am_listening();
	void received_i_am_listening(PMessage c);
	void received_set_config(PMessage c);
	void ask_config();

};

#endif /* PROTOCOL_H_ */

