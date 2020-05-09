#define STEPS_PER_REV 3200
#define UL_PER_REV 200
#define DISPENSE_MS 2000
#define SIGNAL_PAUSE_US 3
#define PIN_STEP 6
#define PIN_EN 13
#define PIN_DISPENSE 7
#define MOTOR_PAUSE_US 10

void step_motor(void);

unsigned int uL_dispense = 1500;
unsigned long total_steps = long(uL_dispense) * long(STEPS_PER_REV) / long(UL_PER_REV);
int US_PER_STEP = long(DISPENSE_MS) * 1000 / total_steps - MOTOR_PAUSE_US * 2; // Correcting for motor pulse pause


void setup() {
  pinMode(PIN_STEP, OUTPUT);
  digitalWrite(PIN_STEP, LOW);
  pinMode(PIN_EN, OUTPUT);
  digitalWrite(PIN_EN, HIGH);
  pinMode(PIN_DISPENSE, INPUT_PULLUP);
  
}

void loop() {
  unsigned long steps = total_steps;
  if(digitalRead(PIN_DISPENSE) == LOW){
    digitalWrite(PIN_EN, LOW);
    while(steps > 0){
      step_motor();
      delayMicroseconds(US_PER_STEP);
      steps--;
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
