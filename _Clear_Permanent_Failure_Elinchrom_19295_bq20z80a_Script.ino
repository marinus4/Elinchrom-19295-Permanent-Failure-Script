#include <Wire.h>

#define I2C_ADDR 0xB
#define MANUFACTURER_ACCESS 0x00
#define PF_STATUS 0x53
#define OPERATION_STATUS 0x54
#define UNSEAL_KEY 0x60
#define PF_KEY 0x62
#define FULL_ACCESS_KEY 0x61

uint16_t readWord(uint8_t address);
uint32_t readLong(uint8_t address);
void writeWord(uint8_t address, uint16_t word);

void setup() {
  Serial.begin(115200);
    // Set Baud rate in Arduino IDE to 115200
    Wire.begin();

    // Step 1 Unseal, 0x36710414 is the Manufacturer Access or Unseal code
    Serial.println("(1) Initial Status Check");
    Serial.print("- OperationStatus: ");
    Serial.println(readWord(OPERATION_STATUS), HEX);
    
    Serial.println("(2) Unseal and Access Level Check...");
    writeWord(MANUFACTURER_ACCESS, 0x0414);
    delay(50);
    writeWord(MANUFACTURER_ACCESS, 0x3672);
    delay(200);

    Serial.print("- OperationStatus: ");
    Serial.println(readWord(OPERATION_STATUS), HEX);

    
    // Step 2, Clear Permanent Failure, 0x17122673 is the Permanent Failure Clear key
    Serial.println("(3) Clearing Permanent Failure Flag...");
    
    Serial.println("- Initial PF Flag Check");
    
    Serial.print("- PF Status: ");
    Serial.println(readWord(PF_STATUS), HEX);
    
    Serial.println("- Clearing PF Flag");
    writeWord(MANUFACTURER_ACCESS, 0x2673);
    delay(50);
    writeWord(MANUFACTURER_ACCESS, 0x1712);
    delay(200);

    Serial.println("(4) Result...");
    Serial.print("- PF Status: ");
    Serial.println(readWord(PF_STATUS), HEX);

    // Step 3 Reset
    Serial.println("(5) Reset...");
    writeWord(0x00, 0x0041);
    delay(1000);

    // Step 4 Seal
    Serial.println("(6) Re-Seal Battery");
    writeWord(MANUFACTURER_ACCESS, 0x0020);

    Serial.print("OperationStatus: ");
    Serial.println(readWord(OPERATION_STATUS), HEX);
}

void loop() {
}

void writeWord(uint8_t address, uint16_t word)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(address);
    Wire.write(word & 0xFF);
    Wire.write((word >> 8) & 0xFF);
    Wire.endTransmission();
}

uint16_t readWord(uint8_t address)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(address);
    Wire.endTransmission(false);

    Wire.requestFrom(I2C_ADDR, 2, true);
    uint16_t retVal = Wire.read();
    retVal |= Wire.read() << 8;
    Wire.endTransmission();

    return retVal;
}

uint32_t readLong(uint8_t address)
{
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(address);
    Wire.endTransmission(false);

    Wire.requestFrom(I2C_ADDR, 5, true);
    Wire.read(); // Ignoring the length byte (this is a block-read transaction)
    uint32_t retVal = Wire.read();
    retVal |= ((uint32_t) Wire.read() << 8);
    retVal |= ((uint32_t) Wire.read() << 16);
    retVal |= ((uint32_t) Wire.read() << 24);
    Wire.endTransmission();

    return retVal;
}

// TI Bq80z20a Codes
// Unseal 00 0414 3672
// Manufacturer Access 00 ffff ffff
// Clear Permanent Failure 00 2673 1712
// Reset 0x0041
