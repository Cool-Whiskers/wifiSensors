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
int run_number = X; //TO DO: Add self-incrementation of run number

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
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:

  // Get temperature from thermistor and add to query string
  char * querystring;
  char * querystring2;
  char * querystring3;
  querystring = "sensor=test&run=";
  querystring2 = "&data_type=temp&data=";
  querystring3 = "&battery_volt=";
  char temp_data[10];
  long thermistorTemp = 1e6 * getTemp();
  ltoa(thermistorTemp, temp_data, 10);
  char run_number_string[10];
  ltoa(run_number, run_number_string, 10);
  char bat_volt_data[10];
  long batteryVolt = 1e6 * getBat();
  ltoa(batteryVolt, bat_volt_data, 10);
  Serial.print("Temperature (C) = "); Serial.println(thermistorTemp / 1e6);
  Serial.print("Battery Votlage (V) = "); Serial.println(batteryVolt / 1e6);

  char* str1 = concatenate(querystring, run_number_string);
  char* str2 = concatenate(querystring2, temp_data);
  char* str3 = concatenate(querystring3, bat_volt_data);
  char* post_request_temp = concatenate(str1, str2);
  char* post_request = concatenate(post_request_temp, str3);
  int len_post_request = strlen(querystring) + 
                         strlen(querystring2) + 
                         strlen(querystring3) +
                         strlen(run_number_string) + 
                         strlen(temp_data) +
                         strlen(bat_volt_data);
  //Serial.println(len_post_request);
  Serial.println(post_request);

  float battery_voltage = getBat();

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 8000)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    //client.println(GETRequest_0);
    client.print("POST /experiments/");
    client.println(" HTTP/1.1");
    client.println("Host: 192.168.86.46");
    client.println("User-Agent: Arduino/1.0");
    //client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(len_post_request);
    client.println();
    client.print(post_request);
    Serial.println("Sent Data Successfuly!");
    Serial.println();
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
