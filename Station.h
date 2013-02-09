/*
 * Station.h
 *
 *  Created on: Jan 30, 2013
 *      Author: Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
 */

#ifndef STATION_H_
#define STATION_H_

#include "GenericStation.h"
#include "config.h"

class Station: public GenericStation {
protected:
	enum FLAGS {
		NOTHING, RETRANSMIT_FIRST, RETRANSMIT, WAITING
	};

	uint8_t flag;
	uint8_t forward;
	uint8_t idSelection[5];
	uint8_t levelSelection[5];
	uint8_t control;

	void sendWhoListen();
	bool sendAskConfig();

	void receivedWhoListen(PMessage p);
	void receivedIListen(PMessage p);
	void receivedAskConfig(PMessage p);
	void receivedSetConfig(PMessage p);

public:
	Station();
	virtual ~Station();
};

#endif /* STATION_H_ */
