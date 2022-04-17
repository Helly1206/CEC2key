/* 
 * CEC2key - keymap
 * Key mapping and keyboard control
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 6-2-2022
 * Copyright: Ivo Helwegen
 */

#include "keymap.h"
#include <EEPROM.h>

Ckeymap::Ckeymap() { // constructor
  keyControl = true;
}

void Ckeymap::init() {
  Keyboard.begin();
  resetCounter();
}

void Ckeymap::handle() { 
  // do something if simulate ....
}

byte Ckeymap::map(byte cec, bool verbose) {
  byte key = getKey(cec);
  if (verbose) {
    char hexval[3];
    memset(hexval, 0, 3);
    Serial.print("#");
    control.Byte2HEX(cec, hexval);
    Serial.print(hexval);
    Serial.print(":");
    if (!keyControl) {
      Serial.print("(");
    }
    control.Byte2HEX(key, hexval);
    Serial.print(hexval);
    if (!keyControl) {
      Serial.println(")");
    } else {
      Serial.println("");
    }
  }
  if ((key != 0) && (keyControl)) {
    Keyboard.press(key);
  }
  return key;
}

bool Ckeymap::release(bool verbose) {
  if (keyControl) {
    Keyboard.releaseAll();
  }
  if (verbose) {
    Serial.print("#");
    if (!keyControl) {
      Serial.print("(");
    } 
    Serial.print("Release");
    if (!keyControl) {
      Serial.println(")");
    } else {
      Serial.println("");
    }
  }
  return true;
}

uint16_t Ckeymap::getPhysical() {
  uint16_t physical = 0;
  EEPROM.get(A_PHYS, physical);
  return (physical == 0) ? CEC_PHYSICAL_ADDRESS : physical; // never use address 0
}

bool Ckeymap::setPhysical(uint16_t physical) {
  EEPROM.put(A_PHYS, physical);
  return true;
}

bool Ckeymap::getOSDName(char *OSDname) {
  bool finished = false;
  int i = 0;
  while (!finished) {
    OSDname[i] = EEPROM.read(A_OSDNAME + i);
    if ((OSDname[i] == 0x00) || (OSDname[i] == 0xFF)) {
      finished = true;
    } else if (i < L_OSDNAME) {
      i++; 
    } else {
      OSDname[i] = '\0';
      finished = true;
    }
  }
  if (i == 0) {
    sprintf(OSDname, "HTPC");
  }
  return true;
}

bool Ckeymap::setOSDName(char *OSDname) {
  bool finished = false;
  int i = 0;
  while (!finished) {
    if (i >= L_OSDNAME) {
      finished = true;
    } else if (i >= strlen(OSDname)) {
      EEPROM.write(A_OSDNAME + i, '\0');
      finished = true;
    } else {
      EEPROM.write(A_OSDNAME + i, OSDname[i]);
      i++;
    }
  }
  return true;
}

byte Ckeymap::getPowerOn() {
  byte state = 0;
  EEPROM.get(A_POWERON, state);
  return ((state < 0) || (state > 2)) ? 0 : state;
}

bool Ckeymap::setPowerOn(byte state) {
  if ((state < 0) || (state > 2)) {
    return false;
  }
  EEPROM.put(A_POWERON, state);
  return true;
}

byte Ckeymap::getPowerStandby() {
  byte state = 0;
  EEPROM.get(A_POWERSTDBY, state);
  return ((state < 0) || (state > 2)) ? 0 : state;
}

bool Ckeymap::setPowerStandby(byte state) {
    if ((state < 0) || (state > 2)) {
    return false;
  }
  EEPROM.put(A_POWERSTDBY, state);
  return true;
}

bool Ckeymap::writeKeyCode(keyCode kc) {
  int address;
  keyCode kcr;
  bool updated = false;
  resetCounter();
  do {
    address = getAddress();
    EEPROM.get(address, kcr);
    if ((kcr.cec == kc.cec) || (kcr.cec == 0xFF)) {
      EEPROM.put(address, kc);
      updated = true;
    }
  } while ((!updated) && incCounter());
  return updated;
}

bool Ckeymap::readNextKeyCode(keyCode &kc) {
  int address = getAddress();
  if (incCounter()) {
    EEPROM.get(address, kc);
    return (kc.cec != 0xFF);
  } else {
    return false;
  }
}

bool Ckeymap::resetCounter() {
  keyCounter = 0;
}

bool Ckeymap::clearKeyCodes() {
  keyCode kc = {0xFF, 0};
  resetCounter();
  do {
    EEPROM.put(getAddress(), kc);
  } while (incCounter());
  return true;
}

bool Ckeymap::toggleKeyControl() {
  keyControl = !keyControl;
  return keyControl;
}

// Privates

int Ckeymap::getAddress() {
  return int(keyCounter) * sizeof(keyCode) + A_KEYCODES;
}

bool Ckeymap::incCounter() {
  if (keyCounter < L_KEYCODES) {
    keyCounter++;
    return true;
  } else {
    return false;
  }
}

byte Ckeymap::getKey(byte cec) {
  byte key = 0;
  keyCode kcr;
  resetCounter();
  do {
    EEPROM.get(getAddress(), kcr);
    if (kcr.cec == cec) {
      key = kcr.key;
    }
  } while ((key == 0)  && (cec != 0xFF) && incCounter());

  return key;
}

Ckeymap keymap;
