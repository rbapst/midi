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
1111 0001 0nnn dddd                         ; MIDI Time Code Quarter Frame; 
1111 0010 0lll llll 0mmm mmmm 		    ; Song Position Pointer;
1111 0011 0sss ssss        		    ; Song Select;
1111 0100      				    ; Undefined (Reserved);
1111 0101      				    ; Undefined (Reserved);
1111 0110				    ; Tune Request;
1111 0111                                   ; End Of Exclusive;             ENDEX

System Real Time
1111 1000 ;Timing Clock;         CLK
1111 1001 ;Undefined (Reserved);
1111 1010 ;Start;                START
1111 1011 ;Continue;             CONT
1111 1100 ;Stop;                 STOP
1111 1101 ;Undefined (Reserved);
1111 1110 ;Active Sensing;       SENSE
1111 1111 ;Reset;                RESET


	1
	2 6 3 1
	8 4 2 6 8 4 2 1
120     0 1 1 1 1 0 0 0

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



