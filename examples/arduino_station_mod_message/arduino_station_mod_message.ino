#include <SPI.h>
#include "message.h"
#include <Station.h>
#include <GenericStation.h>
#include <config.h>
#include "printf.h"

Station<Message> * p;

Message d[6] = {0, 0, 0, 0, 0};
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
  }
}