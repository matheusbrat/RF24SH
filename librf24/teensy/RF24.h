/*
 * RF24.h
 *
 *  Created on: Jan 24, 2013
 *      Author: x-warrior
 */

#ifndef RF24_H_
#define RF24_H_

//typedef char uint8_t;
//typedef long long uint64_t;

#include <Arduino.h>
#include <SPI.h>
#include "nRF24L01.h"


class RF24 {
private:
	uint8_t ce_pin; /**< "Chip Enable" pin, activates the RX or TX role */
	uint8_t csn_pin; /**< SPI Chip select */
	bool wide_band; /* 2Mbs data rate in use? */
	bool p_variant; /* False for RF24L01 and true for RF24L01P */
	uint8_t payload_size; /**< Fixed size of payloads */
	bool ack_payload_available; /**< Whether there is an ack payload waiting */
	bool dynamic_payloads_enabled; /**< Whether dynamic payloads are enabled. */
	uint8_t ack_payload_length; /**< Dynamic size of pending ack payload. */
	uint64_t pipe0_reading_address; /**< Last address set on pipe 0 for reading. */
protected:
	void csn(int mode);
	void ce(int level);
	uint8_t read_register(uint8_t reg, uint8_t* buf, uint8_t len);
	uint8_t read_register(uint8_t reg);
	uint8_t write_register(uint8_t reg, const uint8_t* buf, uint8_t len);
	uint8_t write_register(uint8_t reg, uint8_t value);
	uint8_t write_payload(const void* buf, uint8_t len);
	uint8_t read_payload(void* buf, uint8_t len);
	uint8_t flush_rx(void);
	uint8_t flush_tx(void);
	uint8_t get_status(void);
	void print_status(uint8_t status);
	void print_observe_tx(uint8_t value);
	void print_byte_register(const char* name, uint8_t reg, uint8_t qty = 1);
	void print_address_register(const char* name, uint8_t reg, uint8_t qty = 1);
	void toggle_features(void);

public:
	enum rf24_pa_dbm_e {
		RF24_PA_MIN = 0, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX, RF24_PA_ERROR
	};
	enum rf24_datarate_e {
		RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS
	};
	enum rf24_crclength_e {
		RF24_CRC_DISABLED = 0, RF24_CRC_8, RF24_CRC_16
	};

	RF24(uint8_t _cepin, uint8_t _cspin);
	void begin(void);
	void startListening(void);
	void stopListening(void);
	bool write(const void* buf, uint8_t len);
	bool available(void);
	bool read(void* buf, uint8_t len);
	void openWritingPipe(uint64_t address);
	void openReadingPipe(uint8_t number, uint64_t address);
	void setRetries(uint8_t delay, uint8_t count);
	void setChannel(uint8_t channel);
	void setPayloadSize(uint8_t size);
	uint8_t getPayloadSize(void);
	uint8_t getDynamicPayloadSize(void);
	void enableAckPayload(void);
	void enableDynamicPayloads(void);
	bool isPVariant(void);
	void setAutoAck(bool enable);
	void setAutoAck(uint8_t pipe, bool enable);
	void setPALevel(rf24_pa_dbm_e level);
	rf24_pa_dbm_e getPALevel(void);
	bool setDataRate(rf24_datarate_e speed);
	rf24_datarate_e getDataRate(void);
	void setCRCLength(rf24_crclength_e length);
	rf24_crclength_e getCRCLength(void);
	void disableCRC(void);
	void printDetails(void);
	void powerDown(void);
	void powerUp(void);
	bool available(uint8_t* pipe_num);
	void startWrite(const void* buf, uint8_t len);
	void writeAckPayload(uint8_t pipe, const void* buf, uint8_t len);
	bool isAckPayloadAvailable(void);
	void whatHappened(bool& tx_ok, bool& tx_fail, bool& rx_ready);
	bool testCarrier(void);
	bool testRPD(void);

	virtual ~RF24();
};

#endif /* RF24_H_ */
