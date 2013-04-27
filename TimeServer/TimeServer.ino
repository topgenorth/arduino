/************************************************************************
 * 
 * Just a simple little sketch that will obtain an IP address via 
 * DHCP and then contact the time server at time.nist.gov
 * 
 * April 27, 2013
 *
 ************************************************************************/
#include <SPI.h>
#include <Ethernet.h>

const unsigned int BAUD_RATE = 9600;
const unsigned int DAYTIME_PORT = 13;
const char* time_server = "time.nist.gov";
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // max address for ethernet shield
EthernetClient client;

void setup() {
	Serial.begin(BAUD_RATE);
	if (Ethernet.begin(mac) == 0) {
		for (;;) {
			Serial.println("Could not obtain IP address.");
			delay(1000);
		}
	} else {
		print_ip_address(Ethernet.localIP());
	}
}

void loop() {
	delay(5000);
	Serial.println();
	Serial.print("Connecting...");
	if (client.connect(time_server, DAYTIME_PORT) <= 0) {
		Serial.println("connection failed.");
	} else {
		Serial.println("connected.");
		delay(300);
	}

	while (client.available()) {
		char c = client.read();
		Serial.print(c);
	}

	Serial.println("Disconnecting.");
	client.stop();
	Serial.println();
}

void print_ip_address(IPAddress ip) {
	const unsigned int OCTETS = 4;
	Serial.print("My IP address:");
	for (unsigned int i = 0; i < OCTETS; i++) {
		Serial.print(ip[i]);
		if (i != OCTETS - 1) {
			Serial.print(".");
		}
	}

	Serial.println();
}
