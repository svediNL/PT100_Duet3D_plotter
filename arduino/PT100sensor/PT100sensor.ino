/*SPI:   10 (SS)   -   11 (MOSI)    -      12 (MISO)    -      13 (SCK). 
           PB2     -      PB3       -         PB4       -       PB5
These pins support SPI communication, 
which, although provided by the underlying hardware, 
is not currently included in the Arduino language.
*/

#include <SPI.h>

const byte CONFIG_WRITE_ADRESS     = 0x80;
const byte CONFIG_READ_ADRESS     = 0x00;
const byte RTD_HIGH_BYTE_ADRESS  = 0x01;
const byte RTD_LOW_BYTE_ADRESS   = 0x02;

const byte RECIEVE_BYTE =  0x55; //0x55 send 0101 0101
const byte CONFIG_BYTE  =  0b11000011;
const word DATA_MASK    =  0b0000000011111111;

//SPI messages
word  READ_HIGH_BYTE = 0x00;
word  READ_LOW_BYTE = 0x00;
word  SET_CONFIG = 0x00;
word  READ_CONFIG = 0x00;

const byte SS_PORT     =  0b00000100;
const byte MOSI_PORT   =  0b00001000;
const byte MISO_PORT   =  0b00010000;
const byte SCK_PORT    =  0b00100000;

const unsigned long clockSpeed = 4000000;

SPISettings settings(clockSpeed, MSBFIRST, SPI_MODE1); 
const int chipSelectPin = 10;
word sensorLowByte=   0;
word sensorHighByte=  0;

word sensorValue = 0;
float mappedSensorValue=0;

byte configRead=      0b00000000;

word readBuffer= 0x00; 
int  zTime, cTime, pTime = 0;
int loopCounter = 0;

int speakerPin= A0;
int speakerFreq= 200;

void setup()
{
  initMessages();
  pinMode(speakerPin, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(settings);
  Serial.begin(9600);
  
  delay(1);
  PORTB= PORTB & ~SS_PORT;    // LOW
  SPI.transfer16(SET_CONFIG);
  PORTB= PORTB | SS_PORT;    // HIGH
  delay(1);
  PORTB= PORTB & ~SS_PORT;    // LOW
  readBuffer = SPI.transfer16(READ_CONFIG);
  PORTB= PORTB | SS_PORT;    // HIGH
  
//  Serial.print("readBuffer: ");
//  Serial.print(readBuffer, BIN);
//  Serial.print("\n\r");
  Serial.println("GO");
  zTime = millis();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
// ~~~~~~~~~~~~~~~~~ L O O P ~~~~~~~~~~~~~~~~ //
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

void loop() 
{
  // put your main code here, to run repeatedly:

  cTime = millis();
  
  if( (cTime-pTime) >= 20)
  {
    // READ RTD HIGH BYTE
    PORTB= PORTB & ~SS_PORT;    // LOW
    sensorHighByte = SPI.transfer16(READ_HIGH_BYTE);
    PORTB= PORTB | SS_PORT;    // HIGH
    sensorHighByte= sensorHighByte & DATA_MASK;
  
    delay(1);
    
    // READ RTD LOW BYTE
    PORTB= PORTB & ~SS_PORT;    // LOW
    sensorLowByte = SPI.transfer16(READ_LOW_BYTE);
    PORTB= PORTB | SS_PORT;    // HIGH
    sensorLowByte= sensorLowByte & DATA_MASK;
    
    
    // combine bytes
    sensorValue= sensorHighByte;
    sensorValue= sensorValue << 8;
    sensorValue= sensorValue | sensorLowByte;
    sensorValue= sensorValue >> 1;
    
    mappedSensorValue =  ((float)sensorValue/32)-256;
   Serial.println(sensorValue);

//    if (17800 > sensorValue || sensorValue > 18200) {
//      Serial.print(" > ERROR DETECTED: ");
//      Serial.print(sensorValue);
//      Serial.print("\n\r");
//    }
    speakerFreq = (float) (mappedSensorValue*10) + 200;
    tone(speakerPin, speakerFreq);

    pTime= millis();
    loopCounter++;
  }
  
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
// ~~~~~~~~~~~~~ F U N C T I O N S ~~~~~~~~~~~~~ //
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

void initMessages(){
  // initiate read high byte message
  READ_HIGH_BYTE = READ_HIGH_BYTE | RTD_HIGH_BYTE_ADRESS;
  READ_HIGH_BYTE = READ_HIGH_BYTE << 8;
  READ_HIGH_BYTE = READ_HIGH_BYTE | RECIEVE_BYTE;
  
  // initiate read high byte message
  READ_LOW_BYTE = READ_LOW_BYTE | RTD_LOW_BYTE_ADRESS;
  READ_LOW_BYTE = READ_LOW_BYTE << 8;
  READ_LOW_BYTE = READ_LOW_BYTE | RECIEVE_BYTE;
  
  SET_CONFIG = SET_CONFIG | CONFIG_WRITE_ADRESS;
  SET_CONFIG = SET_CONFIG << 8;
  SET_CONFIG = SET_CONFIG | CONFIG_BYTE;
  
  READ_CONFIG = READ_CONFIG | CONFIG_READ_ADRESS;
  READ_CONFIG = READ_CONFIG << 8;
  READ_CONFIG = READ_CONFIG | RECIEVE_BYTE;

//  Serial.print("READ_HIGH_BYTE: ");
//  Serial.print(READ_HIGH_BYTE, BIN);
//  Serial.print("\n\r");
//  
//  Serial.print("READ_LOW_BYTE: ");
//  Serial.print(READ_LOW_BYTE, BIN);
//  Serial.print("\n\r");
//  
//  Serial.print("SET_CONFIG: ");
//  Serial.print(SET_CONFIG, BIN);
//  Serial.print("\n\r");
//  
//  Serial.print("READ_CONFIG: ");
//  Serial.print(READ_CONFIG, BIN);
//  Serial.print("\n\r");
};


void spiRead(byte adress){
  
  unsigned int readBuffer= 0;  // unsigned int, 16bits
  readBuffer = readBuffer | adress;
  readBuffer = readBuffer <<8;
  delay(1);
  
  PORTB= PORTB & ~SS_PORT;    // LOW
  SPI.transfer16(readBuffer);
  PORTB= PORTB | SS_PORT;    // HIGH
  
};

void spiWrite(unsigned int writeBuff){
  
  PORTB= PORTB & ~SS_PORT;    // LOW
  SPI.transfer16(writeBuff);
  PORTB= PORTB | SS_PORT;    // HIGH

};


