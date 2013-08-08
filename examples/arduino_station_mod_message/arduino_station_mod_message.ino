#include <SPI.h>
#include "message.h"
#include <Station.h>
#include <GenericStation.h>
#include <config.h>
#include "printf.h"

Station<Message> * p;

Message empty = Message(PMessage::TUSER,PMessage::CUSER,0,0,0,0,0);

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
    Message m = p->pickNewMessage(d);
    m.print();
    m.extra = m.extra * 2;
    uint8_t tmp = m.id_dest; 
    m.id_dest = m.id_from;
    m.id_from = tmp;
    p->write(m);
  }
}