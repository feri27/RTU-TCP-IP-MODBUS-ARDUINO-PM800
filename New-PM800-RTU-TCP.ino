//library include
#include <SimpleModbusMaster.h>
#include <SPI.h>
#include <Ethernet.h>
#include "Mudbus.h"
Mudbus Mb;


//////////////////// Port information ///////////////////
#define baud 19200 //baudrate for modbus
#define timeout 100 //time out
#define polling 10 // the scan rate
#define retry_count 10 //count of connection

// used to toggle the receive/transmit pin on the driver
#define TxEnablePin 3
// The total amount of available memory on the master to store data
#define TOTAL_NO_OF_REGISTERS 13

enum
{
  PACKET1,
  PACKET2,
  PACKET3,
  PACKET4,
  TOTAL_NO_OF_PACKETS // last entry of packet
};

// packet array
Packet packets[TOTAL_NO_OF_PACKETS];

// Masters register array
unsigned int regs[TOTAL_NO_OF_REGISTERS];

void setup()
{
  
  //RTU TCP setup
  uint8_t mac[]     = { 0x91, 0xA1, 0xDA, 0x00, 0x52, 0x05 };
  uint8_t ip[]      = { 192, 168, 1, 2 };
  uint8_t gateway[] = { 192, 168, 1, 1 };
  uint8_t subnet[]  = { 255, 255, 255, 0 };
  Ethernet.begin(mac, ip, gateway, subnet);
  
  delay(3000);
  Serial.begin(19200);

  //pin mode setup
  pinMode(2, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  Mb.R[21] = 1987; 

  
  // Initialize each packet for regiter and id device
  modbus_construct(&packets[PACKET1], 1, READ_INPUT_REGISTERS, 1123, 1, 10);//register voltage R
  modbus_construct(&packets[PACKET2], 1, READ_INPUT_REGISTERS, 1124, 1, 11);//register voltage S
  modbus_construct(&packets[PACKET4], 1, READ_INPUT_REGISTERS, 1125, 1, 12);//register voltage T
 
  // Initialize the Modbus
  modbus_configure(&Serial1, baud, SERIAL_8N1, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS, regs);

  
 
}

void loop()
{
  modbus_update();
  unsigned long temp;
  
  int Voltage, Voltage1, Voltage2;
  Voltage = regs[10];
  Voltage1 = regs[11];
  Voltage2 = regs[12];

  //serial rint data from register 
  Serial.print("Voltage L-N :");
  Serial.println(Voltage);
  Serial.println(Voltage1);
  Serial.println(Voltage2);

  Mb.Run();

  //make register for other modbus device
  Mb.R[0] = (Voltage); // voltage R
  Mb.R[1] = (Voltage1); // voltage S
  Mb.R[2] = (Voltage2); // voltage T
  
   //Digital inputs
  Mb.C[0] = digitalRead(2);
  Mb.C[1] = digitalRead(5);
  Mb.C[2] = digitalRead(6);
   //Digital outputs
  digitalWrite(7, Mb.C[4]);
  digitalWrite(8, Mb.C[5]);
  digitalWrite(9, Mb.C[6]);
  digitalWrite(A1, Mb.C[7]);
  digitalWrite(A0, Mb.C[8]);
  
 //serial output for debuging comunication
 delay(1000);
 Serial.print("requests: ");
 Serial.println(packets[PACKET1].requests);
 Serial.print("successful_requests: ");
 Serial.println(packets[PACKET1].successful_requests);
 Serial.print("failed_requests: ");
 Serial.println(packets[PACKET1].failed_requests);
 Serial.print("exception_errors: ");
 Serial.println(packets[PACKET1].exception_errors);
 Serial.print("connection: ");
 Serial.println(packets[PACKET1].connection);
 
}
