// To play a sequence of chord in sync with external midi events
#include <iostream>
#include <fstream>
#include <cstring>
#include <signal.h>
#include <set>

//#include <algorithm>
#include <chrono>
#include <thread>
//#include <ctime>
#include <iomanip>

// Channel Voice Messages
#define NOTEOFF 0x80
#define NOTEON 0x90
#define POLYAFTER 0xa0
#define CC 0xb0
#define PC 0xc0
#define MONOAFTER 0xd0
#define PITCHBEND 0xe0

// System Common Messages
#define MTCQF 0xf1 // MIDI Time Code Quarter Frame
#define SONGPOSPTR 0xf2 // Song Position Pointer
#define SONGSELECT 0xf3 // Song Select
// 0xf4 - 0xf5 Undefined (Reserved)
#define TUNEREQU 0xf6 // Tune Request
#define EOX 0xf7 // End Of Exclusive

// System Exclusive
#define SYSEX 0xf0

// System Real Time Messages
#define CLOCK 0xf8 // Timing Clock
// 0xf9 Undefined (Reserved)
#define START 0xfa // Start
#define CONT 0xfb // Continue
#define STOP 0xfc // Stop
// 0xfd Undefined (Reserved)
#define SENSING 0xfe // Active Sense
#define RESET 0xff // System Reset

// Note Number (Middle C = "C4" = 60)
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

// Convert from Midi Byte to Name (string)
std::string midiEventName(unsigned char event) {
  // Return name of Midi Status Byte (ie byte >= 0xf0)
  if (event >= 0xf0) { // Midi System Message
    switch (event) {
    case 0xf0: return("SYSEX"); break;
    case 0xf1: return("MTCQF"); break;
    case 0xf2: return("SONGPOSPTR"); break;
    case 0xf3: return("SONGSELECT"); break;
      //  case 0xf4: return("reserved"); break;
      //  case 0xf5: return("reserved"); break;
    case 0xf6: return("TUNEREQU"); break;
    case 0xf7: return("EOX"); break;
      //  case 0xf8: return("RTCLOCK"); break;
      //  case 0xf9: return("reserved"); break;
    case 0xfa: return("RTSTART"); break;
    case 0xfb: return("RTCONT"); break;
    case 0xfc: return("RTSTOP"); break;
      //  case 0xfd: return("reserved"); break;
    case 0xfe: return("RTSENSIG"); break;
    case 0xff: return("RESET"); break;
    default: return("reserved system message");
    }
  } // Return name of Channel Message
  else if (event >= 0x80) { // Channel Voice Message
    unsigned char midi_channel = 1 + event & 0x0f;
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
  else { // Midi Data Message
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

void clk() {
  // Called whenever 0xF8 (CLK) midi System Real Time message is received
  // Update average period between CLK event
  static const int clk_count = 24; // Number of clocks used for the avergage
  static int period_us[clk_count] = {0}; // ring buffer of periods in microseconds
  static int sum_periods_us = 0; // sum of the ring buffer
  static int i_period_us = 0; // current pointer in period_us[] ring buffer
  static std::chrono::time_point<std::chrono::steady_clock> clk_clock; // previous absolute clock

  auto now = std::chrono::steady_clock::now();
  auto current_period = (std::chrono::duration_cast<std::chrono::microseconds>(now - clk_clock));
  clk_clock = now;

  sum_periods_us += current_period.count() - period_us[i_period_us];
  period_us[i_period_us] = current_period.count();
  i_period_us++;
  if (i_period_us == clk_count) {
    if (sum_periods_us != 0) {
      // Period in [us] of "clk_count" midi CLK events = sum_periods_us
      // Period in [us] of one midi CLK event = sum_periods_us / clk_count
      // Period in [s] of one midi CLK events = sum_periods_us / (clk_count * 1'000'000)
      // Frequency in [Hz] of 1 midi CLK events = (clk_count * 1'000'000) / sum_periods_us
      // 1 midi CLK event = 1/24 quarter
      // BPM = number of quarter per minute = number of quarter per 60 s
      // BPM = f*60/24 = 2.5 * (clk_count * 1'000'000) / sum_periods_us = 2_500_000 * clk_count/sum_periods_us
      std::cout << "bpm=" << 2500000*clk_count/sum_periods_us << "\n";
    } else {
      std::cout << "bpm=undef";
    }
    i_period_us=0;
  }
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
    clk();
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
      //      playChord(127, chord[(measure_count-1)/2]);
      // std::cout << "clock: " << std::dec << clock_count << std::hex << " b=" << beat_count << " m=" << measure_count << " chord=" << chord[(measure_count - 1)/ 2] + " ON\n";
    }
    else if (clock_count % 24 == 23) {
      //      playChord(0, chord[(measure_count-1)/2]);
      // std::cout << "clock: " << std::dec << clock_count << std::hex << " b=" << beat_count << " m=" << measure_count << " chord=" << chord[(measure_count - 1)/ 2] + " OFF\n";
    }
    clock_count = (clock_count+1) % (24*4*4); // (24 clocks/quarter * 4 quarters/measure * 4 measures)
  }
  if (byte != 0xfe && byte != 0xf8) {
    //    std::cout << midiEventName(byte) << "\n";
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

