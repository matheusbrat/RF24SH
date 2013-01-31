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


public:
	void read();
	bool write(PMessage p);
	GenericStation();
	virtual ~GenericStation();
};

#endif /* GENERICSTATION_H_ */
