#ifndef CONSOLE_COMMAND_HANDLER_h
#define CONSOLE_COMMAND_HANDLER_h

#include <Arduino.h>

class ConsoleCommandHandler {
  public:
    virtual void handle() = 0;
};

#endif
