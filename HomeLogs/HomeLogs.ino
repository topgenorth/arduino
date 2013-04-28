 /**
  * HomeLogs.ino
  * 
  * This sketch will temperature values using a TMP36 analog sensor,
  * and upload the analog value to COSM.com. URL to the feed:
  * https://cosm.com/feeds/128080
  *
  * Built off the sample project provided by COSM.com, using code written
  * by Adrian McEwen with modifications by Sam Mulube.
  *
  * April 28, 2013
  */

#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Cosm.h>

#define API_KEY "" // COSM API Key

const unsigned long IP_DELAY              = 5000;     // The number of seconds to wait while trying to get IP.
const unsigned long CONNECTION_INTERVAL   = 60000;    // delay between connecting to Cosm in milliseconds
const unsigned long FEED_ID               = 128080;   // Cosm feed ID
const unsigned int TEMP_SENSOR_PIN        = 3;        // The temperature sensor pin is A0
const unsigned int CS_PIN                 = 4;        // Required by the Ethernet shield
const unsigned int SDCARD_PIN             = 10;       // Required by the Ethernet shield
const unsigned int BAUD_RATE              = 9600;
const float TEMPERATURE_ADJUSTMENT        = -2.7;     // My TMP36 seems to run about 3.2C hot?
const float SUPPLY_VOLTAGE                = 5000;     // milliVolts

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address for your Ethernet shield
unsigned long lastConnectionTime = 0;                // last time we connected to Cosm

// Define the string for our datastream ID
CosmDatastream datastreams[] = {
  CosmDatastream("tmp36_sensor_reading", strlen("tmp36_sensor_reading"), DATASTREAM_FLOAT),
  CosmDatastream("tmp36_temperature",    strlen("tmp36_temperature"),    DATASTREAM_FLOAT),
};

// Wrap the datastream into a feed
CosmFeed feed(FEED_ID, datastreams, 2 /* number of datastreams */);

EthernetClient client;
CosmClient cosmclient(client);

void setup() {
  Serial.begin(BAUD_RATE);

  Serial.println("***************************");
  while (Ethernet.begin(mac) != 1) {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(IP_DELAY);
  }
  print_ip_address(Ethernet.localIP());

  lastConnectionTime = CONNECTION_INTERVAL + 1; // this will trigger a read when the application first starts up.

  Serial.println("***************************");
}

void loop() {
  // main program loop
  if (millis() - lastConnectionTime > CONNECTION_INTERVAL) {
    Serial.println();
    read_temperature_value();
    lastConnectionTime = millis();
  }
}

void read_temperature_value() {
    const int sensorValue = analogRead(TEMP_SENSOR_PIN);
    const float milliVolts = sensorValue * SUPPLY_VOLTAGE / 1024.0;
    const float temperature = ((milliVolts - 500) / 10) + TEMPERATURE_ADJUSTMENT;

    log_temperature(sensorValue, milliVolts, temperature);
    sendDataToCosm(sensorValue, temperature);
}

void sendDataToCosm(int sensorValue, float temperature) {
  datastreams[0].setFloat(sensorValue);
  datastreams[1].setFloat(temperature);

  Serial.print("Uploading to Cosm...");
  int ret = cosmclient.put(feed, API_KEY);
  Serial.print("PUT return code: ");
  Serial.println(ret);
}

void print_ip_address(IPAddress ip) {
  const unsigned int OCTETS = 4;
  Serial.print("Network initialized, IP address:");
  for (unsigned int i = 0; i < OCTETS; i++) {
    Serial.print(ip[i]);
    if (i != OCTETS - 1) {
      Serial.print(".");
    }
  }

  Serial.println();
}

void log_temperature(int sensorValue, float milliVolts, float temperature) {
  Serial.print("TMP36 reading: ");
  Serial.print(sensorValue);
  Serial.print(", milliVolts: ");
  Serial.print(milliVolts);
  Serial.print("mV, ");
  Serial.print(temperature);
  Serial.println(" Centigrade");
}