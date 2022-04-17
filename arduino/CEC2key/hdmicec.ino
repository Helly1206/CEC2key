/* 
 * CEC2key - hdmicec
 * HDMI CEC control
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 6-2-2022
 * Copyright: Ivo Helwegen
 */

#include "hdmicec.h"
#include "keymap.h"

Chdmicec::Chdmicec() { // constructor
  startdelay = 0;
  power = false;
  focus = false;
}

void Chdmicec::init() {
  // create a CEC client
  uint16_t physical = keymap.getPhysical();
  ceclient = new CEC(physical, CEC_INPUT_PIN, CEC_OUTPUT_PIN);

  ceclient->onReceiveCallback(onReceiveCB);
  ceclient->onTransmitCallback(onTransmitCB);
  // initialize the client with the default device type (PLAYBACK)
  ceclient->begin(); //CEC_LogicalDevice::CDT_PLAYBACK_DEVICE

  setMode(false, false);
  
  startdelay = millis() + 5000;
}

void Chdmicec::handle() { 
  if (startdelay) {
    if (millis() > startdelay) {
      startdelay = 0;
    }
    return;
  }
  // run the client
  ceclient->run();
  if (ceclient->isReady()) {
    reportPhysAddr(0x00); // is not forbidden to do this here
    Serial.println("$Ready");
  }
}

void Chdmicec::onReceive(int source, int dest, unsigned char* buffer, int count) {
  if (count == 0)
    return;
  if (_monitorMode) {
    DbgReceive(source, dest, buffer, count);
    return;
  }
  
  int logicalAddress = ceclient->getLogicalAddress();
  uint16_t physical = 0;
  switch (buffer[0]) { 
    case 0x36: if (_promiscuous) { // Standby (broadcast)
                 DbgReceive(source, dest, buffer, count);
               }
               Serial.println("$Standby");
               powerStandby(true, source); 
               focus = false;
               break;
    case 0x83: if (dest == logicalAddress) { // Give Physical Address (directly addressed)
                 if (_promiscuous) {
                   DbgReceive(source, dest, buffer, count);
                 }
                 reportPhysAddr(source); 
               }
               break;
    case 0x82: physical = keymap.getPhysical(); // report Stream state (broadcast)
               if (dest == CLA_BROADCAST && count == 3) {
                 if (_promiscuous) { // Standby (broadcast)
                   DbgReceive(source, dest, buffer, count);
                 }
                 if (!(buffer[1] == (byte)((physical >> 8) & 0xFF) && buffer[2] == (byte)(physical & 0xFF))) {
                   if (focus) {
                     focus = false;
                     Serial.println("$Unfocus");
                     powerStandby(false, source);
                   }
                 }
               }
               break;
    case 0x86: physical = keymap.getPhysical(); // Set Stream Path (broadcast)
               if (dest == CLA_BROADCAST && count == 3 && (buffer[1] == (byte)((physical >> 8) & 0xFF) && buffer[2] == (byte)(physical & 0xFF))) { 
                 if (_promiscuous) {
                   DbgReceive(source, dest, buffer, count);
                 }
                 reportStreamState(physical, source); 
               }
               break;
    case 0x8F: if (dest == logicalAddress) { // Give Device Power Status (directly addressed)
                 if (_promiscuous) {
                   DbgReceive(source, dest, buffer, count);
                 }
                 reportPowerState(source); 
               }
               break;
    case 0x9F: if (dest == logicalAddress) { // Get CEC version (directly addressed)
                 if (_promiscuous) {
                   DbgReceive(source, dest, buffer, count);
                 }
                 reportCECVersion(source);
               }
               break;  
    case 0x46: if (dest == logicalAddress) { // Give OSD name (directly addressed)
                 if (_promiscuous) {
                   DbgReceive(source, dest, buffer, count);
                 }
                 reportOSDName(source);
               }
               break;
    case 0x8C: if (dest == logicalAddress) { // Give Vendor Id (directly addressed)
                 if (_promiscuous) {
                   DbgReceive(source, dest, buffer, count);
                 }
                 reportVendorID(source);   
               }
               break;
    case 0x44: if (dest == logicalAddress) { // User Control Pressed (no reply) (directly addressed)
                 if (_promiscuous) {
                   DbgReceive(source, dest, buffer, count);
                 }
                 keymap.map(buffer[1], _promiscuous);
               }
               break;
    case 0x45: if (dest == logicalAddress) { // User Control Released (no reply) (directly addressed)
                 if (_promiscuous) {
                   DbgReceive(source, dest, buffer, count);
                 }
                 keymap.release(_promiscuous);
               }
               break;
    default:   if (_promiscuous) {
                 if ((dest == logicalAddress) || (dest == CLA_BROADCAST)) {
                   DbgReceive(source, dest, buffer, count);
                 }  
               }
               break;
  }
}

void Chdmicec::setMode(bool promiscuous, bool monitormode) {
  // set promiscuous mode true
  ceclient->setPromiscuous(promiscuous);
  // enable monitor mode (do not transmit)
  _monitorMode = monitormode;
  ceclient->setMonitorMode(_monitorMode);
  _promiscuous = promiscuous;
  return;
}

int Chdmicec::getLogicalAddress() {
  return ceclient->getLogicalAddress();
}

bool Chdmicec::setLogicalAddress() {
  return ceclient->setLogicalAddress();
}

// Privates

void Chdmicec::onReceiveCB(int source, int dest, unsigned char* buffer, int count) {
  hdmicec.onReceive(source, dest, buffer, count);
}

void Chdmicec::onTransmitCB(int source, int dest, unsigned char* buffer, int count) {
  if (hdmicec._promiscuous) {
    hdmicec.DbgTransmit(source, dest, buffer, count);
  }
}

void Chdmicec::reportPhysAddr(int dest) { // (broadcast)
  uint16_t physical = keymap.getPhysical();
  unsigned char frame[4] = { 0x84, (byte)((physical >> 8) & 0xFF), (byte)(physical & 0xFF), ceclient->getDeviceType() }; 
  ceclient->write(CLA_BROADCAST, frame, sizeof(frame)); 
  Serial.println("$ReportPhysAddr");
  powerOn(true, dest);
} // report physical address

void Chdmicec::reportStreamState(uint16_t physical, int dest) {  // (broadcast)
  unsigned char frame[3] = { 0x82, (byte)((physical >> 8) & 0xFF), (byte)(physical & 0xFF) };       
  ceclient->write(CLA_BROADCAST, frame, sizeof(frame)); 
  Serial.println("$ReportStreamState");
  if (!focus) {
    focus = true;
    Serial.println("$Focus");
    powerOn(false, dest);
  }
} // report stream state (playing)
    
void Chdmicec::reportPowerState(int dest) { 
  // 0 = on, 1 = standby
  unsigned char powerstate = power ? 0x00 : 0x01; 
  unsigned char frame[2] = { 0x90, powerstate }; 
  ceclient->write(dest, frame, sizeof(frame));
  Serial.println("$ReportPowerState");
} // report power state (on)

void Chdmicec::reportCECVersion(int dest) {
  unsigned char frame[2] = { 0x9E, 0x04 };
  ceclient->write(dest, frame, sizeof(frame));
  Serial.println("$ReportCECVersion");
} // report CEC version (v1.3a)
    
void Chdmicec::reportOSDName(int dest) { 
  char OSDName[L_OSDNAME];
  keymap.getOSDName(OSDName);
  unsigned char frame[strlen(OSDName)+1] = { 0x47 };
  strcpy(&frame[1], OSDName);
  ceclient->write(dest, frame, sizeof(frame));
  Serial.println("$ReportOSDName");
}
    
void Chdmicec::reportVendorID(int dest) {
  unsigned char frame[4] = { 0x87, 0x00, 0xF1, 0x0E };
  ceclient->write(dest, frame, sizeof(frame));
  Serial.println("$ReportVendorID");
} // report fake vendor ID

void Chdmicec::DbgReceive(int source, int dest, unsigned char* buffer, int count) {
  char prtbuf[128];
  // This is called when a frame is received.  To transmit
  // a frame call TransmitFrame.  To receive all frames, even
  // those not addressed to this device, set Promiscuous to true.
  sprintf(prtbuf, "Packet received at %ld (%02d->%02d) %02X", millis(), source, dest, ((source&0x0f)<<4)|(dest&0x0f));
  Serial.print(prtbuf);
  for (int i = 0; i < count; i++) {
    sprintf(prtbuf, ":%02X", buffer[i]);
    Serial.print(prtbuf);
  }
  Serial.println("");
}

void Chdmicec::DbgTransmit(int source, int dest, unsigned char* buffer, int count) {
  char prtbuf[128];
  // This is called when a frame is transmitted.
  sprintf(prtbuf, "Packet transmitted at %ld (%02d->%02d) %02X", millis(), source, dest, ((source&0x0f)<<4)|(dest&0x0f));
  Serial.print(prtbuf);
  for (int i = 0; i < count; i++) {
    sprintf(prtbuf, ":%02X", buffer[i]);
    Serial.print(prtbuf);
  }
  Serial.println("");
}

void Chdmicec::powerOn(bool onPower, int dest) {
    if (power) {
    return;
  }
  
  byte ps = keymap.getPowerOn();
  if (onPower) {
    if (ps != POWER_POWER) {
      if (ps == POWER_DISABLED) { // switch power status, even if disabled 
        power = true; 
      }
      return;
    }
  } else {
    if (ps != POWER_FOCUS) {
      return;
    }
  }
  
  byte key = keymap.map(CEC_KEYCODE_POWER_ON_FUNCTION, _promiscuous);
  if (key > 0) {
    power = true;
    Serial.println("$PowerOn");
    keymap.release(_promiscuous);
    reportPowerState(dest);
  }
}

void Chdmicec::powerStandby(bool onPower, int dest) {
  if (!power) {
    return;
  }
  
  byte ps = keymap.getPowerStandby();
  if (onPower) {
    if (ps != POWER_POWER) {
      if (ps == POWER_DISABLED) { // switch power status, even if disabled 
        power = false; 
      }
      return;
    }
  } else {
    if (ps != POWER_FOCUS) {
      return;
    }
  }
  
  byte key = keymap.map(CEC_KEYCODE_POWER_OFF_FUNCTION, _promiscuous);
  if (key > 0) {
    power = false;
    Serial.println("$PowerStandby");
    keymap.release(_promiscuous);
    reportPowerState(dest);
  }  
}

Chdmicec hdmicec;
