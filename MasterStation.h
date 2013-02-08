/*
 * MasterStation.h
 *
 *  Created on: Jan 30, 2013
 *      Author: x-warrior
 */

#ifndef MASTERSTATION_H_
#define MASTERSTATION_H_

#include "GenericStation.h"

class MasterStation: public GenericStation {
protected:
	uint8_t nextId;
	uint8_t canTalkTo[10][5];
	uint8_t levels[10];
	uint8_t talkingTo[10][4];

public:
	MasterStation();
	virtual ~MasterStation();

	void receivedWhoListen(PMessage p);
	void receivedIListen(PMessage p);
	void receivedAskConfig(PMessage p);
	void receivedSetConfig(PMessage p);

	uint8_t getParent(PMessage p);
	uint8_t freePipes(uint8_t id);
	uint8_t getLevel(uint8_t id);
	void testMessage();
};

#endif /* MASTERSTATION_H_ */
