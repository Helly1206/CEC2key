#ifndef CEC_H__
#define CEC_H__

#include "CECframes.h"

#define CEC_INPUT_SETTLE_TIME 50
#define CLA_BROADCAST         0x0F

class CEC: public CECframes {

typedef void (*OnReceiveCallbackFunction)(int, int, unsigned char*, int);
typedef void (*OnTransmitCallbackFunction)(int, int, unsigned char*, int);
typedef void (*OnTransmitCompleteCallbackFunction)(bool);

public:
  typedef enum {
    CDT_TV,
    CDT_RECORDING_DEVICE,
    CDT_OTHER,        // Not a real CEC type..
    CDT_TUNER,  
    CDT_PLAYBACK_DEVICE,
    CDT_AUDIO_SYSTEM,
  } CEC_DEVICE_TYPE;

  CEC(int physicalAddress, int inputPin, int outputPin=-1);
  void begin(CEC_DEVICE_TYPE type = CDT_PLAYBACK_DEVICE);
  bool isReady();
  bool write(int targetAddress, unsigned char* buffer, int count);
  int getLogicalAddress();
  bool setLogicalAddress();
  byte getDeviceType();
  void setPromiscuous(bool promiscuous);
  void setMonitorMode(bool monitorMode);
  void onTransmitCompleteCallback(OnTransmitCompleteCallbackFunction);
  void onReceiveCallback(OnReceiveCallbackFunction);
  void onTransmitCallback(OnTransmitCallbackFunction);
  void run();

private:
  bool ProcessStateMachine(bool* success);
  void OnReceiveComplete(unsigned char* buffer, int count);
  void OnTransmitComplete(bool);
  bool LineState();
  void SetLineState(bool);
  void SignalIRQ();
  bool IsISRTriggered();
  void OnReceive(int source, int dest, unsigned char* buffer, int count);
  OnTransmitCompleteCallbackFunction _onTransmitCompleteCallback;
  OnReceiveCallbackFunction _onReceiveCallback;
  OnTransmitCallbackFunction _onTransmitCallback;
  bool _ready;
  bool _isrTriggered;
  bool _lastLineState2;
  int  _in_line, _out_line;

  typedef enum {
    CLA_TV = 0,
    CLA_RECORDING_DEVICE_1,
    CLA_RECORDING_DEVICE_2,
    CLA_TUNER_1,
    CLA_PLAYBACK_DEVICE_1,
    CLA_AUDIO_SYSTEM,
    CLA_TUNER_2,
    CLA_TUNER_3,
    CLA_PLAYBACK_DEVICE_2,
    CLA_RECORDING_DEVICE_3,
    CLA_TUNER_4,
    CLA_PLAYBACK_DEVICE_3,
    CLA_RESERVED_1,
    CLA_RESERVED_2,
    CLA_FREE_USE,
    CLA_UNREGISTERED,
  } CEC_LOGICAL_ADDRESS;

  typedef enum {
    CEC_CLA_IDLE,
    CEC_CLA_READY,
    CEC_CLA_XMIT_POLLING_MESSAGE,
    CEC_CLA_RCV_POLLING_MESSAGE,
  } CEC_CLA_STATE;

  static int _validLogicalAddresses[6][5];
  int _logicalAddress;
  int _physicalAddress;
  unsigned long _waitTime;

  CEC_DEVICE_TYPE _deviceType;
  CEC_CLA_STATE _claState;
  int _claIndex;
  
};

#endif // CEC_H__
