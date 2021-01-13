/*
  Web client

 This sketch connects to a website (http://www.google.com)
 using a WiFi 101 Shield.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 Circuit:
 * WiFi 101 Shield attached

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 */


#include <SPI.h>
#include <WiFi101.h>
#include <string.h>
#include <ArduinoJson.h>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server = IPAddress(X,X,X,X); //Replace X's with IPv4 Address

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

#define LED 13
#define VBATPIN A7

// Thermistor
#define THERMISTORPIN A5 // Make sure this is right pin
#define THERMISTORNOMINAL 614.8 //TO DO: Calibrate
#define TEMPERATURENOMINAL 22 //TO DO: Calibrate
#define NUMSAMPLES 5
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 100 //TO DO: Calibrate
int samples[NUMSAMPLES];

//POST Request
char directory[] = "/experiments/";

//Experiment run number
int run_number = 11; //TO DO: Add self-incrementation of run number

void setup() {
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);
  
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB port only
//  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi 101 Shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to WiFi");
  Blink(LED, 40, 3);
  printWiFiStatus();

  // Enable automatic sleep
  WiFi.lowPowerMode();
}

char buf[128];
const int capacity = JSON_OBJECT_SIZE(5);
StaticJsonDocument<capacity> doc;

void loop() {
  /* JSON Serialization and Allocation*/
  doc["sensor"] = "JSON";
  doc["run"] = run_number;
  doc["data_type"] = "temp";
  doc["data"] = getTemp();
  doc["battery_volt"] = getBat();
  serializeJson(doc, buf);
  //Serial.println(sizeof(doc));

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 8000)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    //client.println(GETRequest_0);
    client.print("POST /experiments/");
    client.println(" HTTP/1.1");
    client.println("Host: X.X.X.X");
    client.println("User-Agent: Arduino/1.0");
    //client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(sizeof(buf));
    client.println();
    client.print(buf);
    Serial.println("Sent Data Successfuly!");
    Serial.println();
    //Blink(LED, 40, 1);
  }

  free(post_request);
  
  while (client.available()) {
    char c = client.read();
    //Serial.write(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();

    // do nothing forevermore:
    //while (true);
  }
  
  delay(1000); //TO DO: Adjust to vary transmission rate
}

void Blink(byte PIN, byte DELAY_MS, byte loops) {
  for (byte i=0; i<loops; i++)  {
    digitalWrite(PIN,HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN,LOW);
    delay(DELAY_MS);
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi 101 Shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
char * concatenate(char * a, char * b) {
  int len_a = strlen(a);
  int len_b = strlen(b);
  char * result = (char *) malloc(len_a + len_b + 1);
  strcpy(result, a);
  strcpy(result + len_a, b);
  return result;
}

float getBat() {
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;
  measuredvbat *= 3.3;
  measuredvbat /= 1024;
  // Serial.print("VBat: "); Serial.println(measuredvbat);
  return measuredvbat;
}

float getTemp() {
  uint8_t i;
  float average;

  for ( i =0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(THERMISTORPIN);
    delay(10);
  }

  average = 0;
  for (i = 0; i < NUMSAMPLES; i++) {
    average += samples[i];
  }
  average /= NUMSAMPLES;

  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;

  /*Serial.print("Thermistor resistance = ");
  Serial.println(average);*/

  float thermistorTemp;
  thermistorTemp = average / THERMISTORNOMINAL;
  thermistorTemp = log(thermistorTemp);
  thermistorTemp /= BCOEFFICIENT;
  thermistorTemp += 1.0 / (TEMPERATURENOMINAL + 273.15);
  thermistorTemp = 1.0 / thermistorTemp;
  thermistorTemp -= 273.15;

  return thermistorTemp;
}
