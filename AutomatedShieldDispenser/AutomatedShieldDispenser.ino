#define PUMP_ML_MIN 160
#define NUM_TUBES 12
#define TARGET_VOLUME 1000 //uL
#define OFFSET_HOME 50
#define OFFSET_FRACS 150
#define STEPS_PER_REV 3200
#define DIST_PER_REV 3175 //microns
#define SIGNAL_PAUSE_US 5
#define US_PER_STEP 10

#define PIN_DIR_FRAC 5
#define PIN_STEP_FRAC 6
#define PIN_HOME_FRAC 7
#define PIN_RUN 11
#define PIN_DISPENSE 12
#define PIN_EN_FRAC 13

unsigned long move_steps = 0;
unsigned int uL_dispense = 1000;
int dispense_ms = 0;

void dispense(void);
void homeFrac(void);
void moveFrac(long dist_um);
void stepMotor(void);

void setup() {
  pinMode(PIN_DIR_FRAC, OUTPUT);
  digitalWrite(PIN_DIR_FRAC, HIGH);
  pinMode(PIN_STEP_FRAC, OUTPUT);
  digitalWrite(PIN_STEP_FRAC, LOW);
  pinMode(PIN_HOME_FRAC, INPUT_PULLUP);
  pinMode(PIN_RUN, INPUT_PULLUP);
  pinMode(PIN_DISPENSE, OUTPUT);
  digitalWrite(PIN_DISPENSE, LOW);
  pinMode(PIN_EN_FRAC, OUTPUT);
  digitalWrite(PIN_EN_FRAC, HIGH);

  int pump_mL_s = PUMP_ML_MIN / 60;
  int pump_uL_ms = pump_mL_s;
  dispense_ms = TARGET_VOLUME / pump_uL_ms;
}

void loop() {
  if(digitalRead(PIN_DISPENSE) == LOW){
    homeFrac();
    dispense();
    for(int i = 11; i > 0; i--){
      moveFrac(OFFSET_FRACS);
      dispense();
    }
    homeFrac();
    while(digitalRead( PIN_DISPENSE ) == LOW){ }
    delay(1000);
  }
}

void dispense(void){
  digitalWrite(PIN_DISPENSE, HIGH);
  delay(dispense_ms);
  digitalWrite(PIN_DISPENSE, LOW);
}

void homeFrac(void){
  digitalWrite(PIN_DIR_FRAC, LOW);
  digitalWrite(PIN_EN_FRAC, LOW);
  while(digitalRead(PIN_HOME_FRAC) == HIGH){
    step_motor();
    delayMicroseconds(US_PER_STEP);
  }
  moveFrac(OFFSET_HOME);
}

void moveFrac(long dist_um){
  move_steps = abs(dist_um) / DIST_PER_REV / STEPS_PER_REV;
  if(dist_um < 0){
    digitalWrite(PIN_DIR_FRAC, LOW);
  }
  else{
    digitalWrite(PIN_DIR_FRAC, HIGH);
  }
  digitalWrite(PIN_EN_FRAC, LOW);
  while(move_steps > 0){
    step_motor();
    delayMicroseconds(US_PER_STEP);
    move_steps--;
  }
  digitalWrite(PIN_EN_FRAC, HIGH);
}

void step_motor(void){
  digitalWrite(PIN_STEP_FRAC, HIGH);
  delayMicroseconds(SIGNAL_PAUSE_US);
  digitalWrite(PIN_STEP_FRAC, LOW);
  delayMicroseconds(SIGNAL_PAUSE_US);
}
