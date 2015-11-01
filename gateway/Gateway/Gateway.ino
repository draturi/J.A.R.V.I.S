// Sample RFM69 receiver/gateway sketch, with ACK and optional encryption
// Passes through any wireless received messages to the serial port & responds to ACKs
// It also looks for an onboard FLASH chip, if present
// Library and code by Felix Rusu - felix@lowpowerlab.com
// Get the RFM69 and SPIFlash library at: https://github.com/LowPowerLab/

#include <RFM69.h>
#include <SPI.h>

#define NODEID        1    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
#define FREQUENCY     RF69_433MHZ
//#define FREQUENCY     RF69_868MHZ
//#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
//#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ACK_TIME      30 // max # of ms to wait for an ack
#define SERIAL_BAUD   9600

#ifdef __AVR_ATmega1284P__
  #define LED           15 // Moteino MEGAs have LEDs on D15
  #define FLASH_SS      23 // and FLASH SS on D23
#else
  #define LED           9 // Moteinos have LEDs on D9
  #define FLASH_SS      8 // and FLASH SS on D8
#endif

RFM69 radio;
bool promiscuousMode = false; //set to 'true' to sniff all packets on the same network
char input[8];

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
//#ifdef IS_RFM69HW
//  radio.setHighPower(); //only for RFM69HW!
//#endif
  radio.encrypt(ENCRYPTKEY);
//  radio.promiscuous(promiscuousMode);
  char buff[50];
  Blink(LED, 5000);
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);
//  if (flash.initialize())
//  {
//    Serial.print("SPI Flash Init OK ... UniqueID (MAC): ");
//    flash.readUniqueId();
//    for (byte i=0;i<8;i++)
//    {
//      Serial.print(flash.UNIQUEID[i], HEX);
//      Serial.print(' ');
//    }

    //alternative way to read it:
    //byte* MAC = flash.readUniqueId();
    //for (byte i=0;i<8;i++)
    //{
    //  Serial.print(MAC[i], HEX);
    //  Serial.print(' ');
    //}
//  }
//  else
//    Serial.println("SPI Flash Init FAIL! (is chip present?)");
}

byte ackCount=0;
void loop() {
  //process any serial input
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input == 'r') //d=dump all register values
      radio.readAllRegs();
    if (input == 'E') //E=enable encryption
      radio.encrypt(ENCRYPTKEY);
    if (input == 'e') //e=disable encryption
      radio.encrypt(null);
    if (input == 'p')
    {
      promiscuousMode = !promiscuousMode;
      radio.promiscuous(promiscuousMode);
      Serial.print("Promiscuous mode ");Serial.println(promiscuousMode ? "on" : "off");
    }
    if (input == 'i')
    {
      Serial.print("DeviceID: ");
      word jedecid = NODEID;
      Serial.println(jedecid, HEX);
    }
    if (input == 't')
    {
      byte temperature =  radio.readTemperature(-1); // -1 = user cal factor, adjust for correct ambient
      byte fTemp = 1.8 * temperature + 32; // 9/5=1.8
      Serial.print( "Radio Temp is ");
      Serial.print(temperature);
      Serial.print("C, ");
      Serial.print(fTemp); //converting to F loses some resolution, obvious when C is on edge between 2 values (ie 26C=78F, 27C=80F)
      Serial.println('F');
    }
    /*This is serial protocol stuff*/
    if (input == 'l') {
      if (radio.sendWithRetry(2, "l;", 2, 3))
        Serial.print("ok(l;)!\n");
      else Serial.println("Ya dun goof'd.");
    }
    
    if (input == 'h') {
      if (radio.sendWithRetry(2, "h;", 2, 3))
        Serial.print("ok(h;)!\n");
      else Serial.println("Ya dun goof'd.");
    }
  }

//  if (radio.receiveDone())
//  {
//    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
//    if (promiscuousMode)
//    {
//      /*
//      We want to set this Gateway in permiscuous mode, Read everything on this channel.
//      First pass read everything compare the targetid id to self, if gateway, then we respond.
//      
//      Also we want the gateway to transmit to other nodes.
//      */
//      Serial.print("to [");Serial.print(radio.TARGETID, DEC);Serial.print("] ");
//    }
//    for (byte i = 0; i < radio.DATALEN; i++)
//      Serial.print((char)radio.DATA[i]);
//    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
//    
//    if (radio.ACKRequested())
//    {
//      byte theNodeID = radio.SENDERID;
//      radio.sendACK();
//      Serial.print(" - ACK sent.");
//
//      // When a node requests an ACK, respond to the ACK
//      // and also send a packet requesting an ACK (every 3rd one only)
//      // This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
//      if (ackCount++%3==0)
//      {
//        Serial.print(" Pinging node ");
//        Serial.print(theNodeID);
//        Serial.print(" - ACK...");
//        delay(3); //need this when sending right after reception .. ?
//        
//        else Serial.print("nothing");
//      }
//    }
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

byte readSerialLine(char* input, char endOfLineChar=10, byte maxLength=64, uint16_t timeout=10);
byte readSerialLine(char* input, char endOfLineChar, byte maxLength, uint16_t timeout)
{
  byte inputLen = 0;
  Serial.setTimeout(timeout);
  inputLen = Serial.readBytesUntil(endOfLineChar, input, maxLength);
  input[inputLen]=0;//null-terminate it
  Serial.setTimeout(0);
  //Serial.println();
  return inputLen;
}
