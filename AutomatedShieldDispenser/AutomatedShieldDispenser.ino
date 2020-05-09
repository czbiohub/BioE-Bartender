#define PUMP_ML_MIN 160
#define NUM_TUBES 12
#define TARGET_VOLUME 2620 //uL
#define OFFSET_HOME 0
#define OFFSET_FRACS 27000
#define STEPS_PER_REV 200
#define DIST_PER_REV 3175 //microns
#define SIGNAL_PAUSE_US 5
#define US_PER_STEP 1100

#define PIN_EN_FRAC 2
#define PIN_DIR_FRAC 3
#define PIN_STEP_FRAC 4
#define PIN_PUMP 5
#define PIN_RUN_SENSE 10
#define PIN_RUN_SINK 11
#define PIN_LIM_SOURCE 12
#define PIN_LIM_SENSE 13

unsigned long dispense_ms = 0;

void primeSystem(void);
void runProcedure(void);
void dispense(void);
void homeFrac(void);
void moveFrac(long dist_um);
void stepMotor(void);

void setup() {
  
  //Home Limit Switch
  pinMode(PIN_LIM_SOURCE, OUTPUT);
  digitalWrite(PIN_LIM_SOURCE, HIGH);
  pinMode(PIN_LIM_SENSE, INPUT);

  //Run Switch
  pinMode(PIN_RUN_SENSE, INPUT_PULLUP);
  pinMode(PIN_RUN_SINK, OUTPUT);
  digitalWrite(PIN_RUN_SINK, LOW);

  //Pump Toggle
  pinMode(PIN_PUMP, OUTPUT);
  digitalWrite(PIN_PUMP, LOW);

  //Stepper Pins
  pinMode(PIN_EN_FRAC, OUTPUT);
  digitalWrite(PIN_EN_FRAC, HIGH);
  pinMode(PIN_STEP_FRAC, OUTPUT);
  digitalWrite(PIN_STEP_FRAC, LOW);
  pinMode(PIN_DIR_FRAC, OUTPUT);
  digitalWrite(PIN_DIR_FRAC, LOW);

  float pump_mL_s = PUMP_ML_MIN / (float)60;
  float pump_uL_ms = pump_mL_s;
  dispense_ms = (float)TARGET_VOLUME * (float)NUM_TUBES / pump_uL_ms;
}

void loop() {
  if(digitalRead(PIN_RUN_SENSE) == LOW){
    int i = 0;
    while(digitalRead(PIN_RUN_SENSE) == LOW && i < 15){
      i++;
      delay(100);
    }
    if (i == 15) { 
      primeSystem();
    }

    if (i < 15 && i > 1){
      runProcedure();
    }
  }
}

void primeSystem(void){ 
  homeFrac();
  if(digitalRead(PIN_RUN_SENSE) == LOW){ digitalWrite(PIN_PUMP, HIGH); }
  while(digitalRead(PIN_RUN_SENSE) == LOW){ delay(20); }
  digitalWrite(PIN_PUMP, LOW);
  delay(4000);
  for(int i = 7; i > 0; i--){moveFrac(OFFSET_FRACS);}
  moveFrac(OFFSET_FRACS/2);
}

void runProcedure(void){
  homeFrac();
  dispense();
  delay(3000);
  for(int i = 7; i > 0; i--){
    moveFrac(OFFSET_FRACS);
    dispense();
    delay(3000);
  }
  moveFrac(OFFSET_FRACS/2);
}

void homeFrac(void){
  digitalWrite(PIN_DIR_FRAC, LOW);
  digitalWrite(PIN_EN_FRAC, LOW);
  while(digitalRead(PIN_LIM_SENSE) == HIGH){
    stepMotor();
    delayMicroseconds(US_PER_STEP);
  }
  moveFrac(OFFSET_HOME);
  digitalWrite(PIN_EN_FRAC, HIGH);
}

void dispense(void){
  digitalWrite(PIN_PUMP, HIGH);
  delay(dispense_ms);
  digitalWrite(PIN_PUMP, LOW);
}

void pulsePump(void){
  delay(2000);
  digitalWrite(PIN_PUMP, HIGH);
  delay(10);
  digitalWrite(PIN_PUMP, LOW);
}

void moveFrac(long dist_um){
  float dist_per_step = (float)DIST_PER_REV / (float)STEPS_PER_REV;
  unsigned long move_steps = abs(dist_um) / dist_per_step;
  if(dist_um < 0){
    digitalWrite(PIN_DIR_FRAC, LOW);
  }
  else{
    digitalWrite(PIN_DIR_FRAC, HIGH);
  }
  digitalWrite(PIN_EN_FRAC, LOW);
  while(move_steps > 0){
    stepMotor();
    delayMicroseconds(US_PER_STEP);
    move_steps--;
  }
  digitalWrite(PIN_EN_FRAC, HIGH);
}

void stepMotor(void){
  digitalWrite(PIN_STEP_FRAC, HIGH);
  delayMicroseconds(SIGNAL_PAUSE_US);
  digitalWrite(PIN_STEP_FRAC, LOW);
  delayMicroseconds(SIGNAL_PAUSE_US);
}
