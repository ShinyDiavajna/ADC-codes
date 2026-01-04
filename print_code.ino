#define N_BITS 8

// ---------- SAR DAC pins (used in SAR loop) ----------
const int sarDacPins[N_BITS] = {0, 1, 2, 3, 4, 5, 6, 7};

// ---------- Reconstruction DAC pins ----------
const int reconDacPins[N_BITS] = {A0, A1, A2, A3, A4, A5, 8, 9};

// Comparator output (LM311 pin 7)
const int compPin = 10;

byte sarValue = 0;

// ---------------- SETUP ----------------
void setup() {

  // SAR DAC pins
  for (int i = 0; i < N_BITS; i++) {
    pinMode(sarDacPins[i], OUTPUT);
    digitalWrite(sarDacPins[i], LOW);
  }

  // Reconstruction DAC pins
  for (int i = 0; i < N_BITS; i++) {
    pinMode(reconDacPins[i], OUTPUT);
    digitalWrite(reconDacPins[i], LOW);
  }

  // LM311 open collector → pull-up needed
  pinMode(compPin, INPUT_PULLUP);

  // 🔴 START SERIAL COMMUNICATION
  Serial.begin(9600);
}

// ---------------- MAIN LOOP ----------------
void loop() {

  sarValue = 0;

  // SAR conversion: MSB → LSB
  for (int bit = N_BITS - 1; bit >= 0; bit--) {

    // Try setting bit
    sarValue |= (1 << bit);
    outputToDACs(sarValue);

    delayMicroseconds(10);  // settling time

    // Comparator decision
    // LOW  → Vin < Vdac → clear bit
    // HIGH → Vin ≥ Vdac → keep bit
    if (digitalRead(compPin) == LOW) {
      sarValue &= ~(1 << bit);
      outputToDACs(sarValue);
    }
  }

  // 🔴 PRINT FINAL ADC OUTPUT CODE (TO SERIAL MONITOR)
  Serial.println(sarValue);

  delay(300);  // slow printing so you can read it
}

// ---------------- OUTPUT TO BOTH DACs ----------------
void outputToDACs(byte value) {

  // SAR DAC
  for (int i = 0; i < N_BITS; i++) {
    digitalWrite(sarDacPins[i], (value >> i) & 0x01);
  }

  // Reconstruction DAC
  for (int i = 0; i < N_BITS; i++) {
    digitalWrite(reconDacPins[i], (value >> i) & 0x01);
  }
}