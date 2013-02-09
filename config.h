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

#include <stdint.h>
#include <iostream>

using namespace std;

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

#define millis __millis
#define delay(milisec) __msleep(milisec)
#define delayMicroseconds(usec) __usleep(usec)
#define SERIALBEGIN(V)
#define PRINT(V, ...) print_func(V, ##__VA_ARGS__)
#define PRINTln(V, ...) println_func(V, ##__VA_ARGS__)



void static print_func(char * str) {
	cout << str;
}

void static print_func(char * str, int t) {
	cout << str;
}

void static println_func(char * str) {
	cout << str << endl;
}

void static println_func(char * str, int t) {
	cout << str << endl;
}


void static print_func(uint8_t str) {
	int v = (int) str;
        cout << dec << v;
}

void static print_func(uint8_t str, int t) {
	int v = (int) str;
	if (t == HEX) 
	        cout << hex << v;
	else 
		cout << dec << v;
}

void static println_func(uint8_t str) {
	int v = (int) str;
        cout << dec << v << endl;
}

void static println_func(uint8_t str, int t) {
	int v = (int) str;
	if(t == HEX)
		cout << hex << v << endl;
	else
	        cout << dec << v << endl;
}

#endif

#endif /* CONFIG_H_ */
