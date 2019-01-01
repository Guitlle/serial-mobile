/*
  
  Created 22 Dic 2018
  by Guillermo Ambrosio
  Modified 24 Dic 2018
  by G Ambrosio

*/

#include <Wire.h>
#include <SPI.h>

int hexDigit (char input) {
  if (input >= '0' && input <= '9') {
    return (int) (input - '0');
  }
  if (input >= 'A' && input <= 'F') {
    return (int) (input - 'A') + 10;
  }
  return -1;
}

byte strHexToInt8 (char low, char high) {
  byte retval = 0;
  retval = (hexDigit(low) << 4);
  retval |= hexDigit(high);
  return retval;
}

int strHexToInt16 (char * bufferInput) {
  int retval = 0;
  retval = strHexToInt8(bufferInput[2], bufferInput[3]) << 8;
  retval |= strHexToInt8(bufferInput[0], bufferInput[1]);
  return retval;
}

void i2c_write8(byte addr, byte reg, byte value) {
  Wire.beginTransmission(addr);
  Wire.write(reg); // sends register address to read from
  Wire.write(value);  // write data
  Wire.endTransmission();
}

void setup() {
  Wire.begin(); 
  Serial.begin(9600);
}

int reg_A = 0, reg_B = 0, reg_C = 0, reg_D = 0, reg_E = 0, reg_F = 0;
byte temp1, temp2, temp3;
boolean handleCaseInit = false;
char buffer[100];
byte bufferSize, i;
int * regPointer;

void loop() {
  // Read commands from serial coms:
  bufferSize = Serial.readBytesUntil('_', buffer, 100);
  // Execute commands if any:
  if (bufferSize >0) {
    Serial.print("Read N=");Serial.print(bufferSize);Serial.print('\n');
    i = 0;
    while (i <= bufferSize) {                
      handleCaseInit = false;
      Serial.print("]]] "); Serial.println(buffer[i]);
      // API
      switch ((char)buffer[i]) {
        case '.':
          Serial.println("Inicio . ");
          break;
        case 'a':
        // set register a
          regPointer = &reg_A;
        case 'b':
        // set register b
          if (regPointer != NULL)
            regPointer = &reg_B;
        case 'c':
        // set register c
          if (regPointer != NULL)
            regPointer = &reg_C;
        case 'd':
        // set register d
          if (regPointer != NULL)
            regPointer = &reg_D;
        case 'e':
        // set register e
          if (regPointer != NULL)
            regPointer = &reg_E;
        case 'f':
        // set register f
          if (regPointer != NULL)
            regPointer = &reg_F;
          // read 4 bytes, jump the counter.
          (*regPointer) = strHexToInt16(buffer+1);
          i += 4; 
          Serial.println("SetReg ");
          Serial.print(buffer[i]);
          Serial.print("=");
          Serial.print((*regPointer), HEX);
          break;
        case 'g':
        // i2c read address A, register B
          temp1 = reg_A; // the address
          temp2 = (byte) reg_B;
          Wire.beginTransmission(temp1);
          Wire.write(temp2);
          Wire.endTransmission();
          handleCaseInit = true;
        case 'h':
        // i2c read address A, register C
          if (!handleCaseInit) {
            temp1 = reg_A; // the address
            temp2 = (byte) reg_C;
            Wire.beginTransmission(temp1);
            Wire.write(temp2);
            Wire.endTransmission();
            handleCaseInit = true;
          }
        case 'i':
        // i2c read, params address and register
          if (!handleCaseInit) {
            // this will store the address, read 2 bytes
            temp1 = strHexToInt8(buffer[i+1], buffer[i+2]);
            i += 2;
            temp2 = strHexToInt8(buffer[i+1], buffer[i+2]);
            i += 2;
            Wire.beginTransmission(temp1);
            Wire.write(temp2);
            Wire.endTransmission(false);
            handleCaseInit = true;
          }
          Wire.requestFrom(temp1, (uint8_t) 1);
          temp3 = (byte) Wire.read(); 
          Serial.print("I2CRead8: ");Serial.print(temp1, HEX);Serial.print("->");Serial.print(temp2, HEX);Serial.print("=");Serial.print(temp3, HEX);
          break;
        case 'j':
        // i2c write address A, register B, value D
          temp1 = reg_A;
          temp2 = reg_B;
          temp3 = reg_D;
          handleCaseInit = true;
        case 'k':
        // i2c write address A, register B, value E
          if (!handleCaseInit) {
            temp1 = reg_A;
            temp2 = reg_B;
            temp3 = reg_E;
            handleCaseInit = true;
          }
        case 'l':
        // i2c write address A, register C, value D
          if (!handleCaseInit) {
            temp1 = reg_A;
            temp2 = reg_C;
            temp3 = reg_D;
            handleCaseInit = true;
          }
          break;
        case 'm':
        // i2c write address A, register C, value E
          if (!handleCaseInit) {
            temp1 = reg_A;
            temp2 = reg_C;
            temp3 = reg_E;
            handleCaseInit = true;
          }
          break;
        case 'n':
        // i2c write with params address, register, value
          if (!handleCaseInit) {
            temp1 = strHexToInt8(buffer[i+1], buffer[i+2]);
            temp2 = strHexToInt8(buffer[i+3], buffer[i+4]);
            temp3 = strHexToInt8(buffer[i+5], buffer[i+6]);
            i += 6;
            handleCaseInit = true;
          }
          i2c_write8(temp1, temp2, temp3);
          Serial.print("I2CWrite8: ");Serial.print(temp1, HEX);Serial.print("->");Serial.print(temp2, HEX);Serial.print("=");Serial.print(temp3, HEX);
          break;
        case 'O':
        // Set digital output on params pin, mode
          pinMode(strHexToInt8(buffer[i+1], buffer[i+2]), (buffer[i+3] == '1')? OUTPUT : ( (buffer[i+3] == '2')? INPUT : INPUT_PULLUP )  );  
          i += 3;
          Serial.print("pinMode");
          break;
        case 'o':
        // Digital write on params pin, value
          digitalWrite(strHexToInt8(buffer[i+1], buffer[i+2]), (buffer[i+3] == '1')? HIGH : LOW );  
          i += 3;
          Serial.print("digitalWrite");
          break;
        case 'p':
        // Digital read on params, pin
          Serial.println("digitalRead: ");
          Serial.print(digitalRead(strHexToInt8(buffer[i+1], buffer[i+2])) == HIGH ? '1' :  '0');
          i += 2;
          break;
        case 'q':
        // Analog write on params, pin, value
          analogWrite(strHexToInt8(buffer[i+1], buffer[i+2]), strHexToInt8(buffer[i+3], buffer[i+4]));  
          i += 4;
          break;
        case 'Q':
        // Analog read on params, pin
          Serial.println("analogRead:");
          Serial.print(analogRead(strHexToInt8(buffer[i+1], buffer[i+2])), HEX);
          i += 2;
          break;
        case 'r':
          // echo 
          Serial.println("Echo 8bit:");
          //Serial.print(temp1, HEX);
          Serial.print(strHexToInt8(buffer[i+1], buffer[i+2]), HEX);
          i += 2;
          break;
        // TODO: SPI interface
      }
      Serial.print(".\n");
      i++;
    }
  }
}

// To test the pwm module:
// n600010n60FE26_  // sleep mode and set frequency to 100hz (16M / 4096 / 100 -1)
// n600080n600104_  // reset and set totem mode

// n604200n604300_  // set pwm 0, 0x100 in channel 15
// n604400n604501_


// n604200n604300_  // set pwm 0, 0x700 in channel 15
// n604400n604507_


// To test turning on the red led:
// O021o020_
// o021_

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX. This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response. Multiple bytes of data may be available.
*/
/*
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    switch (inChar) {
      case 'a':
        digitalWrite(ledPin, HIGH);  
      break;
      case 'b':
        digitalWrite(ledPin, LOW);  
      break;
    } 
  }
}*/
