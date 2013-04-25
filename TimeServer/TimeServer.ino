/*
 *
 * DS1307 RTC library : https://code.google.com/p/ds1307new/
 */

#include <SPI.h>
#include <Ethernet.h>
#include <DS1307new.h>

const unsigned int BAUD_RATE = 9600;
const unsigned int DAYTIME_PORT = 13;

IPAddress my_ip(192,168,1,112);
IPAddress time_server(192,43,244,18); // time.nist.gov
EthernetClient client;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

void setup() {
	Serial.begin(BAUD_RATE);
	Ethernet.begin(mac, my_ip);
}

void loop() {
	delay(1000);
	// show_clock();
	connect_to_time_server();
}

void connect_to_time_server() {
	Serial.print("Connecting...");
	if (client.connect(time_server, DAYTIME_PORT) <= 0) {
		Serial.println("connection failed.");
	} else {
		Serial.println("connected.");
		delay(300);

		while(client.available() ) {
			char c = client.read();
			Serial.print(c);
		}

		Serial.println("disconnecting.");
		client.stop();
	}
}

/*
void show_clock() {
	Serial.print("Today's date is ");
	RTC.getTime();

	switch (RTC.dow) {
		case 1:
			Serial.print("Monday");
			break;
		case 2:
			Serial.print("Tuesday");
			break;
		case 3:
			Serial.print("Wednesday");
			break;
		case 4:
			Serial.print("Thursday");
			break;
		case 5:
			Serial.print("Friday");
			break;
		case 6:
			Serial.print("Saturday");
			break;
		case 7:
			Serial.print("Sunday");
			break;
	}

	Serial.print(", ");
	switch (RTC.month) {
		case 1:
			Serial.print("January");
			break;
		case 2:
			Serial.print("February");
			break;
		case 3:
			Serial.print("March");
			break;
		case 4:
			Serial.print("April");
			break;
		case 5:
			Serial.print("May");
			break;
		case 6:
			Serial.print("June");
			break;
		case 7:
			Serial.print("July");
			break;
		case 8:
			Serial.print("August");
			break;
		case 9:
			Serial.print("September");
			break;
		case 10:
			Serial.print("October");
			break;
		case 11:
			Serial.print("November");
			break;
		case 12:
			Serial.print("December");
			break;
	}

	Serial.print(" ");
	Serial.print(RTC.day, DEC);
	Serial.print(", ");
	Serial.println(RTC.year, DEC);

	Serial.print("The Current Time is ");
	if (RTC.hour < 10) {
		Serial.print("0");
		Serial.print(RTC.hour, DEC);
	} else {
		Serial.print(RTC.hour, DEC);
	}
	Serial.print(":");
	if (RTC.minute < 10) {
		Serial.print("0");
		Serial.print(RTC.minute, DEC);
	} else {
		Serial.print(RTC.minue, DEC);
	}
	Serial.println();
}
*/