/* 
 * CEC2key - CEC2key
 * HDMI CEC to keyboard control
 * Hardware: Sparkfun Pro Micro
 * Version 0.81
 * 6-2-2022
 * Copyright: Ivo Helwegen
 */

/*
  HDMICEC - initialize and receive CEC (from example)
  keymap - function as callback (map keys from rom, read/ write rom) - also keyboard.h
  control - Serial control (modes: monitor, simulate, run)
*/

#include "hdmicec.h"
#include "control.h"
#include "keymap.h"

void setup() {
  control.init();
  hdmicec.init();
  keymap.init();
}

void loop() {
  control.handle();
  hdmicec.handle();
  keymap.handle();  
}
