// boatProject.ino
// This program will be controlling the AI of the boat project.
// Started on Saturday October 23rd 2021


/*********************************************** INCLUDED LIBRARIES *************************************************/
/*********************************************** IR RECEIVER LIBRARY ************************************************/
#include <IRremote.h> // We are including the IR remote library so that we can include an IR remote to turn the motors on/off
/********************************************************************************************************************/
/*********************************************** SERVO LIBRARY *******************************************************/
#include <Servo.h> // We are including the Servo library to control the servo.
/*********************************************************************************************************************/
/************************************************ CONSTANTS *********************************************************/
/*********************************************** MOTOR CONSTANTS ***************************************************/
// B MOTOR
const int leftMotorPin = 13;
const int leftMotorSpeedPin = 11;
const int leftMotorBrakePin = 8;

// A MOTOR
const int rightMotorPin = 12;
const int rightMotorSpeedPin = 3;
const int rightMotorBrakePin = 9;

// Motor directional constants
const int forward = HIGH; // Moving anticlockwise
const int reverse = LOW; // Moving clockwise
/********************************************************************************************************************/
/*********************************************** IR RECEIVER CONSTANTS **********************************************/
const int irRecvPin = 7; //Might change later based on boat design decisions and spacial requirements.
IRrecv irrecv(irRecvPin);
decode_results results;
long keyCode = 0;
/*********************************************************************************************************************/
/*********************************************** SERVO CONSTANTS *****************************************************/
const int servoPin = 10; //Might change later based on boat design decisions and spacial requirements.
Servo radar; // Create a servo object to control our servo.

// Servo angle constants declaration and initialization
// We are doing this because we want accurate angles while the servo is moving as the servo does not actually have accurate coordinates
// 0 degrees = 180 // West
// 90 degrees = 90 // North
// 180 degrees = 0 // East
// 135 degrees = 45 // North_West
// 45 degrees = 135 // North_East
const int north = 90;
const int northWest = 135;
const int west = 180;
const int northEast = 45;
const int east = 0;
//const int coordinates[] = {north,northWest,west, northWest, north, northEast, east, northEast};
const int coordinates[] = {north,northWest, north, northEast};
/*********************************************************************************************************************/
/*********************************************** ULTRASONIC SENSOR CONSTANTS *****************************************/
// Recall that 12 inches == 1 foot.
// black wire = GND
// white wire = ECHO
// blue wire = TRIG
// green wire = VCC
const int echoPin = 4; //Might change later based on boat design decisions and spacial requirements.
const int trigPin = 5; //Might change later based on boat design decisions and spacial requirements.
/*********************************************************************************************************************/
/******************************************** GLOBAL VARIABLES *******************************************************/
float distance; // Distance to barrier - ULTRASONIC
unsigned long duration; // duration to receive echo - ULTRASONIC
bool isRunning = false; // isRunning is initialized to false because at this point the BOAT IS OFF.
bool isBarrier = false; // isBarrier will record whether there is a barrier
/*********************************************************************************************************************/

void setup() {
  // put your setup code here, to run once:
 
 
  /******************************************************** MOTOR *****************************************************/  
   // Initialize the pins for left Motor
  pinMode(leftMotorPin, OUTPUT);
  pinMode(leftMotorSpeedPin,OUTPUT);
  pinMode(leftMotorBrakePin,OUTPUT);

  // Intialize the pins for right motor
  pinMode(rightMotorPin, OUTPUT);
  pinMode(rightMotorSpeedPin,OUTPUT);
  pinMode(rightMotorBrakePin,OUTPUT);

 // stopMotors();
  /********************************************************************************************************************/

  /***************************************************** IR RECEIVER **************************************************/
  // Start the IR RECEIVER
  irrecv.enableIRIn();
 
  /********************************************************************************************************************/

  /***************************************************** SERVO ********************************************************/
  // Attach and initialize the Servo
  radar.attach(servoPin); // Attach the servo to servoPin 2
  radar.write(north); // Automatically set the ultrasonic sensor point to North on start.
 
  /********************************************************************************************************************/

  /**************************************************** ULTRASONIC SENSOR *********************************************/
  // Initialize the ultrasonic pins
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  /********************************************************************************************************************/
 
}

void loop() {
  irListener(); // Continously listening to the IR receiver
  if (isRunning == true){
    barrierDetection();
    radarMotion();
   
  }
 
}
/*************************************************************** MOTOR FUNCTIONS *******************************************************/
/**************************************************** A. SINGLE MOTOR FUNCTIONS **************************************************/
void leftMotorForward(){
  // This function will push the motors to their max speed of 255
  digitalWrite(leftMotorPin, forward);
  analogWrite(leftMotorSpeedPin, 255);
}
void rightMotorForward(){
  // This function will push the motors to their max speed of 255
  digitalWrite(rightMotorPin, forward);
  analogWrite(rightMotorSpeedPin, 255);
}
void leftMotorReverse(){
  digitalWrite(leftMotorPin, reverse);
  //digitalWrite(leftMotorBrakePin, LOW);
  analogWrite(leftMotorSpeedPin,255);
}
void rightMotorReverse(){
  digitalWrite(rightMotorPin, reverse);
  //igitalWrite(rightMotorBrakePin, LOW);
  analogWrite(rightMotorSpeedPin, 255);
}
void stopLeftMotor(){
  digitalWrite(leftMotorPin, forward);
  analogWrite(leftMotorSpeedPin, 0);
 
}
void stopRightMotor(){
  digitalWrite(rightMotorPin, forward);
  analogWrite(rightMotorSpeedPin, 0);
}

/****************************************************** B. DOUBLE MOTOR FUNCTIONS ************************************************/
void forwardMotion(){
  leftMotorForward();
  rightMotorForward();
}
//void brakeMotors(){}
void stopMotors(){
  stopLeftMotor();
  stopRightMotor();
}
void turnLeft(){

  leftMotorReverse();
  rightMotorForward();
}
void turnRight(){
  rightMotorReverse();
  leftMotorForward();
}
void stabilizeLeft(){
  stopRightMotor();
  leftMotorForward();
}
void stabilizeRight(){
  stopLeftMotor();
  leftMotorForward();
}
/***********************************************************************************************************************************/
/***************************************************** SERVO FUNCTIONS *****************************************************************/
// This function will control the servo as well while also reading barrier distance from the ultrasonic sensor.
// Will need adjusting during and after testing.
// Barrier distances need changing during testing.
// Note that the distance is in inches and NOT feet.
// I think
void barrierDetection(){
  irListener();
  if (radar.read() == north && getBarrierDistance() > 20){
    forwardMotion();
    isBarrier = false;
  }
  else if (radar.read() == north && getBarrierDistance() <= 20){
    stopMotors();
    isBarrier = true;
  }
 
}
void radarMotion(){
  long wBarrierDistance, eBarrierDistance;
  if (isBarrier == true){
   
    for (int i = 0; i <= 1; i++){
      if (i == 0){
        radar.write(west);
        delay(500);
        wBarrierDistance = getBarrierDistance();
      }
      else if (i==1){
        radar.write(east);
        delay(500);
        eBarrierDistance = getBarrierDistance();
        //directionChange(wBarrierDistance, eBarrierDistance);
      }
      //directionChange(wBarrierDistance, eBarrierDistance); ------>>> DEFINITELY NOT HERE ---------->>>>>
    }
    directionChange(wBarrierDistance, eBarrierDistance);
  }
  //directionChange(wBarrierDistance, eBarrierDistance); ------->>>> DEFINITELY NOT HERE -------->>>>>>
}
void directionChange(long wDistance, long eDistance){
  if (wDistance > eDistance){
    // turn Left
    turnLeft();
    delay(1000);
  }
  else if (wDistance <  eDistance){
    // turn right
    turnRight();
    delay(1000);
  }
  isBarrier = false;
  radar.write(north);
}
/*
void caseAngle(int angle){
  switch angle{
    case north:
    if (radar.read() == north && getBarrierDistance() > 12){
      forwardMotion();
    }
    else if (radar.read() == north && getBarrierDistance() <= 12){
      stopMotors();
    }
  }
}*/
/***************************************************************************************************************************************/

/*********************************************** ULTRASONIC SENSOR FUNCTIONS ***********************************************************/
// Returns the barrier distance in inches.
long getBarrierDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the distance to inches.
  distance = duration / 74 /2;
 
  return distance;
}
/***************************************************************************************************************************************/
/*********************************************** IR RECEIVER FUNCTIONS *****************************************************************/
// Map of the IR remote keys.
/*
 *  FF6897 0    
    FF30CF 1    
    FF18E7 2    
    FF7A85 3    
    FF10EF 4    
    FF38C7 5    
    FF5AA5 6    
    FF42BD 7      
    FF4AB5 8    
    FF52AD 9    
    FF906F ＋  
    FFA857 -    
    FFE01F EQ  
    FFB04F U/SD
    FF9867 CYCLE
    FF22DD PLAY/PAUSE
    FF02FD FORWARD
    FFC23D BACKWARD
    FFA25D POWER
    FFE21D MUTE
    FF629D MODE
    FFFFFF DUPLICATE
 *
 */
// This function decodes the entered values for the IR receiver.
String decodeKeyValue(long result)
{
  switch(result){
    case 0xFF6897:
      return "0";
    case 0xFF30CF:
      return "1";
    case 0xFF18E7:
      return "2";
    case 0xFF7A85:
      return "3";
    case 0xFF10EF:
      return "4";
    case 0xFF38C7:
      return "5";
    case 0xFF5AA5:
      return "6";
    case 0xFF42BD:
      return "7";
    case 0xFF4AB5:
      return "8";
    case 0xFF52AD:
      return "9";
    case 0xFF906F:
      return "+";
    case 0xFFA857:
      return "-";
    case 0xFFE01F:
      return "EQ";
    case 0xFFB04F:
      return "U/SD";
    case 0xFF9867:
      return "CYCLE";        
    case 0xFF22DD:
      return "PLAY/PAUSE";  
    case 0xFF02FD:
      return "FORWARD";  
    case 0xFFC23D:
      return "BACKWARD";  
    case 0xFFA25D:
      return "POWER";  
    case 0xFFE21D:
      return "MUTE";  
    case 0xFF629D:
      return "MODE";      
    case 0xFFFFFFFF:
      return "ERROR";  
    default :
      return "ERROR";
    }
}
// This function combines both the toOn and toOff functions to always be listening to turn the functions on or off.
// Placed toOff first because I want the function to pick up on the toOff function first then go to the toOn function which is usually just run once.
void irListener(){
  if (irrecv.decode(&results)){
    keyCode = results.value;
    if (keyCode != 1)
    {
      switch(keyCode){
        case 0xFF30CF:
        toOff();
        break;

        case 0xFFA25D:
        toOn();
        break;
      }
      irrecv.resume();
    }
  }
 
}

// This function listens to instructions to turn ON the motors.
// Pressing the POWER button will turn on the motors.
void toOn(){
 
   // releaseBrakes();
    forwardMotion(); // The motors will start at their max speed of 255 as a normal boat will do.
    isRunning=true; // Set isRunning to show that the motors are running.
    if (radar.attached() == false){
     radar.attach(servoPin);
     radar.write(north);  
    }
   
}

// This function listens to instructions to turn OFF the motors.
// Pressing "0" will turn the motors OFF.
void toOff(){
 
   
    /**************** Before turning off place the servo to default position (if it is currently not) for next boot. ********************/
    stopMotors(); // All motors will be set to "0"
    isRunning = false; // set isRunning to false to show that the motors are off.
    radar.write(north); // Return servo to default position that is NORTH
   
    if (radar.attached() == true){
      radar.detach();
    }
 
}
/********************************************************************************************************************************************************************/
