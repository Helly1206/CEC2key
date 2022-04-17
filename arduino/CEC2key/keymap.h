/* 
 * CEC2key - keymap
 * Key mapping and keyboard control
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 6-2-2022
 * Copyright: Ivo Helwegen
 */

 /* Memory map
  *  0-1: Physical address ??? spa>1234
  *  2-14: OSD name sn>name
  *  15: array length NA
  *  32-n*2: array (first byte: CEC, second byte: key) sk>xx:yy
  *  gpa, sn, gk
  */

#ifndef keymap_h
#define keymap_h

#include "Keyboard.h"

#define DEBUG_KEYMAP

#define CEC_PHYSICAL_ADDRESS    0x1000

#define A_PHYS          0 // word 16 bits
#define A_OSDNAME       2 // string
#define L_OSDNAME       13
#define A_POWERON       30
#define A_POWERSTDBY    31
#define A_KEYCODES      32 // 2 byte
#define L_KEYCODES      128

typedef struct {
  byte cec;
  byte key;
} keyCode;

class Ckeymap {
public:
  Ckeymap(); // constructor
  void init();
  void handle();
  byte map(byte cec, bool verbose);
  bool release(bool verbose);
  uint16_t getPhysical();
  bool setPhysical(uint16_t physical);
  bool getOSDName(char *OSDname);
  bool setOSDName(char *OSDname);
  byte getPowerOn();
  bool setPowerOn(byte state);
  byte getPowerStandby();
  bool setPowerStandby(byte state);  
  bool writeKeyCode(keyCode kc);
  bool readNextKeyCode(keyCode &kc);
  bool resetCounter();
  bool clearKeyCodes();
  bool toggleKeyControl();
private:
  int getAddress();
  bool incCounter();
  byte getKey(byte cec);
  byte keyCounter;
  bool keyControl;
};

extern Ckeymap keymap;

#endif
