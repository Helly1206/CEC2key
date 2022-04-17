/* 
 * CEC2key - control
 * Serial control and debug
 * Hardware: Sparkfun Pro Micro
 * Version 0.80
 * 6-2-2022
 * Copyright: Ivo Helwegen
 */

#ifndef control_h
#define control_h

#define DEBUG_CONTROL

#define MODE_RUN      0
#define MODE_MONITOR  1
#define MODE_LISTEN   2
#define MODE_SIMULATE 3

#define CMD_LEN       20

class Ccontrol {
public:
  Ccontrol(); // constructor
  void init();
  void handle(); 
  void Byte2HEX(byte bte, char *Hex);
private:
  char command[CMD_LEN];
  bool commandAvailable;
  byte mode;
  unsigned long titleTime; 
  void title();
  void helpSim();
  void help();
  void handleCommand();
  void printError();
  void printOK();
  byte HEX2Byte(char *Hex);
  byte INT2Byte(char* txt);
  void updateMode(byte newMode);
};

extern Ccontrol control;

#endif
