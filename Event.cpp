#include "Event.h"
#include <iostream>
bool Event::operator< (const Event &eventObj) const
{
    return(this->time < eventObj.time);
}
std::ostream& operator<<(std::ostream &os, const Event &event)
{
  os<<"Event::("<<event.order<<","<<event.time;
  os<<","<<std::hex << (int)event.note.midicode;
  os<<","<<std::hex << (int)event.note.velocity;
  os<<","<<std::hex << (int)event.note.key;
  os<<")"<<std::endl;
}

