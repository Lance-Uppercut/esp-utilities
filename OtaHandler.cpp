#ifndef OTA_HANDLER_impl_H
#define OTA_HANDLER_impl_H

#include <Arduino.h>

#include <ESP8266HTTPClient.h>
//#include <ESP8266httpClient.h>
#include <ESP8266httpUpdate.h>
#include "OtaHandler.h"

class OtaHandlerImpl : public OtaHandler {
private:
  char *host = "soeren.herokuapp.com";
  //char path[] = "/name?device=z67oVnbt";
  char *deviceId;
  char *user;
  char *password;
  WiFiClient *wifiClient;
  Stream *debugStream;
public:
  OtaHandlerImpl(WiFiClient *wifiClient, char *deviceId, char *user, char *password, Stream *debugStream)
    : wifiClient(wifiClient), deviceId(deviceId), user(user), password(password),debugStream(debugStream) {}

 int getLastError() override {
   return ESPhttpUpdate.getLastError();
 };

 String getLastErrorString() override {
   return ESPhttpUpdate.getLastErrorString();
 };

  void doOtaIfNeeded(char *fileName, char *theVersion) override {
    //strstr returns null if the version isn't in the filename, else it returns a pointer to the string within the other stringt
    char *result = strstr(fileName, theVersion);
    boolean needToUpdate = (result == NULL);
    if (needToUpdate) {
      char buf[100];
      sprintf(buf, "http://%s/api/devices/%s/images", host, deviceId);
 //     Serial.print(F("Doingupdate for "));
   //   Serial.println(fileName);
   //   Serial.print("On url");
    //  Serial.println(buf);
     // Serial.flush();
      String fwv = String(fileName);
      String url = String(buf);

      ESPhttpUpdate.setAuthorization(user, password);
      t_httpUpdate_return ret = ESPhttpUpdate.update(* wifiClient, url, fwv);

      switch (ret) {
        case HTTP_UPDATE_FAILED:
          {
            int lastError;
            const char *lastErrorString;

            lastError = ESPhttpUpdate.getLastError();
            lastErrorString = ESPhttpUpdate.getLastErrorString().c_str();
            //Serial.println(lastErrorString);
            char error[50];
            sprintf(error, "[update] Update failed (%d): %s", lastError, lastErrorString);
   //         Serial.println(error);
          }
          break;
        case HTTP_UPDATE_NO_UPDATES:
     //     Serial.println("[update] Update no Update.");
          break;
        case HTTP_UPDATE_OK:
       //   Serial.println(F("[update] Update ok."));
          break;
      }
    } else {
     // Serial.println("No need to update using OTA. On latest version");
    }
  }
};
#endif