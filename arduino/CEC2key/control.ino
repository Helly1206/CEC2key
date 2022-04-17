/* 
 * CEC2key - control
 * Serial control and debug
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 6-2-2022
 * Copyright: Ivo Helwegen
 */

#include "control.h"
#include <SoftwareReset.hpp>

Ccontrol::Ccontrol() { // constructor
  commandAvailable = false;
  memset(command, 0, CMD_LEN);
  mode = MODE_RUN;
  titleTime = 0;
}

void Ccontrol::init() {
  Serial.begin(115200);
  Serial.flush();
  titleTime = millis() + 2000;
}

void Ccontrol::handle() {  
  if (titleTime) {
    if (millis() > titleTime) {
      titleTime = 0;
      title();
    }
    return;
  }
  //return;
  if (commandAvailable) {
    handleCommand();
    commandAvailable = false;
    memset(command, 0, CMD_LEN);
  } else if (Serial.available()) {
    char c = Serial.read(); 
    if (c > 0) {
      if (c == '\n') {
        commandAvailable = true;
        command[strlen(command)] = '\0';
      } else if ((c != '\r') && (strlen(command) < CMD_LEN - 1)) {
        command[strlen(command)] = c;
      }
    }
  }
}

void Ccontrol::Byte2HEX(byte bte, char *Hex) {
  byte nSize = 2;
  byte nTemp;
  
  while (nSize--) {
    nTemp = bte/16;
    if ((bte - 16*nTemp) < 10) {
      Hex[nSize] = (char) ('0' + (bte - 16*nTemp));
    } else {
      Hex[nSize] = (char) ('A' - 10 + (bte - 16*nTemp));
    }
    bte = nTemp;
  }
  Hex[2] = '\0';
}

// Privates
void Ccontrol::title() {
  Serial.println("CEC2key");
  Serial.print("Mode: ");
  switch (mode) {
    case MODE_MONITOR:  Serial.println("Monitor"); break;
    case MODE_SIMULATE: Serial.println("Simulate"); break;
    case MODE_LISTEN:   Serial.println("Listen"); break;
    default:            Serial.println("Run"); break; 
  }
  Serial.println("Press ? for help");
}

void Ccontrol::helpSim() {
  Serial.println("<<< Simulation mode >>>");
  Serial.println("exit = exit to run mode");
  Serial.println("gk   = get key codes");
  Serial.println("xx   = hex key to simulate");
  title();
}

void Ccontrol::help() {
  Serial.println("Change mode:");
  Serial.println("mm = monitor");
  Serial.println("ms = simulate");
  Serial.println("ml = listen");
  Serial.println("mr = run");
  Serial.println("Settings:");
  Serial.println("spa>xxxx = set physical address (xxxx in hex)");
  Serial.println("gpa      = get physical address");
  Serial.println("sla      = set logical address");
  Serial.println("gla      = get logical address");
  Serial.println("sn>aaaa  = set OSD name (string aaaa)");
  Serial.println("gn       = get OSD name");
  Serial.println("spo>n    = set power on state (n 0=none, 1=power, 2=focus)");
  Serial.println("gpo      = get power on state");
  Serial.println("sps>n    = set power standby state (n 0=none, 1=power, 2=focus)");
  Serial.println("gps      = get power standby state");
  Serial.println("sk>xx:yy = set key code (xx:yy in hex)");
  Serial.println("gk       = get key codes");
  Serial.println("ck       = clear key codes");
  Serial.println("tk       = toggle key control on/ off");
  Serial.println("rst      = reset device");
  Serial.println("");
  title();
}

void Ccontrol::handleCommand() {
  Serial.print(">"); // echo
  Serial.println(command);
  if ((strncmp(command, "?", 1) == 0) && (strlen(command) == 1)) { // help
    if (mode == MODE_SIMULATE) {
      helpSim();
    } else {
      help();
    }
  } else if ((strncmp(command, "mm", 2) == 0) && (strlen(command) == 2)) { // mode monitor
    printOK();
    updateMode(MODE_MONITOR);
    title();
  } else if ((strncmp(command, "ms", 2) == 0) && (strlen(command) == 2)) { // mode simulate
    printOK();
    updateMode(MODE_SIMULATE);
    helpSim();
  } else if ((strncmp(command, "ml", 2) == 0) && (strlen(command) == 2)) { // mode listen
    printOK();
    updateMode(MODE_LISTEN);
    title();
  } else if (((strncmp(command, "mr", 2) == 0) && (strlen(command) == 2)) ||
            (mode == MODE_SIMULATE && ((strncmp(command, "exit", 4) == 0) && (strlen(command) == 4)))) { // mode run
    printOK();
    updateMode(MODE_RUN);
    title();
  } else if (command[0] == 's') { // set
    if (strncmp(&command[1], "pa>", 3) == 0) {
      char pa[5];
      memset(pa, 0, 5);
      strncpy(pa, &command[4], 4);
      if (strlen(pa) == 4) {
        uint16_t ipa = (((uint16_t)HEX2Byte(pa))<<8) + ((uint16_t)HEX2Byte(&pa[2]));
        keymap.setPhysical(ipa) ? printOK() : printError();
      } else {
        printError();
      }
    } else if (strncmp(&command[1], "la", 2) == 0) {
      hdmicec.setLogicalAddress() ? printOK() : printError();
    } else if (strncmp(&command[1], "po>", 3) == 0) {
      byte ps = INT2Byte(&command[4]);
      keymap.setPowerOn(ps) ? printOK() : printError();
    } else if (strncmp(&command[1], "ps>", 3) == 0) {
      byte ps = INT2Byte(&command[4]);
      keymap.setPowerStandby(ps) ? printOK() : printError();
    } else if (strncmp(&command[1], "n>", 2) == 0) {
      char na[L_OSDNAME+1];
      memset(na, 0, L_OSDNAME+1);
      strncpy(na, &command[3], L_OSDNAME+1);
      keymap.setOSDName(na) ? printOK() : printError();
    } else if (strncmp(&command[1], "k>", 2) == 0) {
      char k[6];
      memset(k, 0, 6);
      strncpy(k, &command[3], 5);
      if ((strlen(k) == 5) && (k[2] == ':')) {
        keyCode kc;
        kc.cec = HEX2Byte(k);
        kc.key = HEX2Byte(&k[3]);
        keymap.writeKeyCode(kc) ? printOK() : printError();
      } else {
        printError();
      }
    } else {
      printError();
    }
  } else if (command[0] == 'g') { // get
    if (strncmp(&command[1], "pa", 2) == 0) {
      uint16_t ipa = keymap.getPhysical();
      char pa[5];
      memset(pa, 0, 5);
      Byte2HEX((byte)(ipa >> 8), pa);
      Byte2HEX((byte)(ipa & 0xFF), &pa[2]);
      Serial.print("<");
      Serial.println(pa);
    } else if (strncmp(&command[1], "la", 2) == 0) {
      int la = hdmicec.getLogicalAddress();
      Serial.print("<");
      Serial.println(la);
    } else if (strncmp(&command[1], "po", 2) == 0) {
      byte ps = keymap.getPowerOn();
      Serial.print("<");
      Serial.println(ps);
    } else if (strncmp(&command[1], "ps", 2) == 0) {
      byte ps = keymap.getPowerStandby();
      Serial.print("<");
      Serial.println(ps);
    } else if (strncmp(&command[1], "n", 1) == 0) {
      char na[L_OSDNAME+1];
      Serial.print("<");
      keymap.getOSDName(na);
      Serial.println(na);
    } else if (strncmp(&command[1], "k", 1) == 0) {
      keyCode kc;
      char hexval[3];
      memset(hexval, 0, 3);
      keymap.resetCounter();
      while (keymap.readNextKeyCode(kc)) {
        Serial.print("<[");
        Byte2HEX(kc.cec, hexval);
        Serial.print(hexval);
        Serial.print(":");
        Byte2HEX(kc.key, hexval);
        Serial.print(hexval);
        Serial.println("]");
      }
    } else {
      printError();
    }
  } else if ((strncmp(command, "ck", 2) == 0) && (strlen(command) == 2)) { // clear key pairs
    keymap.clearKeyCodes() ? printOK() : printError();
  } else if ((strncmp(command, "tk", 2) == 0) && (strlen(command) == 2)) { // toggle key control
    keymap.toggleKeyControl() ? Serial.println("<ON") : Serial.println("<OFF");
  } else if ((strncmp(command, "rst", 3) == 0) && (strlen(command) == 3)) { // reset
    Serial.println("<RESETTING");
    Serial.flush();
    delay(1000);
    softwareReset::standard(); //simple();
  } else if (mode == MODE_SIMULATE) {
    byte cec = HEX2Byte(command);
    byte key = keymap.map(cec, false);
    char hexval[3];
    if (key > 0) {
      Serial.print("<[");
      Byte2HEX(cec, hexval);
      Serial.print(hexval);
      Serial.print(":");
      Byte2HEX(key, hexval);
      Serial.print(hexval);
      Serial.println("]");
      keymap.release(false);
    } else {
      printError();
    }
  } else {
    printError();
  }
}

void Ccontrol::printError() {
  Serial.println("<?ERROR");
}

void Ccontrol::printOK() {
  Serial.println("<OK");
}

byte Ccontrol::HEX2Byte(char *Hex) {
  byte retval = 0;
  byte nSize = strlen(Hex);
  if (nSize>2) {
    nSize = 2; //return (retval);
  }
  while (nSize--) {
    if ((*Hex>='A') && (*Hex<='F')) {
      retval = retval * 16 + (*Hex++ - 'A' + 10);
    } else if ((*Hex>='0') && (*Hex<='9')) {
      retval = retval * 16 + (*Hex++ - '0');
    } else {
      retval = 0;
    }
  }
  
  return (retval);
}

byte Ccontrol::INT2Byte(char* txt) {
  byte sum, digit, i;
  sum = 0;
  for (i = 0; i < strlen(txt); i++) {
    digit = txt[i] - 0x30;
    sum = (sum * 10) + digit;
  }
  return sum;
}

void Ccontrol::updateMode(byte newMode) {
  switch (newMode) {
    case MODE_MONITOR:
      hdmicec.setMode(true, false);
      break;
    case MODE_LISTEN:
      hdmicec.setMode(true, true);
      break;
    default: // MODE_SIMULATE, MODE_RUN
      hdmicec.setMode(false, false);
      break;
  }

  mode = newMode;
  return;
}

Ccontrol control;
