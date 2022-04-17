#!/usr/bin/python3

# -*- coding: utf-8 -*-
#########################################################
# SCRIPT : cecserial.py                                 #
#          zserial communication with CEC controller    #
#                                                       #
#          I. Helwegen 2022                             #
#########################################################

####################### IMPORTS #########################
import sys
import serial
from threading import Thread, Event, Lock
import signal
#########################################################

####################### GLOBALS #########################
RUN      = 0
MONITOR  = 1
LISTEN   = 2
SIMULATE = 3

specialKeys = {
        "KEY_NONE": 0x00,
        "KEY_ENTER": 0x0D,
        "KEY_SPACE": 0x20,
        "KEY_LEFT_CTRL": 0x80,
        "KEY_LEFT_SHIFT": 0x81,
        "KEY_LEFT_ALT": 0x82,
        "KEY_LEFT_GUI": 0x83,
        "KEY_RIGHT_CTRL": 0x84,
        "KEY_RIGHT_SHIFT": 0x85,
        "KEY_RIGHT_ALT": 0x86,
        "KEY_RIGHT_GUI": 0x87,
        "KEY_UP_ARROW": 0xDA,
        "KEY_DOWN_ARROW": 0xD9,
        "KEY_LEFT_ARROW": 0xD8,
        "KEY_RIGHT_ARROW": 0xD7,
        "KEY_BACKSPACE": 0xB2,
        "KEY_TAB": 0xB3,
        "KEY_RETURN": 0xB0,
        "KEY_ESC": 0xB1,
        "KEY_INSERT": 0xD1,
        "KEY_DELETE": 0xD4,
        "KEY_PAGE_UP": 0xD3,
        "KEY_PAGE_DOWN": 0xD6,
        "KEY_HOME": 0xD2,
        "KEY_END": 0xD5,
        "KEY_CAPS_LOCK": 0xC1,
        "KEY_F1": 0xC2,
        "KEY_F2": 0xC3,
        "KEY_F3": 0xC4,
        "KEY_F4": 0xC5,
        "KEY_F5": 0xC6,
        "KEY_F6": 0xC7,
        "KEY_F7": 0xC8,
        "KEY_F8": 0xC9,
        "KEY_F9": 0xCA,
        "KEY_F10": 0xCB,
        "KEY_F11": 0xCC,
        "KEY_F12": 0xCD}

cecCodes = {
        "CEC_KEYCODE_SELECT": 0x00,
        "CEC_KEYCODE_UP": 0x01,
        "CEC_KEYCODE_DOWN": 0x02,
        "CEC_KEYCODE_LEFT": 0x03,
        "CEC_KEYCODE_RIGHT": 0x04,
        "CEC_KEYCODE_RIGHT_UP": 0x05,
        "CEC_KEYCODE_RIGHT_DOWN": 0x06,
        "CEC_KEYCODE_LEFT_UP": 0x07,
        "CEC_KEYCODE_LEFT_DOWN": 0x08,
        "CEC_KEYCODE_ROOT_MENU": 0x09,
        "CEC_KEYCODE_SETUP_MENU": 0x0A,
        "CEC_KEYCODE_CONTENTS_MENU": 0x0B,
        "CEC_KEYCODE_FAVORITE_MENU": 0x0C,
        "CEC_KEYCODE_EXIT": 0x0D,
        "CEC_KEYCODE_MEDIA_TOP_MENU": 0x10,
        "CEC_KEYCODE_MEDIA_CONTEXT_SENSITIVE_MENU": 0x11,
        "CEC_KEYCODE_NUMBER_ENTRY_MODE": 0x1D,
        "CEC_KEYCODE_NUMBER_11": 0x1E,
        "CEC_KEYCODE_NUMBER_12": 0x1F,
        "CEC_KEYCODE_NUMBER_0_OR_NUMBER_10": 0x20,
        "CEC_KEYCODE_NUMBERS_1": 0x21,
        "CEC_KEYCODE_NUMBERS_2": 0x22,
        "CEC_KEYCODE_NUMBERS_3": 0x23,
        "CEC_KEYCODE_NUMBERS_4": 0x24,
        "CEC_KEYCODE_NUMBERS_5": 0x25,
        "CEC_KEYCODE_NUMBERS_6": 0x26,
        "CEC_KEYCODE_NUMBERS_7": 0x27,
        "CEC_KEYCODE_NUMBERS_8": 0x28,
        "CEC_KEYCODE_NUMBERS_9": 0x29,
        "CEC_KEYCODE_DOT": 0x2A,
        "CEC_KEYCODE_ENTER": 0x2B,
        "CEC_KEYCODE_CLEAR": 0x2C,
        "CEC_KEYCODE_NEXT_FAVORITE": 0x2F,
        "CEC_KEYCODE_CHANNEL_UP": 0x30,
        "CEC_KEYCODE_CHANNEL_DOWN": 0x31,
        "CEC_KEYCODE_PREVIOUS_CHANNEL": 0x32,
        "CEC_KEYCODE_SOUND_SELECT": 0x33,
        "CEC_KEYCODE_INPUT_SELECT": 0x34,
        "CEC_KEYCODE_DISPLAY_INFORMATION": 0x35,
        "CEC_KEYCODE_HELP": 0x36,
        "CEC_KEYCODE_PAGE_UP": 0x37,
        "CEC_KEYCODE_PAGE_DOWN": 0x38,
        "CEC_KEYCODE_POWER": 0x40,
        "CEC_KEYCODE_VOLUME_UP": 0x41,
        "CEC_KEYCODE_VOLUME_DOWN": 0x42,
        "CEC_KEYCODE_MUTE": 0x43,
        "CEC_KEYCODE_PLAY": 0x44,
        "CEC_KEYCODE_STOP": 0x45,
        "CEC_KEYCODE_PAUSE": 0x46,
        "CEC_KEYCODE_RECORD": 0x47,
        "CEC_KEYCODE_REWIND": 0x48,
        "CEC_KEYCODE_FAST_FORWARD": 0x49,
        "CEC_KEYCODE_EJECT": 0x4A,
        "CEC_KEYCODE_FORWARD": 0x4B,
        "CEC_KEYCODE_BACKWARD": 0x4C,
        "CEC_KEYCODE_STOP_RECORD": 0x4D,
        "CEC_KEYCODE_PAUSE_RECORD": 0x4E,
        "CEC_KEYCODE_RESERVED": 0x4F,
        "CEC_KEYCODE_ANGLE": 0x50,
        "CEC_KEYCODE_SUB_PICTURE": 0x51,
        "CEC_KEYCODE_VIDEO_ON_DEMAND": 0x52,
        "CEC_KEYCODE_ELECTRONIC_PROGRAM_GUIDE": 0x53,
        "CEC_KEYCODE_TIMER_PROGRAMMING": 0x54,
        "CEC_KEYCODE_INITIAL_CONFIGURATION": 0x55,
        "CEC_KEYCODE_SELECT_BROADCAST_TYPE": 0x56,
        "CEC_KEYCODE_SELECT_SOUND_PRESENTATION": 0x57,
        "CEC_KEYCODE_PLAY_FUNCTION": 0x60,
        "CEC_KEYCODE_PAUSE_PLAY_FUNCTION": 0x61,
        "CEC_KEYCODE_RECORD_FUNCTION": 0x62,
        "CEC_KEYCODE_PAUSE_RECORD_FUNCTION": 0x63,
        "CEC_KEYCODE_STOP_FUNCTION": 0x64,
        "CEC_KEYCODE_MUTE_FUNCTION": 0x65,
        "CEC_KEYCODE_RESTORE_VOLUME_FUNCTION": 0x66,
        "CEC_KEYCODE_TUNE_FUNCTION": 0x67,
        "CEC_KEYCODE_SELECT_MEDIA_FUNCTION": 0x68,
        "CEC_KEYCODE_SELECT_AV_INPUT_FUNCTION": 0x69,
        "CEC_KEYCODE_SELECT_AUDIO_INPUT_FUNCTION": 0x6A,
        "CEC_KEYCODE_POWER_TOGGLE_FUNCTION": 0x6B,
        "CEC_KEYCODE_POWER_OFF_FUNCTION": 0x6C,
        "CEC_KEYCODE_POWER_ON_FUNCTION": 0x6D,
        "CEC_KEYCODE_F1_BLUE": 0x71,
        "CEC_KEYCODE_F2_RED": 0x72,
        "CEC_KEYCODE_F3_GREEN": 0x73,
        "CEC_KEYCODE_F4_YELLOW": 0x74,
        "CEC_KEYCODE_F5": 0x75,
        "CEC_KEYCODE_DATA": 0x76}

#########################################################

###################### FUNCTIONS ########################

#########################################################
# Class : cecserial                                     #
#########################################################
class cecserial(object):
    def __init__(self):
        self.settings = {}
        self.sendThread = None
        self.term = Event()
        self.term.clear()
        self.mutex = Lock()
        self.serial = serial.Serial()

    def __del__(self):
        self.serial.close()
        del self.mutex
        del self.term

    def run(self, argv):
        result = True
        signal.signal(signal.SIGINT, self.exit_app)
        signal.signal(signal.SIGTERM, self.exit_app)
        self.handleArgs(argv)
        self.serial.baudrate = self.settings["baudrate"]
        self.serial.port = self.settings["port"]
        self.serial.timeout = 1
        try:
            self.serial.open()
        except:
            print("Error opening serial port")
            result = False
        if result:
            done = False
            if self.settings["loadkeys"]:
                self.loadKeys()
                done = True
            if self.settings["osdname"]:
                self.setOSDName()
                done = True
            if self.settings["phyaddr"]:
                self.setPhy()
                done = True
            if self.settings["poweron"]:
                self.setPowerOnState()
                done = True
            if self.settings["powerstdby"]:
                self.setPowerStandbyState()
                done = True
            if done:
                print("Ready")
            else:
                self.enterMode()
                self.terminal()

        exit(0 if result else 1)

    def handleArgs(self, argv):
        print("CECserial: serial connection to arduino CEC2key v0.8")
        aind = 1
        self.settings["port"] = "/dev/ttyACM0"
        self.settings["baudrate"] = 115200
        self.settings["loadkeys"] = ""
        self.settings["osdname"] = ""
        self.settings["phyaddr"] = ""
        self.settings["poweron"] = ""
        self.settings["powerstdby"] = ""
        self.settings["clearkeys"] = False
        self.settings["mode"] = RUN

        exists = True
        while exists:
            exists, value = self.getInd(argv, aind)
            if exists:
                aind += 1
                if value == "-h":
                    self.printHelp()
                elif value == "-p":
                    exists, value = self.getInd(argv, aind)
                    if exists:
                        aind += 1
                        self.settings["port"] = value
                    else:
                        print("This option requires a port to be entered")
                        self.printError()
                elif value == "-b":
                    exists, value = self.getInd(argv, aind)
                    if exists:
                        aind += 1
                        br = 0
                        try:
                            br = int(argv[2])
                        except:
                            print("Invalid baudrate entered, use default")
                        if br:
                            self.settings["baudrate"] = br
                    else:
                        print("This option requires a baudrate to be entered")
                        self.printError()
                elif value == "-k":
                    exists, value = self.getInd(argv, aind)
                    if exists:
                        aind += 1
                        self.settings["loadkeys"] = value
                    else:
                        print("This option requires a filename to be entered")
                        self.printError()
                elif value == "-c":
                    self.settings["clearkeys"] = True
                elif value == "-n":
                    exists, value = self.getInd(argv, aind)
                    if exists:
                        aind += 1
                        self.settings["osdname"] = value
                    else:
                        print("This option requires a OSD name to be entered")
                        self.printError()
                elif value == "-a":
                    exists, value = self.getInd(argv, aind)
                    if exists:
                        aind += 1
                        self.settings["phyaddr"] = value
                    else:
                        print("This option requires a physical address to be entered")
                        self.helpPhyAddr()
                        self.printError()
                elif value == "-o":
                    exists, value = self.getInd(argv, aind)
                    if exists:
                        aind += 1
                        self.settings["poweron"] = value
                    else:
                        print("This option requires a state method to be entered")
                        self.printError()
                elif value == "-t":
                    exists, value = self.getInd(argv, aind)
                    if exists:
                        aind += 1
                        self.settings["powerstdby"] = value
                    else:
                        print("This option requires a state method to be entered")
                        self.printError()
                elif value == "-m":
                    self.settings["mode"] = MONITOR
                elif value == "-l":
                    self.settings["mode"] = LISTEN
                elif value == "-s":
                    self.settings["mode"] = SIMULATE
                else:
                    self.printError()

    def getInd(self, argv, ind):
        exists = False
        value = ""

        if len(argv)>ind:
            exists = True
            value = argv[ind]

        return exists, value

    def printHelp(self):
        print("Option:")
        print("    -h: print this help file and exit")
        print("    -p: enter serial port [-p /dev/ttyS0]")
        print("    -b: enter serial baudrate [-b 115200]")
        print("    -k: load key codes from file and exit [-k /path/to/file]")
        print("    -c: clear keys before loading new key codes")
        print("    -n: set OSD name [-n name]")
        print("    -a: set physical address (HDMI port) [-a n-n-n-n]")
        print("    -o: set power on state ([d]isabled, [p]ower, [f]ocus)")
        print("    -t: set power standby state ([d]isabled, [p]ower, [f]ocus)")
        print("    -m: enter terminal mode, monitor cec commands")
        print("    -l: enter terminal mode, monitor cec commands, don't send key commands")
        print("    -s: enter terminal mode, simulate key commands")
        print("    (*) A non-specific command enters terminal mode")
        exit(0)

    def printError(self):
        print("Enter cecserial -h for help")
        exit(1)

    def enterMode(self):
        if self.settings["mode"] == MONITOR:
            self.sendCommand("mm")
        elif self.settings["mode"] == LISTEN:
            self.sendCommand("ml")
        elif self.settings["mode"] == SIMULATE:
            self.sendCommand("ms")
        else:
            self.sendCommand("mr")

    def loadKeys(self):
        keyCodes = self.loadFile()
        if self.settings["clearkeys"]:
            print("Clearing keys ...")
            self.sendCommand("ck")
        print("Loading keys ...")
        for cec, key in keyCodes.items():
            cmd = "sk>{}:{}".format(cec, key)
            self.sendCommand(cmd)

    def setOSDName(self):
        print("Set OSD name ...")
        name = self.settings["osdname"][:13] if len(self.settings["osdname"]) > 13 else self.settings["osdname"]
        cmd = "sn>{}".format(name)
        self.sendCommand(cmd)

    def setPhy(self):
        address = self.setAddress()
        print("Set physical address ...")
        cmd = "spa>{}".format(address)
        self.sendCommand(cmd)

    def setPowerOnState(self):
        print("Set power on state ...")
        cmd = "spo>{}".format(self.setState(self.settings["poweron"]))
        self.sendCommand(cmd)

    def setPowerStandbyState(self):
        print("Set power standby state ...")
        cmd = "sps>{}".format(self.setState(self.settings["powerstdby"]))
        self.sendCommand(cmd)

    def terminal(self):
        try:
            self.sendThread = Thread(target=self.send, args=( ))
            self.sendThread.daemon = True
            self.sendThread.start()
        except:
            self.sendThread = None
            print("ERROR Creating thread")

        while not self.term.isSet():
            if self.serial.is_open:
                line = self.serial.readline().decode("utf-8")
                if line:
                    self.mutex.acquire()
                    print(line, end = "")
                    self.mutex.release()

    def sendCommand(self, command):
        cmd = command + "\n"
        resp = ">" + command
        self.serial.write(cmd.encode("utf-8"))
        line = self.serial.readline().decode("utf-8").strip("\n")
        if resp in line:
            line = self.serial.readline().decode("utf-8").strip("\n")
        if not "<OK" in line:
            print("Error executing command: " + command)

    def send(self):
        while not self.term.isSet():
            line = input()
            if line and self.serial.is_open:
                self.mutex.acquire()
                line += "\n"
                self.serial.write(line.encode("utf-8"))
                self.mutex.release()

    def loadFile(self):
        ok = True
        keyCodes = {}

        try:
            with open(self.settings["loadkeys"]) as f:
                cont = True
                while cont:
                    line = f.readline().strip()
                    if line:
                        ok, cec, key = self.processLine(line)
                        if ok:
                            if cec:
                                keyCodes[cec] = key
                        else:
                            cont = False
                    else:
                        cont = False
        except:
            print("File error")
            ok = False

        if not ok:
            print("Incorrect file format")
            print("Use: <cec-code> <key> (spaces or tab as separator,")
            print("                       every new code on a new line)")
            print("codes can be entered as hex number (preceded by 0x) or")
            print("CEC_... cec specific codes or")
            print("ascii character")
            print("Special keys can be entered as KEY_...")
            print("Examples:")
            print("0x41                     0x42")
            print("A                        B")
            print("CEC_KEYCODE_SELECT       KEY_RETURN")
            exit(1)

        return keyCodes

    def setAddress(self):
        address = list("0000")
        try:
            subAddresses = self.settings["phyaddr"].split("-")
            i = 0
            if len(subAddresses)>0 and len(subAddresses)<5:
                while i<len(subAddresses):
                    hexchar = "{0:0{1}X}".format(int(subAddresses[i]), 1)
                    address[i] = hexchar
                    i+=1
                    if hexchar == "0":
                        if i == 1:
                            raise Exception("HDMI port 0 not allowed here")
                        if i < len(subAddresses):
                            hexchar = "{0:0{1}X}".format(int(subAddresses[i]), 1)
                            if hexchar != "0":
                                raise Exception("HDMI port 0 not allowed here")
            else:
                raise Exception("Too many HDMI ports")
        except:
            self.helpPhyAddr()
            exit(1)

        return "".join(address)

    def helpPhyAddr(self):
        print("Enter HDMI port number of up to 4 devices, seperated by '-'")
        print("Every device can have maximum 15 HDMI ports (enter 1 to 15)")
        print("Examples:")
        print("1       :TV HDMI port 1")
        print("4       :TV HDMI port 4")
        print("1-3     :TV HDMI port 1 - box 1 port 3")
        print("1-3-4   :TV HDMI port 1 - box 1 port 3 - box 2 port 4")
        print("1-3-4-2 :TV HDMI port 1 - box 1 port 3 - box 2 port 4 - box 3 port 2")

    def setState(self, statevar):
        state = 0
        if statevar[0].lower() == "p":
            state = 1
        elif statevar[0].lower() == "f":
            state = 2
        return state

    def processLine(self, line):
        ok = True
        cec = ""
        key = ""

        sline = line.split()
        if len(sline) >= 2:
            if sline[0].startswith("0x"):
                icec = -1
                try:
                    icec = int(sline[0], 16)
                except:
                    ok = False
                if icec >= 0 and icec <= 255:
                    try:
                        cec = "{0:0{1}X}".format(icec, 2)
                    except:
                        ok = False
            elif sline[0].startswith("CEC_"):
                try:
                    icec = cecCodes[sline[0]]
                    cec = "{0:0{1}X}".format(icec, 2)
                except:
                    ok = False
            elif len(sline[0]) == 1:
                try:
                    cec = "{0:0{1}X}".format(ord(sline[0]), 2)
                except:
                    ok = False
            else:
                ok = False
            if ok:
                if sline[1].startswith("0x"):
                    ikey = -1
                    try:
                        ikey = int(sline[1], 16)
                    except:
                        ok = False
                    if ikey >= 0 and ikey <= 255:
                        try:
                            key = "{0:0{1}X}".format(ikey, 2)
                        except:
                            ok = False
                elif sline[1].startswith("KEY_"):
                    try:
                        ikey = specialKeys[sline[1]]
                        key = "{0:0{1}X}".format(ikey, 2)
                    except:
                        ok = False
                elif len(sline[1]) == 1:
                    try:
                        key = "{0:0{1}X}".format(ord(sline[1]), 2)
                    except:
                        ok = False
                else:
                    ok = False
        else:
            ok = True # nicely process empty lines

        return ok, cec, key

    def exit_app(self, signum, frame):
        print("Exit app")
        self.term.set()

#########################################################

######################### MAIN ##########################
if __name__ == "__main__":
    cecserial().run(sys.argv)
