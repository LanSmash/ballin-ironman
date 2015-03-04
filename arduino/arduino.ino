
#include<DMXSerial.h>
//http://www.mathertel.de/Arduino/DMXSerial.aspx
//So you have to avoid using the Serial object at all and even don't use it for debugging purpose!


// *** DMXSerial ***

// - - - - -
// DmxSerial - A hardware supported interface to DMX.
// DmxSerialRecv.ino: Sample DMX application for retrieving 3 DMX values:
// address 1 (red) -> PWM Port 9
// address 2 (green) -> PWM Port 6
// address 3 (blue) -> PWM Port 5
// 
// Copyright (c) 2011 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// 
// Documentation and samples are available at http://www.mathertel.de/Arduino
// - - - - -

// Constants for demo program

const int RedPin =    9;  // PWM output pin for Red Light.
const int GreenPin =  6;  // PWM output pin for Green Light.
const int BluePin =   5;  // PWM output pin for Blue Light.

// *** FastLED ***

// ArrayOfLedArrays - see https://github.com/FastLED/FastLED/wiki/Multiple-Controller-Examples for more info on
// using multiple controllers.  In this example, we're going to set up four NEOPIXEL strips on three
// different pins, each strip getting its own CRGB array to be played with, only this time they're going
// to be all parts of an array of arrays.

#include "FastLED.h"
//#define DmxSerial_h //FastLED includes DMX support but only as a LED style write output.

#define NUM_STRIPS 1 //save some memory, we can't address RGBx3x144 LEDs via DMX anyway
#define NUM_LEDS_PER_STRIP 144
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];

void setup () {
  
  // *** DMXSerial ***
  DMXSerial.init(DMXReceiver);
  
  // set some default values
  /*
  DMXSerial.write(1, 80);
  DMXSerial.write(2, 0);
  DMXSerial.write(3, 0);
  */
  
  // enable pwm outputs
  pinMode(RedPin,   OUTPUT); // sets the digital pin as output
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin,  OUTPUT);


  // *** FastLED ***

  // For mirroring strips, all the "special" stuff happens just in setup.  We
  // just addLeds multiple times, once for each strip  
  // tell FastLED there's NUM_LEDS_PER_STRIP (144) NEOPIXEL leds on pin 10, 11 &12
  FastLED.addLeds<NEOPIXEL, 10>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 11>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 12>(leds[0], NUM_LEDS_PER_STRIP);
  // Note, only 170RGB channels available in a single DMX universe (512).
  
  //FastLED.addLeds<NEOPIXEL, 11>(leds[1], NUM_LEDS_PER_STRIP);
  //FastLED.addLeds<NEOPIXEL, 12>(leds[2], NUM_LEDS_PER_STRIP);
}


void loop() {

  // This is a failed attempt to copy the DMX array into a local array
  // Ultimately this would be a way to simply have FastLED push the RGB values directly out without any copying.
  // Array of DMX values (raw).
  // Entry 0 will never be used for DMX data but will store the startbyte (0 for DMX mode).
  //uint8_t  mydmxData[DMXSERIAL_MAX+1];
  //memcpy(mydmxData, DMXSerial._dmxData, DMXSERIAL_MAX+1);


  // *** DMXSerial ***

  // Calculate how long no data packet was received
  unsigned long lastPacket = DMXSerial.noDataSince();
  
  if (lastPacket < 5000) {
    // read recent DMX values and set pwm levels 
    analogWrite(RedPin,   DMXSerial.read(1));
    analogWrite(GreenPin, DMXSerial.read(2));
    analogWrite(BluePin,  DMXSerial.read(3));

  } else {
    // Show pure red color, when no data was received since 5 seconds or more.
    analogWrite(RedPin,   100);
    analogWrite(GreenPin, 200);
    analogWrite(BluePin,  255);
  } // if
  
  #define DMX_START_ADDRESS 1
  int DMXbyte = DMX_START_ADDRESS;
  
  // we can only  
  // This outer loop will go over each strip, one at a time
  //for(int x = 0; x < NUM_STRIPS; x++) {
  int x = 0;
    // This inner loop will go over each led in the current strip, one at a time
    for(int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
      //leds[x][i] = CRGB::Red;
      leds[x][i].red =    DMXSerial.read(DMXbyte);
      DMXbyte++;
      leds[x][i].green =  DMXSerial.read(DMXbyte);
      DMXbyte++;
      leds[x][i].blue =   DMXSerial.read(DMXbyte);
      DMXbyte++;

      
    }
  //}
  //512 channels leave 170 RGB values available. This is only enough for 1x144 strip.
  
  // Copy led colors from strip 0 to strip 1
  // in reality we can simply do this via setup.
  //memmove8( &leds[1], &leds[0], NUM_LEDS_PER_STRIP * sizeof( CRGB) );
  
  //update leds?
  FastLED.show();
}

// End DMX.



