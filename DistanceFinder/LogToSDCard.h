
#ifndef LOGTOSDCARD_H
#define LOG_TO_SDCARD

const char LOGFILE_NAME[]                 = "SUMPLOG.CSV";

// Constants for NTP
const int NTP_PACKET_SIZE                 = 48;       // NTP time stamp is in the first 48 bytes of the message
const unsigned int NTP_REQUEST_PORT       = 123;      // All NTP requests are on port 123.
const unsigned int LOCAL_PORT             = 8888;     // The local port to listen for UDP packets.

File logFile;
IPAddress timeServer(132, 163, 4, 101);               // time-a.timefreq.bldrdoc.gov NTP server
// IPAddress timeServer(132, 163, 4, 102);               // time-b.timefreq.bldrdoc.gov NTP server
// IPAddress timeServer(132, 163, 4, 103);               // time-c.timefreq.bldrdoc.gov NTP server
byte packetBuffer[NTP_PACKET_SIZE];                   //buffer to hold incoming and outgoing packets 
EthernetUDP Udp;                                      // A UDP instance to let us send and receive packets over UDP


// send an NTP request to the time server at the given address 
unsigned long sendNTPpacket(IPAddress& address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE); 
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49; 
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now you can send a packet requesting a timestamp:         
  Udp.beginPacket(address, NTP_REQUEST_PORT);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();  
}

void get_time_from_NTP() {
  unsigned long epoch = get_epoch_time();

  String utcTime = String("The UTC time is ");
  utcTime += (epoch  % 86400L) / 3600;  // print the hour
  utcTime += ":";

  if ( ((epoch % 3600) / 60) < 10 ) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    utcTime += '0';
  }
  utcTime += (epoch  % 3600) / 60;

  utcTime += ':';
  if ( (epoch % 60) < 10 ) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    utcTime += '0';
  }
  utcTime += epoch %60;

  sensor_values.epoch_time = epoch;
}

// Returns the number of seconds since January 1, 1970
unsigned long get_epoch_time() {
  unsigned long epoch = 0;
  if (ethernetInitialize == 0) {
    return epoch;
  }

  sendNTPpacket(timeServer);  // send an NTP packeet to a time server;
  delay(1000);                // wait to see if a reply is available;
  if ( Udp.parsePacket() ) {  
    // We've received a packet, read the data from it
    Udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);  

    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;  

    // now convert NTP time into everyday time:
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;     
    // subtract seventy years:
    epoch = secsSince1900 - seventyYears;  
  }
  
  return epoch;
}

#endif /* !LOGTOSDCARD_H */