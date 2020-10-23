Channel Voice Messages
1000 cccc 0nnn nnnn 0vvv vvvv ;Note Off;               Channel cccc Key nnn nnnn Velocity vvv vvvv
1001 cccc 0nnn nnnn 0vvv vvvv ;Note On;                Channel cccc Key nnn nnnn Velocity vvv vvvv
1010 cccc 0nnn nnnn 0vvv vvvv ;Polyphonic Key Presure; Channel cccc Key nnn nnnn Presure vvv vvvv
1011 cccc 0ccc cccc 0vvv vvvv ;Control Change;         Channel cccc Control ccc cccc <120 Value vvv vvvvv
1100 cccc 0ppp pppp           ;Program Change;         Channel cccc New program = ppp pppp
1101 cccc 0vvv vvvv           ;Channel Presure;        Channel cccc Presure = vvv vvvvv
1110 cccc 0lll llll 0mmm mmmm ;Pitch Bend Change;      Channel cccc Change = mmm mmmm 2^7 + lll llll - 0x2000

Channel Mode Messages
1011 cccc 0111 1ccc 0vvv vvvv ;Channel Mode Message;   Channel cccc Control = 120 - 127 Value vvv

Control Change:
  0 -  31 = MSB of most continuous Controller Data
 32 -  63 = LSB for controllers 0 - 31
 64 -  95 = Additional single byte  controllers
 96 - 101 = Increment/Decrement and Parameter numbers
102 - 119 = Undefined single byte controllers

  0 -  32 = Bank Select
1011 cccc 0111 1000 0000 0000 ;All Sound Off;          Channel cccc All Sound Off
1011 cccc 0111 1001 0vvv vvvv ;Reset All;              Channel cccc Reset All Controller Control = 121 Value vvv vvvv
1011 cccc 0111 1010 0vvv vvvv ;Local Control Off;      Channel cccc Local Control Off (Value = 0) / On (Value = 127)
1011 cccc 0111 1011 0000 0000 ;All Notes Off;          Channel cccc All Notes Off Value = 0
1011 cccc 0111 1100 0000 0000 ;Omni Mode Off;          Channel cccc Omni Mode Off
1011 cccc 0111 1101 0000 0000 ;Omni Mode On;           Channel cccc Omni Mode On
1011 cccc 0111 1110 0vvv vvvv ;Mono Mode On;           Channel cccc Mono Mode On Number channel = vvv vvvvv
1011 cccc 0111 1111 0000 0000 ;Poly Mode On;           Channel cccc Poly Mode On (Mono Off)

System Common Messages
1111 0000 0iii iiii OR                  0x* ; System Exclusive Start;       SYSEX
1111 0000 0iii iiii 0iii iiii 0iii iiii 0x* ;
1111 0000 0111 1101                         ; Universal System Exclusive for schools, reseach, ... (F0 7D)
1111 0000 0111 1110                         ; Universal System Exclusive Non-Real Time (F0 7E)
1111 0000 0111 1111                         ; Universal System Exclusive Real Time (F0 7F)
1111 0001 0nnn dddd                         ; MIDI Time Code Quarter Frame; MTC Quarter Frame (there is a MTC Full Frame in Exclusive mode)
1111 0010 0lll llll 0mmm mmmm 		    ; Song Position Pointer;
1111 0011 0sss ssss        		    ; Song Select;
1111 0100      				    ; Undefined (Reserved);
1111 0101      				    ; Undefined (Reserved);
1111 0110				    ; Tune Request;
1111 0111                                   ; End Of Exclusive;             ENDEX

Universal System Exclusive
F0 7E/7F <device ID> <sub-id#1> <sub-id#2> ... F7
  device ID = device but... not so clear how device should be numbered... . 7F = all channel

F0 7E <device ID> 06 01 F7 = Inq General Information / Identity Request
F0 7E <device ID> 06 02 mm ff ff  dd dd ss ss ss ss F7 = Inq General Information / Identiy Response
    mm = Manufacturer
    ff ff = device family
    dd dd = device family member code
    ss ss ss ss = software release

F0 7E <device ID> 08 00 tt F7 = Bulk Tuning Dump Request
    F0 7E         Universal Non-Real Time SysEx header
    <device ID>   ID of target device
    08            sub-ID#1 = MIDI Tuning Standard
    00            sub-ID#2 = 00H, bulk dump request)
    tt            tuning program number (0 – 127)
    F7            EOX

System Exclusive Messages
F0 <ID Number (1 or 3 bytes)> <device ID (channel)> <sub-id#1> ... F7

DISTRIBUTION OF ID NUMBERS
            American   European    Japanese    Other      Special
1 byte ID:  01 - 1F    20 - 3F     40 - 5F     60 - 7C    7D - 7F
3 byte ID:  00 00 01-  00 20 00-   00 40 00-   00 60 00-
            00 1F 7F   00 3F 7F    00 5F 7F    00 7F 7F
(if 1 byte ID = 00 --> go to 3 bytes id (first is 00))
1 byte ID 00 and 3 bytes ID 00 00 00 are not to be used:
--> YAMAHA = 43

(DGX-200:
f0 7e 7f 06 02 43 00 44 4d 15 00 00 00 00 f7
43 = yamaha
00 44 = family
4d 15 = device in family
00 00 00 00 = software version
)
(MX-49: family = 00 41, device = 47 06)

System Real Time
1111 1000 ;Timing Clock;         CLK
1111 1001 ;Undefined (Reserved);
1111 1010 ;Start;                START
1111 1011 ;Continue;             CONT
1111 1100 ;Stop;                 STOP
1111 1101 ;Undefined (Reserved);
1111 1110 ;Active Sensing;       SENSE
1111 1111 ;Reset;                RESET

Expression = volume variation around main volume
	1
	2 6 3 1
	8 4 2 6 8 4 2 1
120     0 1 1 1 1 0 0 0

Running Status = last status received

Receive algo:
State:
  - wait for value only
  - wait for value or command...
CurrentCommand = None

command stack =
while:
	read value;
	if it's command --> start command
	else if last running command == null --> next
	else push value in command
		if command completed --> play command; clear queue;

Channel Mode:
  - 2 = Omni ON Mono
  - 1 = Omni ON Poly
  - 4 = Omni OFF Mono
  - 3 = Omni OFF Poly

Midi receive:
Dec  Hex Bin
  0-127 = Data Byte
128-255 = Status Byte

Message Type:
  o Channel Message
      o Channel Voice Message
      o Channel Mode Message
  o System Message
     o System Exclusive Message
     o System Common Message
     o System Real-Time Message

Structure of a single message:
  o Status
  o Status DataByte
  o Status DataByte DataByte
Structure of a system exclusive message:
  o Status DataBytes EOX

000-127 00-7F 0000 0000-0111 1111 Data Byte (value)               Channel Voice Messages
128-143 80-8F 1000 0000-1000-1111 Note Off                        Channel Voice Messages
144-159 90-9F 1001 0000-1001-1111 Note On                      	  Channel Voice Messages
160-175 A0-AF 1010 0000-1010-1111 Polyphonic Key Presure          Channel Voice Messages
176-191 B0-BF 1011 0000-1011-1111 Control Change                  Channel Mode Messages
192-207 C0-CF 1100 0000-1100-1111 Program Change                  Channel Voice Messages
208-223 D0-DF 1101 0000-1101-1111 Channel Presure                 Channel Voice Messages
224-239 E0-EF 1110 0000-1110-1111 Pitch Bend                      Channel Voice Messages
240     F0    1111 0000           System Excusive Start           System Exclusive Messages
241     F1    1111 0001           MIDI Time Code Quarter Frame    System Common Messages
242     F2    1111 0010           Song Position Pointer           System Common Messages
243     F3    1111 0011           Song Select                     System Common Messages
244-245 F4-F5 1111 0100-1111 0101 Undefined (Reserved)            System Common Messages
246     F6    1111 0110	          Tune Request                    System Common Messages
247     F7    1111 0111           End Of Exclusive                System Exclusive Messages
248     F8    1111 1000           Timing Clock               CLK  System Real Time
249     F9    1111 1001           Undefined (Reserved)            System Real Time
250     FA    1111 1010           Start                    START  System Real Time
251     FB    1111 1011           Continue                  CONT  System Real Time
252     FC    1111 1100           Stop                      STOP  System Real Time
253     FD    1111 1101           Undefined (Reserved)            System Real Time
254     FE    1111 1110           Active Sensing           SENSE  System Real Time
255     FF    1111 1111           Reset                    RESET  System Real Time



STATE MACHINE:
  WAIT_FOR_MESSAGE
     WAIT_FOR_3_VALUES
     WAIT_FOR_2_VALUES
     WAIT_FOR_1_VALUE
     WAIT_FOR_SYSEX_MAN_ID
     WAIT_FOR_SYSEX_2ND_ID
     WAIT_FOR_SYSEX_3RD_ID
     WAIT_FOR_SYSEX_DEV_ID
     WAIT_FOR_VALUE_OR_ENDEX
     WAIT_FOR_SYSEX_SUBID1
     WAIT_FOR_SYSEX_SUBID2

2 states to manage:
 1) Midi State (current and running)
 2) Parser State

NOTEOFF_CMD
NOTEOFF_1ST_VALUE
NOTEOFF_2ND_VALUE
NOTEON_CMD
NOTEON_1ST_VALUE
NOTEON_2ND_VALUE
poly 2
cc 2
pc 1
cp 1
pb 2
sc ...o
