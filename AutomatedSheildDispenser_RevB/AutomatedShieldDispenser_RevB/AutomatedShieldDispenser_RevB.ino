//Dispense Definitions
#define PUMP_ML_MIN 160
#define NUM_TUBES 12
#define TARGET_VOLUME1 1400 // 1500 uL
#define TARGET_VOLUME2 1850 // 2000 uL

//Fraction Definitions
#define OFFSET_HOME 0
#define OFFSET_FRACS 27000
#define STEPS_PER_REV 200
#define DIST_PER_REV 3175 //microns
#define SIGNAL_PAUSE_US 150
#define US_PER_STEP 1100

//Pin Defintions
#define PIN_LIM_SWITCH 2
#define PIN_EN_FRAC 8
#define PIN_STEP_FRAC 3
#define PIN_DIR_FRAC 4
#define PIN_EN_PUMP 5
#define PIN_REV_PUMP 6
#define PIN_FWD_PUMP 7
#define PIN_RUN_SWITCH 9
#define PIN_MODE_SWITCH 13

unsigned long dispense_ms = 0;

void primeSystem(void);
void pulsePumpRev(void);
void runProcedure(void);
void dispense(void);
void homeFrac(void);
void moveFrac(long dist_um);
void stepMotor(void);

void setup() {

  //Peristaltic Pump
  pinMode(PIN_EN_PUMP, OUTPUT);
  pinMode(PIN_FWD_PUMP, OUTPUT);
  pinMode(PIN_REV_PUMP, OUTPUT);

  digitalWrite(PIN_EN_PUMP, LOW);
  digitalWrite(PIN_FWD_PUMP, LOW);
  digitalWrite(PIN_REV_PUMP, LOW);
  
  //Fraction Collector
  pinMode(PIN_LIM_SWITCH, INPUT_PULLUP);
  pinMode(PIN_EN_FRAC, OUTPUT);
  pinMode(PIN_DIR_FRAC, OUTPUT);
  pinMode(PIN_STEP_FRAC, OUTPUT);

  digitalWrite(PIN_EN_FRAC, HIGH);
  digitalWrite(PIN_DIR_FRAC, LOW);
  digitalWrite(PIN_STEP_FRAC, LOW);

  //Run Switch
  pinMode(PIN_RUN_SWITCH, INPUT_PULLUP);

  //Mode Switch
  pinMode(PIN_MODE_SWITCH, OUTPUT);
  digitalWrite(PIN_MODE_SWITCH, HIGH);
}

void loop() {
  if(digitalRead(PIN_RUN_SWITCH) == LOW){
    int i = 0;
    while(digitalRead(PIN_RUN_SWITCH) == LOW && i < 15){
      i++;
      delay(100);
    }
    if (i == 15) { 
      primeSystem();
    }

    if (i < 15 && i > 1){
      int TARGET_VOLUME = 0;
      if(digitalRead(PIN_MODE_SWITCH) == LOW){
        TARGET_VOLUME = TARGET_VOLUME1;
      }
      else{
        TARGET_VOLUME = TARGET_VOLUME2;
      }
      float pump_mL_s = PUMP_ML_MIN / (float)60;
      float pump_uL_ms = pump_mL_s;
      dispense_ms = (float)TARGET_VOLUME * (float)NUM_TUBES / pump_uL_ms;
      runProcedure();
    }
  }
}

void primeSystem(void){ 
  homeFrac();
  if(digitalRead(PIN_RUN_SWITCH) == LOW){ 
    digitalWrite(PIN_EN_PUMP, HIGH);
    delay(50);
    digitalWrite(PIN_FWD_PUMP, HIGH); 
  }
  while(digitalRead(PIN_RUN_SWITCH) == LOW){ delay(20);}
  digitalWrite(PIN_FWD_PUMP, LOW);
  digitalWrite(PIN_EN_PUMP, LOW);
  pulsePumpRev();
  for(int i = 7; i > 0; i--){moveFrac(OFFSET_FRACS);}
  moveFrac(OFFSET_FRACS/2);
}

void runProcedure(void){
  homeFrac();
  dispense();
  pulsePumpRev();
  for(int i = 7; i > 0; i--){
    moveFrac(OFFSET_FRACS);
    dispense();
    pulsePumpRev();
  }
  moveFrac(OFFSET_FRACS/2);
}

void homeFrac(void){
  digitalWrite(PIN_DIR_FRAC, LOW);
  digitalWrite(PIN_EN_FRAC, LOW);
  while(digitalRead(PIN_LIM_SWITCH) == LOW){
    stepMotor();
    delayMicroseconds(US_PER_STEP);
  }
  moveFrac(OFFSET_HOME);
  digitalWrite(PIN_EN_FRAC, HIGH);
}

void dispense(void){
  digitalWrite(PIN_EN_PUMP, HIGH);
  digitalWrite(PIN_FWD_PUMP, HIGH);
  delay(dispense_ms);
  digitalWrite(PIN_FWD_PUMP, LOW);
  digitalWrite(PIN_EN_PUMP, LOW);
}

void pulsePumpRev(void){
  digitalWrite(PIN_EN_PUMP, HIGH);
  digitalWrite(PIN_REV_PUMP, HIGH);
  delay(200);
  digitalWrite(PIN_REV_PUMP, LOW);
  digitalWrite(PIN_EN_PUMP, LOW);
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
