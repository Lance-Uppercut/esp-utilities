#include "ConsoleCommandHandler.h"
#include "ConsoleHandler.cpp"
#include <TelnetStream.h>

char *theVersion = "the version";
char *componentName = "the component name";

ConsoleHandler *serialConsoleHandler = new ConsoleHandler(componentName, theVersion, &Serial);
ConsoleHandler *telnetConsoleHandler = new ConsoleHandler(componentName, theVersion, &TelnetStream);
void setup() {

  //serialConsoleHandler->setRestartHandler({}->ESP.restart());
  //telnetConsoleHandler->setRestartHandler({}->ESP.restart());
}

void loop() {
  serialConsoleHandler->handle();
  telnetConsoleHandler->handle();
}