#include <iostream>
#include <fstream>
#include <cstring>
#include <signal.h>
#include <set>

// Channel Voice Messages
#define NOTEOFF 0x80
#define NOTEON 0x90
#define POLYAFTER 0xa0
#define CC 0xb0
#define PC 0xc0
#define MONOAFTER 0xd0
#define PITCHBEND 0xe0

// System Common Messages
#define SYSEX 0xf0
#define MTCQF 0xf1 // MIDI Time Code Quarter Frame
#define SONGPOSPTR 0xf2
#define SONGSELECT 0xf3
// 0xf4 - 0xf5 Undefined (Reserved)
#define TUNEREQU 0xf6
#define ENDEX 0xf7

// System Real Time
#define CLOCK 0xf8
// 0xf9 Undefined (Reserved)
#define START 0xfa
#define CONT 0xfb
#define STOP 0xfc
// 0xfd Undefined (Reserved)
#define SENSING 0xfe
#define RESET 0xff

#define B3 59
#define C4 60
#define D4 62
#define E4 64
#define F4 65
#define G4 67
#define A4 69
#define B4 71
#define C5 72


bool done;
static void finish(int ignore) { done = true; }

std::string port = "/dev/snd/midiC1D0";
std::ifstream device;
std::ofstream midiOut;

void setup() {
  // Open midi ports
  device.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
  try {
    device.open(port.c_str(), std::ios::binary);
  }
  catch (std::ifstream::failure e) {
    std::cerr << "Exception opening/reading/closing file:" << port << "\n";
    exit(1);
  }
  midiOut.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
  try {
    midiOut.open(port.c_str(), std::ios::binary);
  }
  catch (std::ofstream::failure e) {
    std::cerr << "Exception opening/writing/closing file:" << port << "\n";
    exit(1);
  }
}

std::string midiEventName(unsigned char event) {
  if (event >= 0xf0) {
    switch (event) {
    case 0xf0: return("SYSEX"); break;
    case 0xf1: return("MTCQF"); break;
    case 0xf2: return("SONGPOSPTR"); break;
    case 0xf3: return("SONGSELECT"); break;
      //  case 0xf4: return("reserved"); break;
      //  case 0xf5: return("reserved"); break;
    case 0xf6: return("TUNEREQU"); break;
    case 0xf7: return("ENDEX"); break;
      //    case 0xf8: return("RTCLOCK"); break;
      //  case 0xf9: return("reserved"); break;
    case 0xfa: return("RTSTART"); break;
    case 0xfb: return("RTCONT"); break;
    case 0xfc: return("RTSTOP"); break;
      //  case 0xfd: return("reserved"); break;
      //    case 0xfe: return("RTSENSIG"); break;
    case 0xff: return("RESET"); break;
    default: return("");
    }
  } else if (event >= 0x80) {
    switch (event & 0xf0) {
    case 0x80: return("NOTEOFF"); break;
    case 0x90: return("NOTEON"); break;
    case 0xa0: return("POLYAFTER"); break;
    case 0xb0: return("CC"); break;
    case 0xc0: return("PC"); break;
    case 0xd0: return("MONOAFTER"); break;
    case 0xe0: return("PITCHBEND"); break;
    }
  }
  else {
    return(std::to_string(event));
  }
  return("");
}

void playChord(int velocity, const std::string& chord) {
  char buffer[3] = { (char) 0x90, 0, 0 };
  
  if (chord.compare("A")==0) { buffer[1] = 69; };
  if (chord.compare("E")==0) { buffer[1] = 64; };
  buffer[2]=velocity;
  midiOut.write(buffer, 3);
  midiOut.flush();
  std::cout << chord << " vel=" << velocity << "\n";

}

void loop() {
  char buffer[1];
  unsigned char byte;

  static unsigned char current_state = 0;
  static unsigned int clock_count = 0;
  static unsigned int beat_count = 0;
  static unsigned int measure_count = 1;
  static unsigned char channel = 0;
  static std::string chord[2] = { "Am", "E"};

  device.read(buffer, 1);
  byte = buffer[0];

  if (byte == CLOCK) {
    if (clock_count % 24 == 0) {
      //      std::cout << std::dec << clock_count << std::hex << "%24==0\n";
      beat_count++;
      if (beat_count == 5) {
	beat_count = 1;
	measure_count++;
	if (measure_count == 5) {
	  measure_count = 1;
	}
      }
    };
    if (clock_count % 24 == 0) {
      playChord(127, chord[(measure_count-1)/2]);
      // std::cout << "clock: " << std::dec << clock_count << std::hex << " b=" << beat_count << " m=" << measure_count << " chord=" << chord[(measure_count - 1)/ 2] + " ON\n";
    }
    else if (clock_count % 24 == 23) {
      playChord(0, chord[(measure_count-1)/2]);
      // std::cout << "clock: " << std::dec << clock_count << std::hex << " b=" << beat_count << " m=" << measure_count << " chord=" << chord[(measure_count - 1)/ 2] + " OFF\n";
    }
    clock_count = (clock_count+1) % (24*4*4); // (24 clocks/quarter * 4 quarters/measure * 4 measures)
  }
  if (byte != 0xfe && byte != 0xf8) {
    std::cout << midiEventName(byte) << "\n";
  }
  if ( byte & 0x80 ) {
    if ( byte >=  0xf0 ) { // System Message
      //std::cout << "SYSTEM:" << std::hex << (int)byte << "\n";
      // if (byte == SENSING) {
      //std::cout << "SENSE\n";
      //      }
    }
    else { // Channel Message, Status followed by 0, 1 or 2 DataByte
      // channel = byte & 0x0f;
      // current_state = byte & 0xf0;
      // std::cout << "CMD:" << std::hex << (int)current_state;
      // std::cout << " CHANNEL:" << std::hex << 1+(int)channel << "\n";
    }
  }
  else {
    std::cout << "VAL " << std::hex << (int)byte << "\n";
  }
}

int main(int argc, char *argv[]) {
  setup();
  done = false;
  (void) signal(SIGINT, finish);

  while(!done) {
    loop();
  }
  device.close();
}
