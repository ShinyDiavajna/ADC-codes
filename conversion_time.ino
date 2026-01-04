#define N_BITS 8

// ---------- SAR DAC pins (used in SAR loop) ----------
const int sarDacPins[N_BITS] = {0, 1, 2, 3, 4, 5, 6, 7};

// ---------- Reconstruction DAC pins ----------
const int reconDacPins[N_BITS] = {A0, A1, A2, A3, A4, A5, 8, 9};

// Comparator output (LM311 open collector)
const int compPin = 10;

// Timing pin to measure conversion time
const int timingPin = 13;   // Arduino onboard LED pin

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

  // Timing pin
  pinMode(timingPin, OUTPUT);
  digitalWrite(timingPin, LOW);
}

// ---------------- MAIN LOOP ----------------
void loop() {

  // ===== START OF CONVERSION =====
  digitalWrite(timingPin, HIGH);

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

  // ===== END OF CONVERSION =====
  digitalWrite(timingPin, LOW);

  // Hold result
  delay(100);
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