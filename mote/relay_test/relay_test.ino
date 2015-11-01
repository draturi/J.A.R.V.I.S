/*
This is a simple protocol implementation that will toggle Pin 13 on the mote based on the 
recieved message. Following is an example of the protocol:
    h; <- this will toggle the pin high
    l; <- this will toggle the pin low
    
*/
#define RELAYCTRL 12
#define LEDPIN     9

String inputString = "";
bool msgRcvd = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  inputString.reserve(64);
  pinMode(LEDPIN, OUTPUT);      
  pinMode(RELAYCTRL, OUTPUT);
  // Setup complete
  digitalWrite(LEDPIN, HIGH);
  delay(2000);
  digitalWrite(LEDPIN, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (msgRcvd) {
    char state_req = inputString[0]; // this is for simplicity e.g h; <- this is a message
    switch (state_req) {
      case 'h':
//        setPin(RELAYCTRL, true);
        digitalWrite(RELAYCTRL, HIGH);
        digitalWrite(LEDPIN, HIGH);
        Serial.println("ACK(h)!!");
        inputString = "";
        break;
      case 'l':
//        setPin(RELAYCTRL, false);
        digitalWrite(RELAYCTRL, LOW);
        digitalWrite(LEDPIN, LOW);
        Serial.println("ACK(l)!!");
        inputString = "";
        break;
      default:
        Serial.println("Invalid Message!");
        inputString = "";
        break;
    }
    msgRcvd = false;   
  }
}

// this is not working right now.
void setPin(int pin, bool state) {
  digitalWrite(pin, state ? HIGH : LOW);
}

void serialEvent() {
//  Serial.println("got something.");
  while(Serial.available()){
    // get the new byte
    char rcvd = (char)Serial.read();
    // add it to the string
    inputString += rcvd;
//    Serial.write("DEBUG:");
//    Serial.write((char *)inputString);
    // our protocol is ';' terminated. If we see a semicolon
    // set a msgRcvd flag, this will let the main loop know
    // that it needs to do something with this message.
    if (rcvd == ';') {
      msgRcvd = true;
    }
  }
}
