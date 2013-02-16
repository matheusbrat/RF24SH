#if defined(__MK20DX128__)
#include "../librf24/teensy/RF24.h"
#warning You must include SPI Library.
#elif !defined(__MK20DX128__) && defined(ARDUINO)
#include "../librf24/arduino/RF24.h"
#warning You must include SPI Library.
#else
#endif
