#ifndef Event_h
#define Event_h
#include <iostream>
struct MidiEvent
{
  unsigned char midicode;
  unsigned char velocity;
  unsigned char key;
};

class Event
{
 public:
  int order;
  float time;
  MidiEvent note;
  Event(int order, float time, MidiEvent note)
    : order(order)
    , time(time)
    , note(note)
  {}
  bool operator< (const Event & eventObj)const;
 private:
};
std::ostream& operator<<(std::ostream &os, const Event &event);
#endif

