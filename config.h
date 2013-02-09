/*
 * config.h
 *
 *  Created on: Feb 9, 2013
 *      Author: Matheus (X-warrior) Bratfisch (matheusbrat@gmail.com)
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#if defined(ARDUINO) || defined(__KV20DX128__)

#define SERIALBEGIN(V) Serial.begin(V);
#define PRINT(V, ...) Serial.print(V, ##__VA_ARGS__)
#define PRINTln(V, ...) Serial.println(V, ##__VA_ARGS__)

#else

#define millis __millis
#define delay(milisec) __msleep(milisec)
#define delayMicroseconds(usec) __usleep(usec)
#define SERIALBEGIN(V)
#define PRINT(V, ...)
#define PRINTln(V, ...)

#endif

#endif /* CONFIG_H_ */
