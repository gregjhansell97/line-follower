/*
 * NOTE: This code was used for an arduino NANO, but should be able to be 
 * implemented on other arduinos that have four analog ports, and 4 PWM ports
 */


//motors (used for synchronization)
#define MAX_RIGHT_MOTOR 135.0//about straight
#define MAX_LEFT_MOTOR 145.0
#define MIN_RIGHT_MOTOR 95.0 //need the point zero so its treated as a float
#define MIN_LEFT_MOTOR 110.0
#define MAX_PW 100.0

//sensor array (used for synchronization)
#define WHITE_RR 796
#define WHITE_R 796
#define WHITE_L 775
#define WHITE_LL 788

#define BLACK_RR 227
#define BLACK_R 290
#define BLACK_L 100
#define BLACK_LL 141


//motors
const int right_motor_forward = 3;
const int right_motor_reverse = 9;
const int left_motor_forward = 6;
const int left_motor_reverse = 5;

//sensor array
const int RR_pin = A0;
const int R_pin = A1;
const int L_pin = A2;
const int LL_pin = A3;

const int RR_threshold = (WHITE_RR + BLACK_RR)/2.0;
const int R_threshold = (WHITE_R + BLACK_R)/2.0;
const int L_threshold = (WHITE_L + BLACK_L)/2.0;
const int LL_threshold = (WHITE_LL + BLACK_LL)/2;

//turning
bool favor_right; //did the car recently go left or right?

//============================================================================================================================================================================================================================================
/*
 * Testing functions:
 * The functions that don't ever get used but are used for testing.
 * Also synching up in different environments.
 */

/*
 * Put the light sensors on white paper and see the output they read, 
 * record it in the #define statements called WHITE_xx. Do the same thing
 * but with the black part and change the BLACK_xx.
 */
 void synch_light_sensor(){
  float valRR, valR, valL, valLL;
  valRR = valR = valL = valLL = 0;
  for(int i = 0; i < 500; i++){
    valRR += analogRead(RR_pin);
    valR += analogRead(R_pin);
    valL += analogRead(L_pin);
    valLL += analogRead(LL_pin); 
    delay(5);
  }

  valRR = valRR/500;
  valR = valR/500;
  valL = valL/500;
  valLL = valLL/500;
  
  Serial.print("RR:");
  Serial.print(valRR);
  Serial.print("\t|R:");
  Serial.print(valR);
  Serial.print("\t|L:");
  Serial.print(valL);
  Serial.print("\t|LL:");
  Serial.println(valLL);
}

/*
 * drives the car straight as slow as it can 
 */
void min_straight(){
  right_motor(1);
  left_motor(1);
}

/*
 * A visual representat of how the car is viewing
 * the line as it drive along
 */
void view_line(){
  if(analogRead(LL_pin) < LL_threshold){
    Serial.print("B");
  }else{
    Serial.print("W");
  }
  if(analogRead(L_pin) < L_threshold){
    Serial.print("B");
  }else{
    Serial.print("W");
  }
  if(analogRead(R_pin) < R_threshold){
    Serial.print("B");
  }else{
    Serial.print("W");
  }
  if(analogRead(RR_pin) < RR_threshold){
    Serial.println("B");
  }else{
    Serial.println("W");
  }
}
//============================================================================================================================================================================================================================================

/*
 * implementation functions directly working with setup, loop (just run)
 */
 
/*
 * At the very start what goes down
 */
void setup() {
  favor_right = true;
  delay(6000); //takes the car 6 seconds to start moving
  Serial.begin(9600);
}

/*
 * gets called on repeat
 */
void loop() {
  run();
}

void run(){
  /*
   * store the variables to save on function calls
   */
  bool RR_black = RR_is_black();
  bool R_black = R_is_black();
  bool L_black = L_is_black();
  bool LL_black = LL_is_black();
  bool RR_white = !RR_black;
  bool R_white = !R_black;
  bool L_white = !L_black;
  bool LL_white = !LL_black;
  
  if(RR_black){ //turn right
    hard_turn(1);
    favor_right = true; //going right
    while(RR_is_black()|| (L_is_white() && R_is_white())){
      //if the car ever encounters all black continue turning
      while(RR_is_black() && R_is_black() && L_is_black() && LL_is_black()) right();
    }
  }

  if(LL_black){ //turn left
    hard_turn(-1);
    favor_right = false; 
    while(LL_is_black() || (L_is_white() && R_is_white())){
      //if the car ever encounters all black continue turning
      while(RR_is_black() && R_is_black() && L_is_black() && LL_is_black()) left();
    }
  } 

  /*
   * This turn is used in desparate occasions when the car has lost the line
   * completly. It remembers it's last turn
   */
  if(LL_white && L_white && R_white && RR_white){ //emergency turn
    int turn_status = -1;
    if(favor_right) turn_status = 1;
    stop();
    delay(30);
    while(LL_is_white() && L_is_white() && R_is_white() && RR_is_white()) hard_turn(turn_status); 
    stop();
    delay(30);
  }
  if(LL_white && (L_black || R_black) && RR_white){ //go straight
    straight();
    favor_right = R_black;
    while(LL_is_white() && (L_is_black() || R_is_black()) && RR_is_white());
  }
  straight(); //get's it started moving but normally gets overriden
}
//============================================================================================================================================================================================================================================
/*
 * Helper functions that aren't used in the loop but are valuable
 * because other functions use them.
 */
 
void straight(){
  right_motor(MAX_PW);
  left_motor(MAX_PW);
}

void right(){
  set_direction(5000);
  favor_right = true;
}

void left(){
  set_direction(-5000);
  favor_right = false;
}

/*
 * A relic of older code but is still used for future code,
 * if you want to turn the car one can pick any number from 
 * -5000 to 5000 and turn it accordingly where:
 * 0 is straight
 * 5000 is all the way right (right motor stopped)
 * -5000 is all the way left (left motor stopped)
 */
// o is straight
//5000 is all the way right
//-5000 is all the way left 
void set_direction(int change){
  if(change < -5000) change = -5000;
  if(change > 5000) change = 5000;

  //right handling:
  int right_pw = MAX_PW - (MAX_PW/5000.0)*change;
  if(change < 0) right_pw = MAX_PW + (MAX_PW/700000.0)*change;

  //left handling:
  int left_pw  = MAX_PW + (MAX_PW/5000.0)*change;
  if(change > 0) left_pw = MAX_PW - (MAX_PW/700000.0)*change; 

  right_motor(right_pw);
  left_motor(left_pw);
}

/*
 * A hard turn spins the wheels in opposite
 * directions to make very sharp turns, it takes
 * in two possible values:
 * 1 and -1. If it's zero, the 
 * value is ignored, but other numbers
 * are assumed to either be 1 or -1
 */
void hard_turn(int change){
  if(change > 0) change = 1;
  if(change < 0) change = -1;
  
  if(change == 1){ //sharp right
    favor_right = true; 
    right_motor(-1.5*MAX_PW);
    left_motor(1.5*MAX_PW);  
  }else if(change == -1){ //left
    favor_right = false;
    right_motor(1.5*MAX_PW);
    left_motor(-1.5*MAX_PW);
  }
}

void stop(){
  right_motor(0);
  left_motor(0);
}


bool RR_is_black(){
  return analogRead(RR_pin) < RR_threshold;
}

bool R_is_black(){
  return analogRead(R_pin) < R_threshold;
}

bool L_is_black(){
  return analogRead(L_pin) < L_threshold;
}

bool LL_is_black(){
  return analogRead(LL_pin) < LL_threshold;
}

bool RR_is_white(){
  return !RR_is_black();
}
bool R_is_white(){
  return !R_is_black();
}  

bool L_is_white(){
  return !L_is_black();
}

bool LL_is_white(){
  return !LL_is_black();
}

//============================================================================================================================================================================================================================================

/*
 * The root of how the motors are driven, shouldn't be modified
 */
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


