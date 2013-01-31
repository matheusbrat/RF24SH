/*
 * GenericStation.h
 *
 *  Created on: Jan 30, 2013
 *      Author: x-warrior
 */

#ifndef GENERICSTATION_H_
#define GENERICSTATION_H_

#include "RF24.h"
#include "PMessage.h"

#define PROTO_PIPE 0x7878787878LL
#define PROTO_PIPE_PREFIX 0xABABABAB

class GenericStation {
protected:
	RF24 radio; // CS, CE PINS
	uint8_t childPipes[4];
	uint8_t parentPipe;
	uint8_t * childNodes[4];
	uint8_t childNodesSize[4];
	uint8_t id;
	uint8_t level;

	void print();
	bool indirectChild(uint8_t id);
	void registerPipe(uint8_t pipeNumber, uint8_t id);
	void registerIndirecChild(uint8_t parent, uint8_t id);
	uint8_t findOpenPipe();
	uint8_t findChildPipe(uint8_t id);
	void processRead(PMessage p);

	virtual void receivedWhoListen(PMessage p);
	virtual void receivedIListen(PMessage p);
	virtual void receivedAskConfig(PMessage p);
	virtual void receivedSetConfig(PMessage p);

	bool writePipe(uint64_t pipe, PMessage p);
	bool write(PMessage p);

public:
	void read();
	bool writePipe(uint8_t pipeNumber, PMessage p);
	GenericStation();

	virtual ~GenericStation();
};

#endif /* GENERICSTATION_H_ */
