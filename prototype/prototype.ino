
/* pinout */
#define MOTOR_1_PWM 2
#define MOTOR_2_PWM 1
#define MOTOR_3_PWM 55
#define MOTOR_4_PWM 55

#define MOTOR_1_DIR 4
#define MOTOR_2_DIR 0 
#define MOTOR_3_DIR 55 
#define MOTOR_4_DIR 55

#define MOTOR_1_POT A2
#define MOTOR_2_POT A1
#define MOTOR_3_POT A0
#define MOTOR_4_POT 55

/*TODO: implement limit sensing in init procedure + interrupt if sensor triggered then invert direction instead of timing based approach */ 
#define MOTOR_1_SENSOR 55
#define MOTOR_2_SENSOR 55
#define MOTOR_3_SENSOR 55
#define MOTOR_4_SENSOR 55

/* How long a motor stays on before it needs to change direction */ 
#define MOTOR_1_DURATION_MS  5000  
#define MOTOR_2_DURATION_MS  5000
#define MOTOR_3_DURATION_MS  5000
#define MOTOR_4_DURATION_MS  1000 // short dispensing from the pump

#define PUMP_PERIODICITY_S 30


/* type definitions*/

struct motor_t {
  uint8_t id;
  uint8_t pwm_pin;
  uint8_t dir_pin;
  uint8_t pot_pin;
  uint32_t duration;
  uint8_t direction;
  uint8_t pwm_value;
  uint32_t dir_change_time;
};

/* global variables */
motor_t motor_1 = {0};
motor_t motor_2 = {0};
motor_t motor_3 = {0};
motor_t pump = {0};


void setup() {
  // put your setup code here, to run once:
  // Serial.begin(152000);

  // setup sensor inputs

  // setup motor outputs
  pinMode(MOTOR_1_PWM, OUTPUT);
  pinMode(MOTOR_2_PWM, OUTPUT);
  pinMode(MOTOR_3_PWM, OUTPUT);
  pinMode(MOTOR_4_PWM, OUTPUT);

  pinMode(MOTOR_1_DIR, OUTPUT);
  pinMode(MOTOR_2_DIR, OUTPUT);
  pinMode(MOTOR_3_DIR, OUTPUT);
  pinMode(MOTOR_4_DIR, OUTPUT);


  // initialize motor outputs 
  digitalWrite(MOTOR_1_PWM, LOW);
  digitalWrite(MOTOR_2_PWM, LOW);
  digitalWrite(MOTOR_3_PWM, LOW);
  digitalWrite(MOTOR_4_PWM, LOW);

  digitalWrite(MOTOR_1_DIR, LOW);
  digitalWrite(MOTOR_2_DIR, LOW);
  digitalWrite(MOTOR_3_DIR, LOW);
  digitalWrite(MOTOR_4_DIR, LOW);

  // initialize individual motor structs
  motor_1.id = 1; 
  motor_1.pwm_pin = MOTOR_1_PWM; 
  motor_1.dir_pin = MOTOR_1_DIR;
  motor_1.pot_pin = MOTOR_1_POT;
  motor_1.duration  =  MOTOR_1_DURATION_MS;
  motor_1.dir_change_time = millis();
  Serial.println( motor_1.dir_change_time);


  motor_2.id = 2;
  motor_2.pwm_pin = MOTOR_2_PWM; 
  motor_2.dir_pin = MOTOR_2_DIR;
  motor_2.pot_pin = MOTOR_2_POT;
  motor_2.duration  =  MOTOR_2_DURATION_MS;
  motor_2.dir_change_time = millis();

  motor_3.id = 3;
  motor_3.pwm_pin = MOTOR_3_PWM; 
  motor_3.dir_pin = MOTOR_3_DIR;
  motor_3.pot_pin = MOTOR_3_POT;
  motor_3.duration  =  MOTOR_3_DURATION_MS;
  motor_3.dir_change_time = millis();

  pump.id = 4 ;
  pump.pwm_pin = MOTOR_4_PWM; 
  pump.dir_pin = MOTOR_4_DIR;
  pump.pot_pin = MOTOR_4_POT;
  pump.duration  =  MOTOR_4_DURATION_MS;
  pump.dir_change_time = millis();

}

void loop() {
  // put your main code here, to run repeatedly:

  uint32_t now = millis();

  // read the potentiometer, update speed reading and PWM value
  run_motor(&motor_1);
  
  // run_motor(motor_2);
  // run_motor(motor_3);

  delay(50);
}

void run_motor(motor_t* motor){
  
  // read potentiometer
  uint16_t pot = analogRead(motor->pot_pin);
  Serial.print("POT: ");
  Serial.print(pot);
  Serial.print(" - PWM: ");

  // update speed value
  motor->pwm_value = pot_to_pwm_value(pot);
  Serial.println(motor->pwm_value);

  // update PWM output 
  analogWrite(motor->pwm_pin, motor->pwm_value);

  // check how long has passed since the last direction update 
  uint32_t diff = millis() - motor->dir_change_time;

  if( diff >= motor->duration ){
    // flip direction
    motor->direction ^= 0x01; // XOR on the last bit

    // update direction pin 
    digitalWrite(motor->dir_pin, motor->direction);

    // update last switch time
    motor->dir_change_time = millis();

    Serial.print("Direction changed for motor ");
    Serial.println(motor->id);
  }

}

uint8_t pot_to_pwm_value(uint16_t potentiometer_reading){
  
  uint8_t val = potentiometer_reading >> 2; // ADC value is actually only 10 bits, shift by 2 to make 8 bits

  // we want to turn off the motor in really low speeds
  if (val < 10 ){
    val = 0;
  }

  return val;
}





