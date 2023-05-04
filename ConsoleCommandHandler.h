#pragma once

#include <Arduino.h>

class ConsoleCommandHandler {
  public:
    virtual void handle() = 0;
};
