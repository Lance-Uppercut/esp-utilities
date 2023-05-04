#pragma once

#include <Arduino.h>

class OtaHandler {
public:
  virtual void doOtaIfNeeded(char *fileName, char *theVersion) = 0;
  virtual int getLastError() =0;
  virtual String getLastErrorString() = 0;
};
