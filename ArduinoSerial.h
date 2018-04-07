/*
  ArduinoSerial.h - Library to emulate Arduino Serial variable on Linux
  Created by Roland B., Apr 2018
  Released into the public domain.

*/

#ifndef ArduinoSerial_h
#define ArduinoSerial_h

struct ring_buffer;

class ArduinoSerial 
{
 public:
  ArduinoSerial(ring_buffer *rx_buffer);
  void begin(unsigned long);
  void end();
  int available();
  virtual int read(void);
 private:
  ring_buffer *_rx_buffer;
};

extern ArduinoSerial Serial;

#endif

