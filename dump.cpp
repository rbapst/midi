#include <iostream>
#include <fstream>
#include <signal.h>
#include <set>
#include "Event.h"

#define NOTEOFF 0x80
#define NOTEON  0x90
#define POLYAFTER   0xa0
#define CC 0xb0
#define PC 0xc0
#define MONOAFTER 0xd0
#define PITCHBEND 0xe0
#define SYSEX 0xf0
#define ENDEX 0xf7
#define CLOCK 0xf8
#define SENSING 0xfe

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
  todo.insert({i++, (double)(random() & 255) / (double)256.0, {0x90, C4, 63}});
  todo.insert({i++, (double)(random() & 255) / (double)256.0, {0x90, D4, 63}});
  todo.insert({i++, (double)(random() & 255) / (double)256.0, {0x90, E4, 63}});
  todo.insert({i++, (double)(random() & 255) / (double)256.0, {0x90, F4, 63}});
  todo.insert({i++, (double)(random() & 255) / (double)256.0, {0x90, G4, 63}});
  todo.insert({i++, (double)(random() & 255) / (double)256.0, {0x90, A4, 63}});
  todo.insert({i++, (double)(random() & 255) / (double)256.0, {0x90, B4, 63}});
  device.open(port.c_str(), std::ios::binary);
  for (std::set<Event>::iterator it=todo.begin(); it != todo.end();it++) {
    std::cout<< *it;
  }
}

void loop() {
  char buffer[1];
  unsigned char byte;
  
  device.read(buffer, 1);
  byte = buffer[0];
  if (byte & 0x80) {
    if ( byte == CLOCK) {} // ...
    else if ( byte == SENSING) {} // ...
    else {
      std::cout << "CMD " << std::hex << (int)byte << "\n";
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

