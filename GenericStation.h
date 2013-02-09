/*
 * GenericStation.h
 *
 *  Created on: Jan 30, 2013
 *      Author: Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
 */

#ifndef GENERICSTATION_H_
#define GENERICSTATION_H_

#if defined(ARDUINO) || defined(__KV20DX128__)
#include "RF24.h"
#else
#include <RF24/RF24.h>
#endif

#include "PMessage.h"
#include "config.h"

#define PROTO_PIPE 0x7878787878LL
#define PROTO_PIPE_PREFIX 0x00ABABABABLL
#define ID_TO_PIPE(P) (uint64_t) ((PROTO_PIPE_PREFIX << 8) | P)

class GenericStation {
protected:
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
	void processReadProtocol(PMessage p);

	virtual void receivedWhoListen(PMessage p);
	virtual void receivedIListen(PMessage p);
	virtual void receivedAskConfig(PMessage p);
	virtual void receivedSetConfig(PMessage p);

	bool writePipe(uint64_t pipe, PMessage p);
	bool writeProtocol(PMessage p);
	bool read(uint8_t * pipeNumber, PMessage &p);
	PMessage processRead(PMessage p);

public:
    RF24 radio; // CS, CE PINS
	void readProtocol();
	bool writePipe(uint8_t pipeNumber, PMessage p);
	GenericStation();
	int update(PMessage p[5]);
	PMessage readMaster();


	virtual ~GenericStation();
};

#endif /* GENERICSTATION_H_ */
