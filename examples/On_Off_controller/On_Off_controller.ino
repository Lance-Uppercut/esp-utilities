//#define ESP32_RTOS  // Uncomment this line if you want to use the code with freertos only on the ESP32
// Has to be done before including "OTA.h"

//NodeMCU 1.0 (esp-12E module)
//#include <WiFi.h>

#include <OTA.h>
//#include <credentials.h>
#include <WebSocketsClient.h>
#include <SimpleTimer.h>
//#include <SimpleDHT.h>
#include "DHT.h"
//makerextender_forhave


char* mySSID = "PrettyFlyForAWifi-2.4G";
char* myPASSWORD = "Zaq12wsx";

//const char* mySSID = "makerextender_forhave";
//const char* myPASSWORD = "12345678";


// constants won't change:
const long interval = 2000;  // interval at which to blink (milliseconds)
WebSocketsClient webSocketClient;
//WiFiClient client;

const byte BUILTIN_LED1 = 1;     //GPIO0
const int ledPin = LED_BUILTIN;  // the number of the LED pin

#define DHTPIN 10      // tx
#define dhtPowerPin 0  //gpio0
#define echoPin 13     //gpio2
#define trigPin 15     //rx
//0=gpio0
//1=gpio1 = tx
//2 = GPIO2
//3=GPIO3 = rx


//for prod
char host[] = "soeren.herokuapp.com";
char user[] = "q1641205362515";
char password[] = "2ee6be67-d357-446e-bd1b-865baa521216";
char path[] = "/name?device=qmBxxaf2";
char deviceId[] = "qmBxxaf2";
char sketchName[] = "On_off_example";

SimpleTimer timer;

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

boolean shouldReport = false;
boolean pumpOn = false;
void turnOnPump() {
  turnOnInternalLed();
  pumpOn = true;
}
void turnOnInternalLed() {
  digitalWrite(ledPin, LOW);
}
void turnOffInternalLed() {
  digitalWrite(ledPin, HIGH);
}
void turnOffPump() {
  turnOffInternalLed();
  pumpOn = false;
}
// a function to be executed periodically

void repeatMe() {
  TelnetStream.print("Uptime (s): ");
  TelnetStream.println(millis() / 1000);
  shouldReport = true;
}

void turnOffDht() {
  digitalWrite(dhtPowerPin, HIGH);
}

void turnOnDht() {
  digitalWrite(dhtPowerPin, HIGH);
}


float temperature = 0;
float humidity = 0;
DHT dht(DHTPIN, DHTTYPE);

void calculateTempHum() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    TelnetStream.println("Failed to read from DHT sensor!");
    turnOffDht();
    timer.setTimeout(2500, turnOnDht);
    return;
  }

  temperature = t;
  humidity = h;

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  TelnetStream.print("Humidity: ");
  TelnetStream.print(h);
  TelnetStream.print(" %\t");
  TelnetStream.print("Temperature: ");
  TelnetStream.print(t);
  TelnetStream.print(" *C ");
  TelnetStream.print(f);
  TelnetStream.print(" *F\t");
  TelnetStream.print("Heat index: ");
  TelnetStream.print(hic);
  TelnetStream.print(" *C ");
  TelnetStream.print(hif);
  TelnetStream.println(" *F");
}

float percent = 0;
long duration;   // variable for the duration of sound wave travel
float distance;  // variable for the distance measurement

void calculateDistance() {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2.0;  // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  TelnetStream.print("Distance: ");
  TelnetStream.print(distance);
  TelnetStream.println(" cm");
}

// Replace with your network credentials
const char* ssid = "ESP32-Access-Point";
const char* wifiPassword = "123456789";

char ipaddressString[256];

void reportIPAddress() {
  // send message to server when Connected
  IPAddress ipAddress = WiFi.localIP();

  sprintf(ipaddressString, "%d.%d.%d.%d", ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);
  TelnetStream.println(ipaddressString);
  //webSocketClient.sendTXT(ipaddressString);
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  char* deviceIdFromMessage;
  char* command;
  char* commandValue;
  switch (type) {
    case WStype_DISCONNECTED:
      TelnetStream.printf("[WSc] Disconnected!\n");
      restartIfNotConnected();
      break;
    case WStype_CONNECTED:
      {
        TelnetStream.printf("[WSc] Connected to url: %s\n", payload);
        timer.setTimeout(5000, reportIPAddress);
        shouldReport = true;
        turnOffInternalLed();
      }
      break;
    case WStype_TEXT:
      TelnetStream.printf("[WSc] get text: %s\n", payload);
      //14:58:50.193 -> [WSc] get text: bcb03298-2fcd-4cb3-a938-342d9b793e68,powerstate,TurnOn
      deviceIdFromMessage = strtok((char*)payload, ",");
      command = strtok(NULL, ",");
      commandValue = strtok(NULL, ",");
      TelnetStream.print("Command: ");
      TelnetStream.println(command);
      TelnetStream.print("Value: ");
      TelnetStream.println(commandValue);
      if (strcmp(command, "powerstate") == 0) {
        if (strcmp(commandValue, "TurnOn") == 0) {
          turnOnPump();
        } else {
          turnOffPump();
        }
        shouldReport = true;
      } else {
        TelnetStream.println("Unknown command");
      }

      // send message to server
      // webSocketClient.sendTXT("message here");
      break;
    case WStype_BIN:
      TelnetStream.printf("[WSc] get binary length: %u\n", length);
      //hexdump(payload, length);

      // send data to server
      // webSocketClient.sendBIN(payload, length);
      break;
    case WStype_PING:
      // pong will be send automatically
      TelnetStream.println("[WSc] get ping");
      break;
    case WStype_PONG:
      // answer to a ping we send
      TelnetStream.println("[WSc] get pong");
      break;
    default:
      TelnetStream.print("[WSc] default:");
      //      TelnetStream.printf(type);
      TelnetStream.println(type);
      break;
  }
}

void printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      TelnetStream.println("WEP");
      break;
    case ENC_TYPE_TKIP:
      TelnetStream.println("WPA");
      break;
    case ENC_TYPE_CCMP:
      TelnetStream.println("WPA2");
      break;
    case ENC_TYPE_NONE:
      TelnetStream.println("None");
      break;
    case ENC_TYPE_AUTO:
      TelnetStream.println("Auto");
      break;
  }
}


void listNetworks() {
  // scan for nearby networks:
  TelnetStream.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    TelnetStream.println("Couldn't get a wifi connection");
    while (true)
      ;
  }

  // print the list of networks seen:
  TelnetStream.print("number of available networks:");
  TelnetStream.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    TelnetStream.print(thisNet);
    TelnetStream.print(") ");
    TelnetStream.print(WiFi.SSID(thisNet));
    TelnetStream.print("\tSignal: ");
    TelnetStream.print(WiFi.RSSI(thisNet));
    TelnetStream.print(" dBm");
    TelnetStream.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
}

void restartIfNotConnected() {
  if (!WiFi.isConnected()) {
    ESP.restart();
  } else {
    TelnetStream.println("Still connected to wifi");
  }
}

void setup() {
  Serial.begin(115200);
  //Serial.println(F("Booting"));


  setupOTA(sketchName, mySSID, myPASSWORD);

  // Your setup code

  TelnetStream.println("Setup");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(dhtPowerPin, OUTPUT);
  turnOnDht();
  //0=gpio0
  //1=gpio1 = tx
  //2 = GPIO2
  //3=GPIO3 = rx
  //  pinMode(pinDHT22, OUTPUT);
  //tx=pin 1
  //rX=3, dont use as output (use ldr)
  //https://www.instructables.com/How-to-use-the-ESP8266-01-pins/
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Input
                             //GP2= dht22
                             //TXS=LED
                             //GP0=LDR1
                             //RXD=LDR2

  WiFi.softAP(ssid, wifiPassword);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);



  TelnetStream.println("Connecting to websocket");
  turnOnInternalLed();
  webSocketClient.begin(host, 80, path);

  webSocketClient.setAuthorization(user, password);

  webSocketClient.onEvent(webSocketEvent);
  // try ever 5000 again if connection has failed
  webSocketClient.setReconnectInterval(5000);
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  webSocketClient.enableHeartbeat(15000, 15000, 2);

  dht.begin();

  listNetworks();
  calculateTempHum();
  calculateDistance();
  timer.setInterval(60 * 1000, repeatMe);
  timer.setInterval(60 * 1000, listNetworks);
  timer.setInterval(10 * 1000, calculateTempHum);
  timer.setInterval(30 * 1000, restartIfNotConnected);
  timer.setInterval(3 * 1000, calculateDistance);
  Serial.println("Setup done");
}


void loop() {
  //#ifdef defined(ESP32_RTOS) && defined(ESP32)
  //#else // If you do not use FreeRTOS, you have to regulary call the handle method.
  ArduinoOTA.handle();
  //#endif
  webSocketClient.loop();
  timer.run();

  // Your code here
  if (shouldReport) {
    shouldReport = false;
    TelnetStream.println("LED state changed. Reporting");
    char powerstate[4];
    if (pumpOn) {
      strcpy(powerstate, "On");
      TelnetStream.println("Reporting pump on");
    } else {
      strcpy(powerstate, "Off");
      TelnetStream.println("Reporting pump off");
    }

    char s[256];
    //sprintf(s, "powerstate=%s,percent=%.0f,deviceId=%s", powerstate, percent, deviceId);
    //sprintf(s, "powerstate=%s,deviceId=%s", powerstate, deviceId);
    sprintf(s, "powerstate=%s,temperature=%0.2f,humidity=%0.2f,brightness=%0.2f,distance=%0.2f,deviceId=%s,ipaddress=%s", powerstate, temperature, humidity, humidity, distance, deviceId, ipaddressString);

    TelnetStream.print("Sending text: ");
    TelnetStream.println(s);
    webSocketClient.sendTXT(s);
  }
}
