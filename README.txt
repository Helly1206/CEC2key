CEC2key v0.8.0

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
The CEC_KEYCODE_POWER_OFF_FUNCTION and CEC_KEYCODE_POWER_ON_FUNCTION need to be set as keycodes to make used of this

The physical address (spa, gpa) needs to be in line with the HDMI port to device is connected to. HDMI 1 on the TV corresponds with 1000 (2 > 2000, etc.)

The logical address is automatically detected. sla can re-search for a logical address. gla prints the current logical address.

The OSD name can be set by sn, gn. A maximum of 1 characters is allowed. More characters will be trimmed.

Key codes can be entered using sk (with gk and ck) but the preferred way is the use the python tool for that, as arduino accepts only hexadrecimal characters directly.

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
