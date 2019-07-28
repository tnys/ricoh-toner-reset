// This code updates Ricoh Toner chip for Ricoh Aficio SP C250SF, SP C250DN, C250e, etc...
// I2C_ADDRESS states toner color.
// 83 is Chip K - black
// 82 is Chip C - cyan
// 81 is Chip M - magenta
// 80 is Chip Y - yellow

#include <Wire.h>

// blank data for K, C, M & Y chip. 128 array of data. I got it from data dump of a replacement chip. 
byte KChipData[]={168,0,1,3,18,1,1,255,100,0,52,48,55,53,52,51,20,9,65,66,22,0,22,38,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,100,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0};
byte CChipData[]={168,0,1,3,14,2,1,255,100,0,49,49,49,53,52,54,20,2,65,66,23,0,7,1,255,255,255,255,255,255,255,255,88,48,56,54,80,52,48,49,50,48,56,0,68,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,1,85,0,18,0,40,0,5,184,230,50,0,128,0,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0};
byte MChipData[]={168,0,1,3,14,3,1,255,100,0,49,49,49,53,52,54,20,2,65,66,24,0,7,16,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,100,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0};
byte YChipData[]={168,0,1,3,14,4,1,255,100,0,49,49,49,53,52,55,20,2,65,66,25,0,3,7,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,100,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0};

void setup() {
  // Start Wire and Serial bus
  Wire.begin();
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

int isToner() {
  byte i2caddress = getI2CAddress(); // try to whether there's an I2C component connected
  if (i2caddress == 255)
    return -1;

  // found i2c component
  Serial.print("I2C address: ");
  Serial.println(i2caddress);
    
  // start read chip
  byte readData[128];
  for(byte i=0;i<128;i++){
      readData[i] = i2cread(i2caddress, i);
      Serial.print(readData[i]);
      Serial.print(",");
  } 
  Serial.println(")");

  if (memcmp(readData, KChipData, 2 * sizeof(byte)) == 0)
    return i2caddress;
  else if (memcmp(readData, CChipData, 2 * sizeof(byte)) == 0)
    return i2caddress;
  else if (memcmp(readData, MChipData, 2 * sizeof(byte)) == 0)
    return i2caddress;
  else if (memcmp(readData, YChipData, 2 * sizeof(byte)) == 0)
    return i2caddress;
  else return -1;
}

void flashledfourtimes() {
  flashled();  
  flashled();  
  flashled();  
  flashled();  
}

void flashled() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);  
  digitalWrite(LED_BUILTIN, LOW);
  delay(300);  
}

void loop() {
  delay(2000);
  flashled();
  Serial.println("Looking for toner..");
  byte i2caddress = isToner();
  if (i2caddress != 255) // valid toner
  {
    flashledfourtimes();
    byte writeData[128];
    switch(i2caddress) {
      case 83: 
        Serial.println("K Toner found!");
        memcpy(writeData, KChipData, 128 * sizeof(byte));
        break;
      case 82: 
        Serial.println("C Toner found!");
        memcpy(writeData, CChipData, 128 * sizeof(byte));
        break;
      case 81: 
        Serial.println("M Toner found!");
        memcpy(writeData, MChipData, 128 * sizeof(byte));
        break;
      case 80: 
        Serial.println("Y Toner found!");
        memcpy(writeData, YChipData, 128 * sizeof(byte));
        break;
    }
  
    Serial.println("Writing data!");
    for(byte i = 0; i < 128; i++){
      i2cwrite(i2caddress, (byte)i, (byte)writeData[i]);
    }
    flashledfourtimes();
    Serial.println("Writing done. Sleeping 30 seconds.");
    delay(30000);
  }
}

void i2cwrite(int i2caddress, byte address, byte data) {
  Wire.beginTransmission(i2caddress);
  Wire.write((byte)address);
  Wire.write((byte)data);
  Wire.endTransmission();
  delay(20);
}

byte i2cread(int i2caddress, byte address) {
  byte rData = 0;
  Wire.beginTransmission(i2caddress);
  Wire.write((byte)address);
  Wire.endTransmission();

  Wire.requestFrom(i2caddress,1);
  while (Wire.available()){
    rData = Wire.read();
    return rData;
  }
}

byte getI2CAddress() {
    for(int i=0;i<128;i++){
      Wire.requestFrom(i,1);  //request first data byte
      while(Wire.available()){
        byte c = Wire.read();
        return i;    //if data exist, print it out. That way you can identify which address.
      }
      delay(5);
  }
  return -1;
}
