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

template <class MESSAGE_TYPE>
class Station: public GenericStation<MESSAGE_TYPE> {
private:
	void startup();
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
	bool sendAskConfig(bool resend);

	void receivedWhoListen(MESSAGE_TYPE p);
	void receivedIListen(MESSAGE_TYPE p);
	void receivedAskConfig(MESSAGE_TYPE p);
	void receivedSetConfig(MESSAGE_TYPE p);

public:
	Station();
	Station(uint8_t ce, uint8_t csn);
	void begin();
	bool write(MESSAGE_TYPE msg);
	int update(MESSAGE_TYPE p[5]);
	virtual ~Station();
};

#include "Station-impl.h"

#endif /* STATION_H_ */
