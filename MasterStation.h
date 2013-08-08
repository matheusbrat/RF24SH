/*
 * MasterStation.h
 *
 *  Created on: Jan 30, 2013
 *      Author: Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
 */

#ifndef MASTERSTATION_H_
#define MASTERSTATION_H_

#include "GenericStation.h"
#include "config.h"

template <class MESSAGE_TYPE>
class MasterStation: public GenericStation<MESSAGE_TYPE> {
private:
	void startup();
protected:
	uint8_t canTalkTo[10][5];
	uint8_t levels[10];
	uint8_t talkingTo[10][4];
	uint8_t lastParent;
	long lastAskConfig;


public:
	MasterStation();
	virtual ~MasterStation();
	MasterStation(uint8_t ce, uint8_t csn);



	void receivedWhoListen(MESSAGE_TYPE p);
	void receivedIListen(MESSAGE_TYPE p);
	void receivedAskConfig(MESSAGE_TYPE p);
	void receivedSetConfig(MESSAGE_TYPE p);

	uint8_t getParent(MESSAGE_TYPE p);
	uint8_t freePipes(uint8_t id);
	uint8_t getLevel(uint8_t id);
	void testMessage();
	bool write(MESSAGE_TYPE p);
	uint8_t nextId;
	int update(MESSAGE_TYPE p[5]);
};

#include "MasterStation-impl.h"

#endif /* MASTERSTATION_H_ */
