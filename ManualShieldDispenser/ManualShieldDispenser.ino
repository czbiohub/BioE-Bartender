#define STEPS_PER_REV 3200
#define UL_PER_REV 200
#define DISPENSE_MS 2000
#define SIGNAL_PAUSE_US 3
#define PIN_STEP 6
#define PIN_EN 13
#define PIN_DISPENSE 7
#define US_PER_STEP 10

unsigned long total_steps = 0;
unsigned int uL_dispense = 3000;
void step_motor(void);

void setup() {
  pinMode(PIN_STEP, OUTPUT);
  digitalWrite(PIN_STEP, LOW);
  pinMode(PIN_EN, OUTPUT);
  digitalWrite(PIN_EN, HIGH);
  pinMode(PIN_DISPENSE, INPUT_PULLUP);
  
}

void loop() {
  if(digitalRead(PIN_DISPENSE) == LOW){
    total_steps = long(uL_dispense) * long(STEPS_PER_REV) / long(UL_PER_REV);
    US_PER_STEP = long(DISPENSE_MS) * 1000 / total_steps - motor_pause_us * 2; // Correcting for motor pulse pause
    digitalWrite(PIN_EN, LOW);
    while(total_steps > 0){
      step_motor();
      delayMicroseconds(US_PER_STEP);
      total_steps--;
    }
    digitalWrite(PIN_EN, HIGH);
    while(digitalRead( PIN_DISPENSE ) == LOW){ }
    delay(1000);
  }
}

void step_motor(void){
  digitalWrite(PIN_STEP, HIGH);
  delayMicroseconds(SIGNAL_PAUSE_US);
  digitalWrite(PIN_STEP, LOW);
  delayMicroseconds(SIGNAL_PAUSE_US);
}
