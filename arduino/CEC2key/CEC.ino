#include "CEC.h"

#define CEC_HIGH LOW
#define CEC_LOW HIGH

int CEC::_validLogicalAddresses[6][5] = { 
    {CLA_TV,                  CLA_FREE_USE,           CLA_UNREGISTERED,       CLA_UNREGISTERED, CLA_UNREGISTERED, },
    {CLA_RECORDING_DEVICE_1,  CLA_RECORDING_DEVICE_2, CLA_RECORDING_DEVICE_3, CLA_UNREGISTERED, CLA_UNREGISTERED, },
    {CLA_UNREGISTERED,        CLA_UNREGISTERED,       CLA_UNREGISTERED,       CLA_UNREGISTERED, CLA_UNREGISTERED, },
    {CLA_TUNER_1,             CLA_TUNER_2,            CLA_TUNER_3,            CLA_TUNER_4,      CLA_UNREGISTERED, },
    {CLA_PLAYBACK_DEVICE_1,   CLA_PLAYBACK_DEVICE_2,  CLA_PLAYBACK_DEVICE_3,  CLA_UNREGISTERED, CLA_UNREGISTERED, },
    {CLA_AUDIO_SYSTEM,        CLA_UNREGISTERED,       CLA_UNREGISTERED,       CLA_UNREGISTERED, CLA_UNREGISTERED, },
    };

#define MAKE_ADDRESS(s,d) ((((s) & 0xf) << 4) | ((d) & 0xf))

// create a new instance of CEC
CEC::CEC(int physicalAddress, int inputPin, int outputPin) : CECframes(CLA_UNREGISTERED) {
  _physicalAddress = physicalAddress;
  _logicalAddress = CLA_UNREGISTERED;
  _waitTime = 0;
  _deviceType = CDT_OTHER;
  _claState = CEC_CLA_XMIT_POLLING_MESSAGE;
  _claIndex = 0;

  _onReceiveCallback = NULL;
  _onTransmitCallback = NULL;
  _onTransmitCompleteCallback = NULL; 
  
  _isrTriggered = false;
  _lastLineState2 = true;
  _in_line = inputPin;
  _out_line = outputPin<0 ? inputPin : outputPin;
  
  _ready = false; 
}

// init CEClient 
void CEC::begin(CEC_DEVICE_TYPE type) {
  pinMode(_out_line, OUTPUT);
  pinMode( _in_line,  INPUT);

  SetLineState(true);
  delay(200);

  CECframes::Initialize();
  _deviceType = type;

  if (MonitorMode) {
    _claState = CEC_CLA_READY;
  }
}

// return the ready state
bool CEC::isReady() {
    if (_ready) {
      _ready = false;
      return true;
    } else {
      return false;
    }
}

// write a packet on the bus
bool CEC::write(int targetAddress, unsigned char* buffer, int count) {
  if (_claState != CEC_CLA_IDLE)
    return false;

  unsigned char addr[1];

  if (_onTransmitCallback)
    _onTransmitCallback(_logicalAddress, targetAddress, buffer, count);

  addr[0] = MAKE_ADDRESS(_logicalAddress, targetAddress);
  ClearTransmitBuffer();
  if (!TransmitPartial(addr, 1))
    return false;
  return Transmit(buffer, count);
}

// return the logical address
int CEC::getLogicalAddress() {
  return _logicalAddress;
}

bool CEC::setLogicalAddress() {
  if ((_claState == CEC_CLA_IDLE) && (!MonitorMode)) {
    _claState = CEC_CLA_XMIT_POLLING_MESSAGE;
    _claIndex = 0;
    _logicalAddress = CLA_UNREGISTERED;
    _ready = false;
    return true;
  }
  return false;
}

byte CEC::getDeviceType() {
  return (byte)(_deviceType & 0xFF);
}

// enable-disable promiscuous mode
void CEC::setPromiscuous(bool promiscuous) {
    Promiscuous = promiscuous;
}

// enable-disable monitor mode
void CEC::setMonitorMode(bool monitorMode) {
    MonitorMode = monitorMode;
}

// set callback function when a transmit is complete
void CEC::onTransmitCompleteCallback(OnTransmitCompleteCallbackFunction callback) {
    _onTransmitCompleteCallback = callback;
}

// set callback function when a new packet is received
void CEC::onReceiveCallback(OnReceiveCallbackFunction callback) {
    _onReceiveCallback = callback;
}

// set callback function when a new packet is received
void CEC::onTransmitCallback(OnTransmitCallbackFunction callback) {
    _onTransmitCallback = callback;
}

// run, to be executed in the loop for the FSM
void CEC::run() {  
  bool state = LineState();
  if (_lastLineState2 != state) {
    _lastLineState2 = state;
    SignalIRQ();
  } 
  
  // Initial pump for the state machine (this will cause a transmit to occur)
  while (!ProcessStateMachine(NULL));

  if (((_waitTime == (unsigned long)-1 && !TransmitPending()) || (_waitTime != (unsigned long)-1 && _waitTime > micros())) && !IsISRTriggered())
    return;
    
  unsigned long wait = Process();
  if (wait != (unsigned long)-2)
    _waitTime = wait;
  return;
}

// ----- PRIVATE METHODS -----

// OnTransmitComplete redefinition, if a callback function is available, call it
void CEC::OnTransmitComplete(bool success) {
  if (_onTransmitCompleteCallback) 
    _onTransmitCompleteCallback(success);
  //Serial.println("TRA COMPLETE");
  if (_claState == CEC_CLA_RCV_POLLING_MESSAGE && _logicalAddress == CLA_UNREGISTERED) {
    while (!ProcessStateMachine(&success));
  }
}

// OnReceive redefinition, if a callback function is available, call it
// if not, call the parent function
void CEC::OnReceive(int source, int dest, unsigned char* buffer, int count) {
  if (_onReceiveCallback)
    _onReceiveCallback(source, dest, buffer, count);        

  /*
  if (!MonitorMode && dest == _logicalAddress && count == 1 && buffer[0] == 0x83) {
    unsigned char buffer[4];
    buffer[0] = 0x84;
    buffer[1] = _physicalAddress >> 8;
    buffer[2] = _physicalAddress;
    buffer[3] = _deviceType;
    write(CLA_BROADCAST, buffer, 4);
  } */ 
}

bool CEC::LineState() {
  int value = digitalRead(_in_line);
  /*Serial.print("get:");
  Serial.println(value == CEC_HIGH);*/
  return value == CEC_HIGH;
}

void CEC::SetLineState(bool state) {
  digitalWrite(_out_line, state?CEC_HIGH:CEC_LOW);
  // give enough time for the line to settle before sampling
  // it
  delayMicroseconds(CEC_INPUT_SETTLE_TIME);
  _lastLineState2 = LineState();
  //_lastLineState2 = state;
  /*Serial.print("set:");
  Serial.println(state);*/
}

void CEC::SignalIRQ() {
  // This is called when the line has changed state
  _isrTriggered = true;
}

bool CEC::IsISRTriggered() {
  if (_isrTriggered) {
    _isrTriggered = false;
    return true;
  }
  return false;
}

bool CEC::ProcessStateMachine(bool* success) {
  unsigned char buffer[1];
  bool wait = false;

  /*if (_claState != CEC_CLA_IDLE) {
    Serial.print("ST:");
    Serial.print(_claState);
    Serial.print(",");
    Serial.println(_claIndex);
  }*/

  switch (_claState) {
  case CEC_CLA_XMIT_POLLING_MESSAGE:
    // Section 6.1.3 specifies that <Polling Message> while allocating a Logical Address
    // will have the same initiator and destination address
    buffer[0] = MAKE_ADDRESS(_validLogicalAddresses[_deviceType][_claIndex], _validLogicalAddresses[_deviceType][_claIndex]);
    if (_onTransmitCallback)
      _onTransmitCallback(_validLogicalAddresses[_deviceType][_claIndex], _validLogicalAddresses[_deviceType][_claIndex], buffer, 0);
    ClearTransmitBuffer();
    Transmit(buffer, 1);
      
    _claState = CEC_CLA_RCV_POLLING_MESSAGE;
    wait = true;
    break;

  case CEC_CLA_RCV_POLLING_MESSAGE:
    if (success) {
      if (*success) {
        // Someone is there, try the next
        _claIndex++;
        if (_validLogicalAddresses[_deviceType][_claIndex] != CLA_UNREGISTERED)
          _claState = CEC_CLA_XMIT_POLLING_MESSAGE;
        else {
          _logicalAddress = CLA_UNREGISTERED;
          _claState = CEC_CLA_READY;
        }
      } else {
        // We hereby claim this as our logical address!
        _logicalAddress = _validLogicalAddresses[_deviceType][_claIndex];
        SetAddress(_logicalAddress);
        _claState = CEC_CLA_READY;
      }
    }
    else
      wait = true;
    break;

  case CEC_CLA_READY:
    _claState = CEC_CLA_IDLE;
    _ready = true;
    wait = true;
    break;

  case CEC_CLA_IDLE:
    wait = true;
    break;
  }

  return wait;
}

void CEC::OnReceiveComplete(unsigned char* buffer, int count) {
  if (count >= 1) { //was ASSERT
    int sourceAddress = (buffer[0] >> 4) & 0x0f;
    int targetAddress = buffer[0] & 0x0f;
    OnReceive(sourceAddress, targetAddress, buffer + 1, count - 1);
  }
}
