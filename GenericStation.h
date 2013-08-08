/*
 * GenericStation.h
 *
 *  Created on: Jan 30, 2013
 *      Author: Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
 */

#ifndef GENERICSTATION_H_
#define GENERICSTATION_H_


//#include "SPI.h"
#if defined(__MK20DX128__) || defined(ARDUINO)
#include "utility/RF24.h"
#else
#include "librf24/rpi/RF24.h"
#endif
/*#if defined(__MK20DX128__)
#include "librf24/teensy/RF24.h"
#elif !defined(__MK20DX128__) && defined(ARDUINO)
#include "librf24/arduino/RF24.h"
#else
#include "librf24/rpi/RF24.h"
#endif*/

#include "PMessage.h"
#include "config.h"

#define PROTO_PIPE 0x7878787878LL
#define PROTO_PIPE_PREFIX 0x00ABABABABLL
#define ID_TO_PIPE(P) (uint64_t) ((PROTO_PIPE_PREFIX << 8) | P)

template <class MESSAGE_TYPE>
class GenericStation {
private:
	void startup();
protected:
	uint8_t childPipes[4];
	uint8_t * childNodes[4];
	uint8_t childNodesSize[4];
	uint8_t level;

	void print();
	bool indirectChild(uint8_t id);
	void registerPipe(uint8_t pipeNumber, uint8_t id);
	void registerIndirectChild(uint8_t parent, uint8_t id);
	uint8_t findOpenPipe();
	void processReadProtocol(MESSAGE_TYPE p);

	virtual void receivedWhoListen(MESSAGE_TYPE p);
	virtual void receivedIListen(MESSAGE_TYPE p);
	virtual void receivedAskConfig(MESSAGE_TYPE p);
	virtual void receivedSetConfig(MESSAGE_TYPE p);

	bool writePipe(uint64_t pipe, MESSAGE_TYPE p);
	bool writeProtocol(MESSAGE_TYPE p);
	MESSAGE_TYPE processRead(MESSAGE_TYPE p);

public:
	uint8_t findChildPipe(uint8_t id);
	bool read(uint8_t * pipeNumber, MESSAGE_TYPE &p);
    RF24 radio; // CS, CE PINS
	void readProtocol();
	bool writePipe(uint8_t pipeNumber, MESSAGE_TYPE p);
	GenericStation();
	GenericStation(uint8_t ce, uint8_t csn);
	int update(MESSAGE_TYPE p[5]);
	MESSAGE_TYPE readMaster();
	bool write(MESSAGE_TYPE p);
	MESSAGE_TYPE pickNewMessage(MESSAGE_TYPE p[5]);
	uint8_t id;
	uint8_t parentPipe;


	virtual ~GenericStation();
};

#include "GenericStation-impl.h"

#endif /* GENERICSTATION_H_ */
