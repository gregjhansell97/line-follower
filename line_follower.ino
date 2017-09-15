#define MAX_RIGHT_MOTOR 178.0//about straight
#define MAX_LEFT_MOTOR 140.0
#define MIN_RIGHT_MOTOR 0.0 //need the point zero so its treated as a float
#define MIN_LEFT_MOTOR 0.0
#define MAX_PW 70

const int right_motor_forward = 3;
const int right_motor_reverse = 9;

const int left_motor_forward = 6;
const int left_motor_reverse = 5;

void setup() {
  // put your setup code here, to run once: 
  Serial.begin(9600);
  pinMode(right_motor_forward, OUTPUT);
  pinMode(right_motor_reverse, OUTPUT);
  //initial start-up items
}



void loop() {
  delay(10);
  set_direction(0);
  delay(10);
  hard_turn(1);
}

void straight(){
  right_motor(MAX_PW);
  left_motor(MAX_PW);
}

// o is straight
//5 is all the way right
//-5 is all the way left 
void set_direction(int change){
  if(change < -5) change = -5;
  if(change > 5) change = 5;

  //right handling:
  int right_pw = MAX_PW - (MAX_PW/5.0)*change;
  if(change < 0) right_pw = MAX_PW;

  //left handling:
  int left_pw  = MAX_PW + (MAX_PW/5.0)*change;
  if(change > 0) left_pw = MAX_PW; 

  right_motor(right_pw);
  left_motor(left_pw);
}

//-1 is left
//1 is right
//ignored otherwise
void hard_turn(int change){
  if(change == 1){ //sharp right
    right_motor(-MAX_PW/4.0);
    left_motor(MAX_PW/4.0);  
  }else if(change == -1){
    right_motor(MAX_PW/4.0);
    left_motor(-MAX_PW/4.0);
  }
}

void stop(){
  right_motor(0);
  left_motor(0);
}

void right_motor(int duty){
  drive_motor(right_motor_forward, right_motor_reverse, MAX_RIGHT_MOTOR, MIN_RIGHT_MOTOR, duty);
}

void left_motor(int duty){
  drive_motor(left_motor_forward, left_motor_reverse, MAX_LEFT_MOTOR, MIN_LEFT_MOTOR, duty);
}

void drive_motor(const int forward, const int reverse, const int max_m, const int min_m, int duty){
  if(duty > 100) duty = 100;
  if(duty < -100) duty = -100;
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

