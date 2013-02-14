#include <SPI.h>
#include <PMessage.h>
#include <Station.h>
#include <GenericStation.h>
#include <config.h>
#include "printf.h"

Station * p;

PMessage d[5] = {0, 0, 0, 0, 0};
bool details = false;
long control = millis();

void setup() {
  SERIALBEGIN(57600);
  printf_begin();
  p = new Station();
  p->begin();
}

void loop() {
  if(p->update(d) > 0) {
    PRINTln("RECEBI");
    d[0].print();   
  }
}