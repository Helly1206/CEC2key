CEC2key v0.8.1

CEC2key -- Convert HDMI CEC to a keyboard input
======= == ======= ==== === == = ======== =====

This is an arduino project to control devices over HDMI CEC. In my case I used it to control a kodi PC with the remote control of my TV. Keyboard inputs will be used to control kodi directly.

The provided keyfile is an example that uses my CEC commands to convert them into kodi key storkes. But it can be adapted if required.

Arduino:
--------

This project requires an arduino that has USB hardware like the arduino leonardo. In my case I used an arduino pro-micro containg the same hardware as a leonardo, only smaller.

A small schematic containing 2 transistors needs to be built to be able to read and write signals on the CEC bus.

The CECclient library is modified to solve a few bugs that made it not working on a Leonardo board and some small additions to make it more feasible for this purpose.
Sources:
https://www.arduino.cc/reference/en/libraries/ceclient/
https://github.com/floe/CEC

An arduino program is built around it to be able to modify parameters without needing to rebuild the arduino.

Commands from the terminal interface (/dev/ttyACM0, 115200 bps):

Change mode:
mm = monitor
ms = simulate
ml = listen
mr = run
Settings:
spa>xxxx = set physical address (xxxx in hex)
gpa      = get physical address
sla      = set logical address
gla      = get logical address
sn>aaaa  = set OSD name (string aaaa)
gn       = get OSD name
spo>n    = set power on state (n 0=none, 1=power, 2=focus)
gpo      = get power on state
sps>n    = set power standby state (n 0=none, 1=power, 2=focus)
gps      = get power standby state
sk>xx:yy = set key code (xx:yy in hex)
gk       = get key codes
ck       = clear key codes
tk       = toggle key control on/ off
rst      = reset device

In monitor mode all commands, responses and keystrokes are printed to the terminal
In listen mode, all HDMI CEC channels logged, but no data is transmitted
In run mode, only little information is printed
In simulate mode, the user is able to simulate some CEC codes to test keyboard responses
<<< Simulation mode >>>
exit = exit to run mode
gk   = get key codes
xx   = hex key to simulate

If required, the system can automatically power on or off by a keystroke (spo, gpo, sps, gps).
0=none, not used
1=power, power on if tv powers on, standby if tv enters standby mode
2=focus, power on if hdmi channel is selected, power off if a different device is selected
3=allfocus, power on (or off) every time this device is selected, even is already powered on (or off)
The CEC_KEYCODE_POWER_OFF_FUNCTION and CEC_KEYCODE_POWER_ON_FUNCTION need to be set as keycodes to make used of this

The physical address (spa, gpa) needs to be in line with the HDMI port to device is connected to. HDMI 1 on the TV corresponds with 1000 (2 > 2000, etc.)

The logical address is automatically detected. sla can re-search for a logical address. gla prints the current logical address.

The OSD name can be set by sn, gn. A maximum of 1 characters is allowed. More characters will be trimmed.

Key codes can be entered using sk (with gk and ck) but the preferred way is the use the python tool for that, as arduino accepts only hexadrecimal characters directly.

CEC codes:
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
"CEC_KEYCODE_DATA": 0x76

Special key codes:
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
"KEY_F12": 0xCD

For testing keyboard output can be switched off and on by tk. If off, no real keyboard commands will be send.

rst resets the device if required.

Python interface program:
------ --------- --------

A small python program is written to be able to control, setup and debug the device from it's serial interface, using a terminal interface or some direct sommands.

Options:

-h: print this help file and exit
-p: enter serial port [-p /dev/ttyS0]
-b: enter serial baudrate [-b 115200]
-k: load key codes from file and exit [-k /path/to/file]
-c: clear keys before loading new key codes
-n: set OSD name [-n name]
-a: set physical address (HDMI port) [-a n-n-n-n]
-o: set power on state ([d]isabled, [p]ower, [f]ocus)
-t: set power standby state ([d]isabled, [p]ower, [f]ocus)
-m: enter terminal mode, monitor cec commands
-l: enter terminal mode, monitor cec commands, don't send key commands
-s: enter terminal mode, simulate key commands
(*) A non-specific command enters terminal mode

Key codes can be loaded from key files. See the example file
Use: <cec-code> <key> (spaces or tab as separator,
                       every new code on a new line)
codes can be entered as hex number (preceded by 0x) or
CEC_... cec specific codes or
ascii character
Special keys can be entered as KEY_...
Examples:
0x41                     0x42
A                        B
CEC_KEYCODE_SELECT       KEY_RETURN

Setting the physical address to the HDMI port the device is connected to:
Enter HDMI port number of up to 4 devices, seperated by '-'
Every device can have maximum 15 HDMI ports (enter 1 to 15)
Examples:
1       :TV HDMI port 1
4       :TV HDMI port 4
1-3     :TV HDMI port 1 - box 1 port 3
1-3-4   :TV HDMI port 1 - box 1 port 3 - box 2 port 4
1-3-4-2 :TV HDMI port 1 - box 1 port 3 - box 2 port 4 - box 3 port 2

That's all for now ...

Please send Comments and Bugreports to hellyrulez@home.nl
