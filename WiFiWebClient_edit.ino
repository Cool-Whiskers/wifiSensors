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

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server = IPAddress(192,168,86,46);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

#define IDENTIFIER 1
#define UNIT "Temp"
#define LED 13

// Thermistor
#define THERMISTORPIN A0
#define THERMISTORNOMINAL 614.8 //TO DO: Adjust
#define TEMPERATURENOMINAL 22 //TO DO: Adjust
#define NUMSAMPLES 5
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 1000 //TO DO: Adjust
int samples[NUMSAMPLES];

// Query string
char GETRequest[100];
char GETRequest_0[100] = "POST /experiments/?sensor=";
char GETRequest_1[] = "&data_type=";
char GETRequest_2[] = "&data=";
char ID[3]; 
char Unit[10] = UNIT;
char data[20];

void setup() {
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

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
  printWiFiStatus();
  
  // Construct the query string using defined variables
  ltoa(IDENTIFIER, ID, 10);
  strcat(GETRequest_0, ID);
  strcat(GETRequest_0, GETRequest_1);
  strcat(GETRequest_0, Unit);
  strcat(GETRequest_0, GETRequest_2);

  memcpy(GETRequest, GETRequest_0, sizeof(GETRequest_0) / sizeof(GETRequest_0[0]));
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:

  // Get temperature from thermistor and add to query string
  long thermistorTemp = 1e6 * getTemp();
  ltoa(thermistorTemp, data, 10);
  strcat(GETRequest_0, data);

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 8000)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println(GETRequest_0);
    //client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
    Serial.println("Sent Data Successfuly!");
    Blink(LED, 40, 1);
  }
  
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

  //Clear and reset GETRequest query string
  memset(GETRequest_0, NULL, sizeof(GETRequest_0) / sizeof(GETRequest_0[0]));
  memcpy(GETRequest_0, GETRequest, sizeof(GETRequest_0) / sizeof(GETRequest_0[0]));
  
  delay(5000); //TO DO: Adjust to vary transmission rate
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
