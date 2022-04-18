/* 
 * CEC2key - hdmicec
 * HDMI CEC control
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 6-2-2022
 * Copyright: Ivo Helwegen
 */

#ifndef hdmicec_h
#define hdmicec_h

#include "CEC.h"

#define DEBUG_HDMICEC

#define CEC_INPUT_PIN           2
#define CEC_OUTPUT_PIN          3 //9

#define CEC_KEYCODE_POWER_OFF_FUNCTION 0x6C
#define CEC_KEYCODE_POWER_ON_FUNCTION  0x6D

#define POWER_DISABLED     0
#define POWER_POWER        1
#define POWER_FOCUS        2
#define POWER_FOCUS_ALWAYS 3

class Chdmicec {
public:
  Chdmicec(); // constructor
  void init();
  void handle(); 
  void onReceive(int source, int dest, unsigned char* buffer, int count);
  void setMode(bool promiscuous, bool monitormode);
  int getLogicalAddress();
  bool setLogicalAddress();
private:
  bool _monitorMode;
  bool _promiscuous;
  bool power;
  bool focus;
  CEC *ceclient;
  static void onReceiveCB(int source, int dest, unsigned char* buffer, int count);
  static void onTransmitCB(int source, int dest, unsigned char* buffer, int count);
  void reportPhysAddr(int dest);
  void reportStreamState(uint16_t physical, int dest);
  void reportPowerState(int dest);
  void reportCECVersion(int dest);
  void reportOSDName(int dest);
  void reportVendorID(int dest);
  void DbgReceive(int source, int dest, unsigned char* buffer, int count);
  void DbgTransmit(int source, int dest, unsigned char* buffer, int count);
  void powerOn(bool onPower, int dest);
  void powerStandby(bool onPower, int dest);
  unsigned long startdelay;
};

extern Chdmicec hdmicec;

#endif
