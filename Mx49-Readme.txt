SYSTEM EXCLUSIVE 

TRANSMIT
  IDENTITY REPLY
    F0 7E 7F 06 02 43 00 41 47 06 00 00 00 7F F7

  BULK DUMP
    F0 43 0n 7F 17 bh bl ah am al 0? dd ... dd cc F7
  PARAMETER CHANGE
    F0 43 1n 7F 17 ah am al dd ... dd F7

RECEIVE
  IDENTITY REQUEST
    F0 7E 0n 06 01 F7
  GM MODE ON
    F0 7E 7F 09 01 F7
  MIDI MASTER VOLUME
    F0 7F 7F 04 01 11 mm F7 (mm = volume)

  PARAMETER CHANGE
    F0 43 1n 7F 17 ah am al dd ... dd F7
  BULK DUMP REQUEST
    F0 43 2n 7F 17 ah am al F7
  PARAMETER REQUEST
    F0 43 3n 7F 17 ah am al F7

F0 43 0n = TRANSMIT BULK DUMP
F0 43 1n = TRANSMIT PARAMETER CHANGE
      	 = RECEIVE PARAMETER CHANGE
F0 43 2n = RECEIVE BULK DUMP REQUEST
F0 43 3n = RECEIVE PARAMETER REQUEST
