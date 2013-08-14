#include <SPI.h>
#include "message.h"
#include <GenericStation.h>
#include <MasterStation.h>
#include <config.h>
#include "printf.h"

MasterStation<Message> * p;

Message empty = Message(PMessage::TUSER,PMessage::CUSER,0,0,0,0,0);

Message d[5] = {empty, empty, empty, empty, empty};

bool details = false;
long control = millis();

void setup() {
  SERIALBEGIN(57600);
  printf_begin();
  p = new MasterStation<Message>();
}

void testMessage() { 
    Message t = Message(PMessage::TUSER, PMessage::CUSER, (p->nextId - 1), 0x01, 233, 255, 100, 25.23);
    p->write(t);
}
void loop() {
  if(millis() - control > 1000) {
    testMessage();
    control = millis();
  }
  p->update(d);
  if(p->update(d) > 0) {
    PRINTln("RECEBI");
    Message m = p->pickNewMessage(d);
    m.print();
  }
}