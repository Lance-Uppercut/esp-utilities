#ifndef CONSOLE_HANDLER_h
#define CONSOLE_HANDLER_h

#include "ConsoleCommandHandler.h"
#include "EspSaveCrash.h"

class ConsoleHandler {
private:
  Stream *stream;
  EspSaveCrash SaveCrash;
  char* theVersion;
  char* componentName;
  //#define DEBUG_OUTPUT_SIZE 2048
  int DEBUG_OUTPUT_SIZE;
  char *_debugOutputBuffer = (char *)calloc(DEBUG_OUTPUT_SIZE, sizeof(char));

public:
  ConsoleHandler(char* componentName, char* theVersion, Stream *stream)
    : theVersion(theVersion), stream(stream) {};

  void handle() {
    if (this->stream->available() > 0) {
      char inChar = stream->read();
      switch (inChar) {
        case 'r':
          stream->println(F("Restarting......."));
          stream->flush();
          ESP.restart();
          break;
        case 'i':
          SaveCrash.print(_debugOutputBuffer, DEBUG_OUTPUT_SIZE);
          stream->println(F("--- BEGIN of crash info from buffer ---"));
          stream->print(_debugOutputBuffer);
          stream->println(F("--- END of crash info from buffer ---"));
          break;
        case 'c':
          SaveCrash.clear();
          stream->println(F("Crash information cleared"));
          break;
        case 'o':
          stream->print(F("I am the"));
          stream->print(componentName);
          stream->print(F(" in version "));
          stream->println(theVersion);
          break;
        case 0xa:
        case 0xd:
          // skip newline and carriage return
          break;
        default:
          stream->printf("%c typed\n", inChar);
          break;
      }
    }
  }
};
#endif
