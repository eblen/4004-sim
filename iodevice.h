#ifndef IODEVICE_H
#define IODEVICE_H

#include "types.h"

class iodevice
{
  public:
  virtual void   port_output(Nibble i) = 0;
  virtual Nibble port_input ()         = 0;

  virtual ~iodevice() {}
};

#endif
