#include <iostream>
#include <fstream>
#include <signal.h>
#include <set>
#include "Event.h"

#define NOTEOFF 0x80
#define NOTEON 0x90
#define POLYAFTER 0xa0
#define CC 0xb0
#define PC 0xc0
#define MONOAFTER 0xd0
#define PITCHBEND 0xe0
#define SYSEX 0xf0
#define MTCQF 0xf1
#define SONGPOSPTR 0xf2
#define SONGSELECT 0xf3
#define TUNEREQU 0xf6
#define ENDEX 0xf7
#define CLOCK 0xf8
#define START 0xfa
#define CONT 0xfb
#define STOP 0xfc
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

std::set<Event> todo;


void setup() {
  int i=0;
  todo.insert({i++, (float)(random() & 255) / (float)256.0, {0x90, C4, 63}});
  todo.insert({i++, (float)(random() & 255) / (float)256.0, {0x90, D4, 63}});
  todo.insert({i++, (float)(random() & 255) / (float)256.0, {0x90, E4, 63}});
  todo.insert({i++, (float)(random() & 255) / (float)256.0, {0x90, F4, 63}});
  todo.insert({i++, (float)(random() & 255) / (float)256.0, {0x90, G4, 63}});
  todo.insert({i++, (float)(random() & 255) / (float)256.0, {0x90, A4, 63}});
  todo.insert({i++, (float)(random() & 255) / (float)256.0, {0x90, B4, 63}});

  for (std::set<Event>::iterator it=todo.begin(); it != todo.end();it++) {
    std::cout<< *it;
  }

  device.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
  try {
    device.open(port.c_str(), std::ios::binary);
  }
  catch (std::ifstream::failure e) {
    std::cerr << "Exception opening/reading/closing file:" << port << "\n";
    exit(1);
  }
}

void loop() {
  char buffer[1];
  unsigned char byte;

  static unsigned char current_state = 0;
  static unsigned char channel = 0;

  device.read(buffer, 1);
  byte = buffer[0];

  if (byte & 0x80) {
    if ( byte >=  0xf0 ) { // System Message
      std::cout << "SYSTEM:" << std::hex << (int)byte << "\n";
      if (byte == SENSING) {
	std::cout << "SENSE\n";
      }
    }
    else { // Channel Message, Status followed by 0, 1 or 2 DataByte
      channel = byte & 0x0f;
      current_state = byte & 0xf0;
      std::cout << "CMD:" << std::hex << (int)current_state;
      std::cout << " CHANNEL:" << std::hex << 1+(int)channel << "\n";
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
