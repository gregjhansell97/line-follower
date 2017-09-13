#define MAX_RIGHT_MOTOR 255.0
#define MIN_RIGHT_MOTOR 127.0 //need the point zero so its treated as a float


int sensorValue = 0;        // value read from the pot
int outputValue = 0;  

const int right_motor_forward = 6;
const int right_motor_reverse = 3;

void setup() {
  // put your setup code here, to run once: 
  Serial.begin(9600);
  pinMode(right_motor_forward, OUTPUT);
  pinMode(right_motor_reverse, OUTPUT);
  //initial start-up items

}

void loop() {
  right_motor(-100);
  //analogWrite(right_motor_forward, 255);
}

void right_motor(int duty){
  drive_motor(right_motor_forward, right_motor_reverse, MAX_RIGHT_MOTOR, MIN_RIGHT_MOTOR, duty);
}

void drive_motor(const int forward, const int reverse, const int max_m, const int min_m, int duty){
  if(duty > 100) duty = 100;
  if(duty < -100) duty = -100;
  delay(10);
  if(duty > 0){ //forwards
    analogWrite(forward, ((max_m - min_m)/100.0)*duty + min_m);
    analogWrite(reverse, 0);
  }else if(duty < 0){ //reverse
    analogWrite(forward, 0);
    analogWrite(reverse, ((max_m - min_m)/100.0)*(-duty) + min_m);
  }else{ //stopped
    analogWrite(forward, 0);
    analogWrite(reverse, 0);
  }
}


/*
 * returns an int that represents three numbers:
 * 1: To the left
 * 0: Center
 * -1
 */
float scan_line(){

}

