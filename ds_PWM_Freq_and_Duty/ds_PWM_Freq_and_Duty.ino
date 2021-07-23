// generate pulses to drive step-up voltage generator
// high or low duty cycle?
// two potentiometers are inputs for frequency and duty cycle, connect to Agnd and 3.3V (pins 26, 27)

// add fctn to ENA/DIS output at interval to observe switching.

#include <elapsedmillis.h>
#include <Streaming.h>
// instances of timed counters
elapsedMicros p_period;
elapsedMicros p_width;
elapsedMillis check_inputs;

// pins
const int LED_PIN   = 13;
const int PER_PIN   = A0;   //
const int DCYC_PIN  = A1;   //
const int PULSE_PIN = 11;   // serial drive transistor
const int SHUNT_PIN = 12;   // shunt pin is at same freq and anti-phase of PULSE
const int ENA_PIN = 9;
const int OFF_STATE = 1;
const int ON_STATE = 0;

// globals
const boolean PRINT_FLAG = 0;
static char xbuf[20];    // for string formatting

int const check_time = 250;   // ms

int D;      // duty cycle (fraction of time high)
int P;      // period of cycle
int P_prev = 0;
int D_prev = 0;


// -----------------------------------------------------------------------
void setup() {
  pinMode(LED_PIN, OUTPUT); //
  pinMode(PULSE_PIN, OUTPUT); //
  pinMode(SHUNT_PIN, OUTPUT); //
  pinMode(ENA_PIN, OUTPUT); //
  digitalWriteFast(PULSE_PIN, OFF_STATE);
  digitalWriteFast(SHUNT_PIN, OFF_STATE);
  digitalWriteFast(ENA_PIN, 0);

  Serial.begin(38400);
  while (!Serial && (millis() < 7000)) {  // timeout interval for print
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); delay(30);    // startup flash while print setup
    ; // continues if the monitor window is never opened
  }
  Serial.println("\nGenerating PWM\r\n");

  read_ADCs();
  print_D_P();
  delay(100);
}

void loop() {
  // generate a controlled pulse width D at period P
  // shunt off first, on last
  static int i = 0;
  // every N pulses, shunt to see time to switch off and back on to full V
  i++;

  if ( i % 128 > 2) {
    // pulse
    digitalWriteFast(SHUNT_PIN, OFF_STATE); digitalWriteFast(PULSE_PIN, ON_STATE);
    delayMicroseconds(D);
    digitalWriteFast(PULSE_PIN, OFF_STATE); //digitalWriteFast(SHUNT_PIN, ON_STATE);
    delayMicroseconds(P - D);
  }
  else {
    // shunt
    digitalWriteFast(LED_PIN, 1);

    digitalWriteFast(PULSE_PIN, OFF_STATE); digitalWriteFast(SHUNT_PIN, ON_STATE);
    delayMicroseconds(P - D);
    delayMicroseconds(D);
    //delay(1);
    digitalWriteFast(LED_PIN, 0);
  }
  if (check_inputs >= check_time) {
    check_inputs = check_inputs - check_time;
    read_ADCs();
  }

}   // end of loop


void read_ADCs() {
  int k;
  int DD;
  k = (analogRead(PER_PIN) + analogRead(PER_PIN)) / 2; // avg
  P = map(k, 0, 1023, 800, 30);   // 1000us (1kHz) - 100us (10kHz)
  k = (analogRead(DCYC_PIN) + analogRead(DCYC_PIN)) / 2;
  DD = map(k, 0, 1023, 6, 124); // percent ~scaled to 128
  D = DD * P / 128;
  if (1 || P_prev != P || D_prev != D ) {   // something's changed
    // digitalWriteFast(LED_PIN, 1); delay(5);
    // digitalWriteFast(LED_PIN, 0); delay(5);

    if (PRINT_FLAG) {
      Serial.print(millis() % 9000 + 999);
      print_D_P();
    }
    P_prev = P;
    D_prev = D;
  }
}

void print_D_P() {
  Serial.print("  freq ");  Serial.print(1000.0 / P);
  Serial.print("kHz   dcyc ");    Serial.print(100 * D / P);
  Serial.print("%   ");  Serial.print(D);   Serial.print(" of "); Serial.print(P); Serial.print(" [us] ");
  Serial.print("\r\n");
}



/*
  static boolean ON_flag = 0;
  // start with high output
  if (p_period < (D) && !ON_flag) {
    digitalWriteFast(PULSE_PIN, 1);
    ON_flag = 1;
  }
  else if (p_period >= (P - D)) { // && ON_flag) {
    digitalWriteFast(PULSE_PIN, 0);
    p_period = 0;      //p_period - P;
    ON_flag = 0;
  }
  //  Serial.print(ON_flag);
*/















