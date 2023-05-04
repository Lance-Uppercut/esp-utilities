#include <ESP8266HTTPClient.h>
#include "ConsoleCommandHandler.h"
#include "ConsoleHandler.cpp"
#include <TelnetStream.h>
#include "OtaHandler.h"
#include "OtaHandler.cpp"
char *theVersion = "the version";
char *componentName = "the component name";

WiFiClient http;
char user[] = "q1672933475805";
char socketPassword[] = "bc863bc7-33f2-4198-a4a0-31bc66ebfdf7";


ConsoleHandler *serialConsoleHandler = new ConsoleHandler(componentName, theVersion, &Serial);
ConsoleHandler *telnetConsoleHandler = new ConsoleHandler(componentName, theVersion, &TelnetStream);
  //OtaHandler(WiFiClient *wifiClient, char *deviceId, char *user, char *password, Stream *debugStream)

OtaHandlerImpl* otaHandler = new OtaHandlerImpl(&http, (char*)"deviceId", (char*)user, (char*)socketPassword, &Serial);
void setup() {

  //serialConsoleHandler->setRestartHandler({}->ESP.restart());
  //telnetConsoleHandler->setRestartHandler({}->ESP.restart());
}

void loop() {
  serialConsoleHandler->handle();
  telnetConsoleHandler->handle();
}