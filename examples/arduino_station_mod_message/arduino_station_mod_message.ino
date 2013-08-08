#include <SPI.h>
#include "message.h"
#include <Station.h>
#include <GenericStation.h>
#include <config.h>
#include "printf.h"

Station<Message> * p;

Message empty = Message(PMessage::TUSER,PMessage::CUSER,0,0,0,0,0,0);

Message d[5] = {empty, empty, empty, empty, empty};
bool details = false;
long control = millis();

void setup() {
  SERIALBEGIN(57600);
  printf_begin();
  p = new Station<Message>();
  p->begin();
}

void loop() {
  if(p->update(d) > 0) {
    PRINTln("RECEBI");
    d[0].print();
    d[0].extra = d[0].extra * 2;
    uint8_t tmp = d[0].id_dest; 
    d[0].id_dest = d[0].id_from;
    d[0].id_from = tmp;
    p->write(d[0]);
  }
}