/*
  ArduinoSerial.h - Library to emulate Arduino Serial variable on Linux
  Created by Roland B., Apr 2018
  Released into the public domain.

*/

#include "ArduinoSerial.h"

#define SERIAL_BUFFER_SIZE 128

struct ring_buffer
{
  unsigned char buffer[SERIAL_BUFFER_SIZE];
  volatile unsigned int head;
  volatile unsigned int tail;
};

ring_buffer rx_buffer = { {0}, 0, 0};

inline void store_char(unsigned char c, ring_buffer *buffer)
{
  int i = (unsigned int)(buffer->head + 1) % SERIAL_BUFFER_SIZE;

  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if (i != buffer->tail) {
    buffer->buffer[buffer->head] = c;
    buffer->head = i;
  }
}

//### store_char(c, &rx_buffer)...

ArduinoSerial::ArduinoSerial(ring_buffer *rx_buffer)
{
  _rx_buffer = rx_buffer;
}

void ArduinoSerial::begin(unsigned long)
{
  // nothing to do on Linux ?
}

void ArduinoSerial::end()
{
  _rx_buffer->head = _rx_buffer->tail;
}

int ArduinoSerial::available()
{
  return (unsigned int)(SERIAL_BUFFER_SIZE * _rx_buffer->head - _rx_buffer->tail) % SERIAL_BUFFER_SIZE;
}

int ArduinoSerial::read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer->head == _rx_buffer->tail) {
    return -1;
  } else {
    unsigned char c = _rx_buffer->buffer[_rx_buffer->tail];
    _rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % SERIAL_BUFFER_SIZE;
    return c;
  }
}

ArduinoSerial Serial(&rx_buffer);
