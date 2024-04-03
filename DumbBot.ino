// ---------------------------------------------------------------------------
// NewPing library sketch that interfaces with all but the SRF06 sensor using
// only one Arduino pin. You can also interface with the SRF06 using one pin
// if you install a 0.1uf capacitor on the trigger and echo pins of the sensor
// then tie the trigger pin to the Arduino pin (doesn't work with Teensy).
// ---------------------------------------------------------------------------

#include <NewPing.h>
#include <CircularBuffer.hpp>
#include <Adafruit_MotorShield.h>

// DC MOTORS
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *Left_Motor = AFMS.getMotor(1);
Adafruit_DCMotor *Right_Motor = AFMS.getMotor(2);

int L_Motor_Speed = 0;
int R_Motor_Speed = 0;

//ECHO SENSOR's
#define MAX_DISTANCE 900 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing Front_Right_Sonar(12, 12, MAX_DISTANCE); // NewPing setup of pin and maximum distance.
NewPing Rear_Right_Sonar(13, 13, MAX_DISTANCE); //RIGHT SONARS

#define buff_size 5
CircularBuffer<int,buff_size> FR_Sonar_Buffer;
CircularBuffer<int,buff_size> BR_Sonar_Buffer;
int FR_Sonar_Value = 0;
int BR_Sonar_Value = 0;

// SONAR FUCNTIONS
void update(){
  FR_Sonar_Buffer.push(Front_Right_Sonar.ping_cm());
  BR_Sonar_Buffer.push(Rear_Right_Sonar.ping_cm());

  for (int i = 0; i < buff_size; i++){
    FR_Sonar_Value = FR_Sonar_Value + FR_Sonar_Buffer[i];
    BR_Sonar_Value = BR_Sonar_Value + BR_Sonar_Buffer[i];
  }

  FR_Sonar_Value = FR_Sonar_Value/buff_size;
  BR_Sonar_Value = BR_Sonar_Value/buff_size;
}
void dump_sonar_data(){
  Serial.print("R: Front: ");
  Serial.print(FR_Sonar_Value);
  Serial.print(" Back: ");
  Serial.print(BR_Sonar_Value);
  Serial.println("cm");
}

//DRIVE FUNCTIONS
int safe_drive_motors(){
  if(L_Motor_Speed > 200){
    return -1;
  }
  if(R_Motor_Speed > 200){
    return -1;
  }
    if(L_Motor_Speed < 0){
    return -1;
  }
  if(R_Motor_Speed < 0){
    return -1;
  }
  drive_motor(L_Motor_Speed, R_Motor_Speed);
}
void drive_motor ( int a, int b ){
  Left_Motor->setSpeed(a);
  Right_Motor->setSpeed(b);
}
void dump_motor_data(){
  Serial.print("L ");
  Serial.print(L_Motor_Speed);
  Serial.print(" R: ");
  Serial.println(R_Motor_Speed);
}

void setup() {
  Serial.begin(9600); // Open serial monitor at 9600 baud to see ping results.
  if (!AFMS.begin()) {         // create with the default frequency 1.6KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1);
  }      
  Left_Motor->run(FORWARD);
  Right_Motor->run(FORWARD);
  pinMode(11, OUTPUT);
  Serial.println("All Status's Updated");
  delay(1000);
}
void die(){
  drive_motor(0,0);
  delay(99999);
  
}
//Test Deltas
int delta = 0;
int kp = 8;
void loop() {
  //die();
  update();

  delta = int(FR_Sonar_Value) - int(BR_Sonar_Value);
  Serial.println(delta);
  L_Motor_Speed = 150 + kp*delta;
  R_Motor_Speed = 150;
  if(L_Motor_Speed > R_Motor_Speed){
    digitalWrite(11,HIGH);
  }else{
    digitalWrite(11,LOW);
  }
  safe_drive_motors();

 
  delay(29);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
}
