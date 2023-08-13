
/* pinout */
#define MOTOR_1_PWM 1
#define MOTOR_2_PWM 2
#define MOTOR_3_PWM 5
#define PUMP_PWM 6

#define MOTOR_1_DIR 0
#define MOTOR_2_DIR 3 
#define MOTOR_3_DIR 4 
#define PUMP_DIR 7

#define MOTOR_1_POT A2
#define MOTOR_2_POT A1
#define MOTOR_3_POT A0

#define PUMP_BTN 16

/* pump settings */
#define PUMP_DURATION_MS  1000 // short dispensing from the pump
#define PUMP_SPEED 125 // choose a number between 0 (off) and 255 (max speed) 

/* type definitions*/
struct motor_t {
  uint8_t id;
  uint8_t pwm_pin;
  uint8_t dir_pin;
  uint8_t pot_pin;
  uint8_t direction;
  uint8_t pwm_value;
};

/* global variables */
motor_t motor_1 = {0};
motor_t motor_2 = {0};
motor_t motor_3 = {0};
motor_t pump = {0};

bool pump_is_running = false;
uint32_t pump_run_start = 0;
volatile bool btn_press_flag = false;
volatile uint32_t last_btn_press = 0;


void setup() {
  // put your setup code here, to run once:
  // Serial.begin(152000);

  // setup sensor inputs

  // setup motor outputs
  pinMode(MOTOR_1_PWM, OUTPUT);
  pinMode(MOTOR_2_PWM, OUTPUT);
  pinMode(MOTOR_3_PWM, OUTPUT);
  pinMode(PUMP_PWM, OUTPUT);

  pinMode(MOTOR_1_DIR, OUTPUT);
  pinMode(MOTOR_2_DIR, OUTPUT);
  pinMode(MOTOR_3_DIR, OUTPUT);
  pinMode(PUMP_DIR, OUTPUT);

  pinMode(PUMP_BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PUMP_BTN), btn_press, FALLING);

  // initialize motor outputs 
  digitalWrite(MOTOR_1_PWM, LOW);
  digitalWrite(MOTOR_2_PWM, LOW);
  digitalWrite(MOTOR_3_PWM, LOW);
  digitalWrite(PUMP_PWM, LOW);

  digitalWrite(MOTOR_1_DIR, LOW);
  digitalWrite(MOTOR_2_DIR, LOW);
  digitalWrite(MOTOR_3_DIR, LOW);
  digitalWrite(PUMP_DIR, LOW);

  // initialize individual motor structs
  motor_1.id = 1; 
  motor_1.pwm_pin = MOTOR_1_PWM; 
  motor_1.dir_pin = MOTOR_1_DIR;
  motor_1.pot_pin = MOTOR_1_POT;

  motor_2.id = 2;
  motor_2.pwm_pin = MOTOR_2_PWM; 
  motor_2.dir_pin = MOTOR_2_DIR;
  motor_2.pot_pin = MOTOR_2_POT;

  motor_3.id = 3;
  motor_3.pwm_pin = MOTOR_3_PWM; 
  motor_3.dir_pin = MOTOR_3_DIR;
  motor_3.pot_pin = MOTOR_3_POT;

  pump.id = 4 ;
  pump.pwm_pin = PUMP_PWM; 
  pump.dir_pin = PUMP_DIR;
  pump.pot_pin = PUMP_BTN;
}

void loop() {
  // put your main code here, to run repeatedly:

  uint32_t now = millis();

  // read the potentiometer, update speed reading and PWM value
  run_motor(&motor_1);
  run_motor(&motor_2);
  run_motor(&motor_3);
  
  run_pump(&pump);

  delay(50);
}

void run_motor(motor_t* motor){
  
  // read potentiometer
  uint16_t pot = analogRead(motor->pot_pin);

  if (pot_to_pwm_value(pot) != motor->pwm_value){
    // update speed value
    motor->pwm_value = pot_to_pwm_value(pot);

    // update PWM output 
    analogWrite(motor->pwm_pin, motor->pwm_value);
    
    Serial.print("Motor ");
    Serial.print(motor->id);
    Serial.print(" changed speed to ");
    Serial.println(motor->pwm_value);
  }
  
}

void run_pump(motor_t* pump){

  uint32_t now = millis();

  // check if the pump is already running
  if (btn_press_flag)
  {
    if (!pump_is_running){
      pump->pwm_value = PUMP_SPEED;
      analogWrite(pump->pwm_pin, pump->pwm_value);
      
      pump_is_running = true;
      pump_run_start = millis();

      Serial.println("Pump started");
    }

    if ((now - pump_run_start) > PUMP_DURATION_MS)
    {
      pump_is_running = false;
      btn_press_flag = false;

      Serial.println("Pump stopped");
    }
    
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

void btn_press()
{
  uint32_t now = millis();

  if ((now - last_btn_press) > 100)
  {
    last_btn_press = now;
    btn_press_flag = true;
  }

}



