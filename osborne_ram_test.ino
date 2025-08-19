/*
DRAM Test for 64k Osborne Executive Memory Board
For use with Osborne Computer Corp RD1140 REV. 02 
Testing done with Arduino MEGA 2560
*/

// https://github.com/levitas/osborne_ram_test/

// Pin definitions
const int CAE_PIN = 42;     // CAE* (Column Address Enable)
const int RAE_PIN = 44;     // RAE* (Row Address Enable)  
const int RAS_PIN = 46;     // RAS (Row Address Strobe - active HIGH)
const int CAS_PIN = 48;     // CAS* (Column Address Strobe - active LOW)
const int RD_PIN = 8;       // RD* (Read Enable)

// Address Pins
const int ADDR_PINS[] = {
  53, 51, 49, 47, 45, 43, 41, 39, 37, 35, 33, 31, 29, 27, 25, 23
};

// Data pins
const int DATA_PINS[] = {36, 34, 32, 30, 28, 26, 24, 22};

const byte TEST_BYTES[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

#define NX(x) (sizeof(x) / sizeof((x)[0]))

const size_t NA = NX(ADDR_PINS);
const size_t ND = NX(DATA_PINS);
const size_t NB = NX(TEST_BYTES);

const uint32_t addressCount = 65536;

void setup() {
  Serial.begin(115200);
  Serial.println("=== Osborne 64k RAM Test ===");
  Serial.println("Testing basic write/read cycle");
  Serial.println();
  
  initializePins();
  
  memTest();
}

void loop() {
  // Nothing in main loop
}

void initializePins() {
  Serial.println("Initializing pins...");
  
  // Control pins as outputs
  pinMode(CAE_PIN, OUTPUT);
  pinMode(RAE_PIN, OUTPUT);
  pinMode(RAS_PIN, OUTPUT);
  pinMode(CAS_PIN, OUTPUT);
  pinMode(RD_PIN, OUTPUT);
  
  // Address pins as outputs
  for (int i = 0; i < NA; i++) {
    pinMode(ADDR_PINS[i], OUTPUT);
  }
  
  // Set inactive states
  digitalWrite(CAE_PIN, HIGH);      // CAE* inactive
  digitalWrite(RAE_PIN, HIGH);      // RAE* inactive
  digitalWrite(RAS_PIN, LOW);       // RAS inactive (LOW for active HIGH)
  digitalWrite(CAS_PIN, HIGH);      // CAS* inactive
  digitalWrite(RD_PIN, HIGH);       // RD* inactive
  
  // Clear address bus
  for (int i = 0; i < NA; i++) {
    digitalWrite(ADDR_PINS[i], LOW);
  }
  
  // Data pins start as inputs
  setDataPinsInput();
  
  Serial.println("Pins initialized");
  Serial.println();
}

void setAddress(word addr) {
  // Initialize Default States
  digitalWrite(CAE_PIN, HIGH);      // CAE* inactive
  digitalWrite(RAE_PIN, HIGH);      // RAE* inactive
  digitalWrite(RAS_PIN, LOW);       // RAS inactive (LOW for active HIGH)
  digitalWrite(CAS_PIN, HIGH);      // CAS* inactive

  // Set Address Bits
  for (int i = 0; i < NA; i++) {
    digitalWrite(ADDR_PINS[i], (addr >> i) & 1);
  }

  // Set Row Address
  digitalWrite(RAE_PIN, LOW);       // RAE* active
  digitalWrite(RAS_PIN, HIGH);      // RAS active (LOW for active HIGH)
  digitalWrite(RAS_PIN, LOW);       // RAS inactive (LOW for active HIGH)
  digitalWrite(RAE_PIN, HIGH);      // RAE* inactive

  // Set Col Address
  digitalWrite(CAE_PIN, LOW);       // CAE* active
  digitalWrite(CAS_PIN, LOW);       // CAS* active
  digitalWrite(CAS_PIN, HIGH);      // CAS* inactive
  digitalWrite(CAE_PIN, HIGH);      // CAE* inactive
}

void setDataPinsOutput() {
  for (int i = 0; i < ND; i++) {
    pinMode(DATA_PINS[i], OUTPUT);
  }
}

void setDataPinsInput() {
  for (int i = 0; i < ND; i++) {
    pinMode(DATA_PINS[i], INPUT);
  }
}

void writeDataBus(byte data) {
  setDataPinsOutput();
  for (int i = 0; i < ND; i++) {
    digitalWrite(DATA_PINS[i], (data >> i) & 1);
  }
}

byte readDataBus() {
  setDataPinsInput();
  delayMicroseconds(1);
  
  byte data = 0;
  for (int i = 0; i < ND; i++) {
    if (digitalRead(DATA_PINS[i]) == HIGH) {
      data |= (1 << i);
    }
  }
  return data;
}

void dramWrite(word addr, byte data) {
  // Set data bus
  writeDataBus(data);
  
  // Set write enable
  digitalWrite(RD_PIN, HIGH);       // RD* inactive

  // Set address
  setAddress(addr);
  
  // Release data bus
  setDataPinsInput();
}

byte dramRead(word addr) {
  // Set read enable
  digitalWrite(RD_PIN, LOW);       // RD* active

  // Set address
  setAddress(addr);
  
  // Read data
  byte data = readDataBus();
  
  return data;
}

void printBinary(size_t data) {
  for (size_t b = ND; b > 0; b--) {
    Serial.print(bitRead(data, b - 1));
  }
}

void memTest() {
  uint32_t firstError = 0;
  uint32_t lastError = 0;
  uint32_t errorCount = 0;

  for (int i = 0; i < NB; i++) {
    byte pattern = TEST_BYTES[i];
    Serial.print("== Testing Bit Pattern ");
    printBinary(pattern);
    Serial.print(" ==");
    Serial.println();

    for (uint32_t addr = 0; addr < addressCount; addr++) {
      if (addr % 0x1000 == 0) {
        Serial.print("Testing Address Block 0x");
        Serial.print(addr, HEX);
        Serial.println();
      }
      dramWrite(addr, pattern);

      byte data = dramRead(addr);

      if (data != pattern) {
        lastError = addr;
        if (errorCount++ == 0) {
          firstError = addr;
        }

        Serial.print("Error at address 0x");
        Serial.print(addr, HEX);
        Serial.print(" - Got: ");
        printBinary(data);
        Serial.print(", expected: ");
        printBinary(pattern);
        Serial.println();
      }
    }
  }

  Serial.println("Test complete");
  print(errorCount);
  Serial.print(" errors found (");
  Serial.print((100.f * errorCount) / ((float)bit(NB) * addressCount));
  Serial.println("% failed)");
  
  if (errorCount > 0) {
    Serial.print("Error span: 0x");
    Serial.print(firstError, HEX);
    Serial.print(" to 0x");
    Serial.println(lastError, HEX);
  }
}
