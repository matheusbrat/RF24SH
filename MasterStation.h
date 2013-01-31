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
public:
	MasterStation();
	virtual ~MasterStation();

	void receivedWhoListen(PMessage p);
	void receivedIListen(PMessage p);
	void receivedAskConfig(PMessage p);
	void receivedSetConfig(PMessage p);

	uint8_t getParent(PMessage p);

};

#endif /* MASTERSTATION_H_ */
