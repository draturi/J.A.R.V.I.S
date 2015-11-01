// Sample RFM69 sender/node sketch, with ACK and optional encryption
// Sends periodic messages of increasing length to gateway (id=1)
// It also looks for an onboard FLASH chip, if present
// Library and code by Felix Rusu - felix@lowpowerlab.com
// Get the RFM69 and SPIFlash library at: https://github.com/LowPowerLab/
#include <RFM69.h>
#include <SPI.h>
//#include <SPIFlash.h>

#define NODEID        2    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
#define GATEWAYID     1
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
#define FREQUENCY   RF69_433MHZ
//#define FREQUENCY   RF69_868MHZ
//#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
//#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ACK_TIME      30 // max # of ms to wait for an ack
#ifdef __AVR_ATmega1284P__
  #define LED           15 // Moteino MEGAs have LEDs on D15
  #define FLASH_SS      23 // and FLASH SS on D23
#else
  #define LED           9 // Moteinos have LEDs on D9
  #define FLASH_SS      8 // and FLASH SS on D8
#endif

#define SERIAL_BAUD   9600
//#define RLYCTRL       LED //we want to see the comms working
#define RLYCTRL       12

//int TRANSMITPERIOD = 300; //transmit a packet to gateway so often (in ms)
//char payload[] = "123 ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char buff[20];
byte sendSize=0;
boolean requestACK = false;
//SPIFlash flash(FLASH_SS, 0xEF30); //EF30 for 4mbit  Windbond chip (W25X40CL)
RFM69 radio;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  pinMode(RLYCTRL, OUTPUT);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
//#ifdef IS_RFM69HW
//  radio.setHighPower(); //uncomment only for RFM69HW!
//#endif
  radio.encrypt(ENCRYPTKEY);
  char buff[50];
  Blink(LED, 5000);
  sprintf(buff, "\nReceiving at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
}

//long lastPeriod = -1;
boolean sendAck = false;
void loop() {
  /*
  This is the reciever node
  */
  if (radio.receiveDone())
  {
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    for (byte i = 0; i < radio.DATALEN; i++){
      Serial.print((char)radio.DATA[i]);
    }
    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
    
    /*Protocol code:
      l; -> turn relay pin low.
      h; -> turn relay pin high.
    */
    if (radio.DATA[1] == ';') {
      // no we know we have seen some protocol data.
      if (radio.DATA[0] == 'l') {
        digitalWrite(RLYCTRL,LOW);
        sendAck = true;
      }else if (radio.DATA[0] == 'h') {
        digitalWrite(RLYCTRL, HIGH);
        sendAck = true;
      }else {
        sendAck = false;
      }
    }

    if (radio.ACKRequested() && sendAck)
    {
      radio.sendACK();
      Serial.print(" - ACK sent");
      sendAck = false;
    }
    
//    Blink(LED,5);
    Serial.println();
  }
//  // Send with retry seems to ask for an ack?
//  radio.sendWithRetry(GATEWAYID, buff, buffLen);
//  delay(TRANSMITPERIOD);
//
//  int currPeriod = millis()/TRANSMITPERIOD;
//  if (currPeriod != lastPeriod)
//  {
//    lastPeriod=currPeriod;
//    Serial.print("Sending[");
//    Serial.print(sendSize);
//    Serial.print("]: ");
//    for(byte i = 0; i < sendSize; i++)
//      Serial.print((char)payload[i]);
//
//    if (radio.sendWithRetry(GATEWAYID, payload, sendSize))
//     Serial.print(" ok!");
//    else Serial.print(" nothing...");
//
//    sendSize = (sendSize + 1) % 31;
//    Serial.println();
//    Blink(LED,3);
//  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
