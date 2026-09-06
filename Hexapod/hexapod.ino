//***********************************************************************************
// This is the main code which runs the hexapod
//
// Some hexapod code resources that I used below
//   Aecert robotics (gaits / state_Car / Helpers / Bezier / vectors)
//     https://github.com/Ryan-Mirch/Aecerts_Hexapod_V1
//   Inverse kinematics from JustAnotherMakerChannel
//     https://www.youtube.com/watch?v=HjmIOKSp7v4
//   Hexapod 3D frame and legs from MakeYourPet
//     https://github.com/MakeYourPet/hexapod
//
// Using libraries
//   ESPnow, connects two ESP32s together
//***********************************************************************************
#include <Arduino.h>
#include <math.h>
#include "servoHex.h"          //servo controllers
#include "Helpers.h"           //helper functions, Aecert
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <helper_DATA.h>
#include <helper_COM_recv.h>

//***********************************************************************************
// define constants
//***********************************************************************************
const int LED = 2;             //esp32 on board LED

//***********************************************************************************
// define
//***********************************************************************************
state_t currentState = stateOFF;

//***********************************************************************************
// configure stride
//***********************************************************************************
float strideLength = 200;              //x
float distanceStraddle = -10;          //y
float distanceFromGroundBase = 40;     //z, -z is higher, +z is foot extend below
float liftHeight = -80;                //z lift when stepping
float strideSpeed = 1;

//***********************************************************************************
// timing of the input
//***********************************************************************************
unsigned long timeSinceLastInput = 0;
long elapsedTime = 0;
long loopStartTime = 0;

//***********************************************************************************
// walk vector
//***********************************************************************************
Vector2 walkingVector = Vector2(50,0);   //forward/back, turn left/right
bool isConnected = 0;

//***********************************************************************************
// initialize legs      legNumber   Side    Position (on clock)
//***********************************************************************************
LEG leg0(0);   //initialize 0   Right    1:00
LEG leg1(1);   //initialize 1   Right    3:00
LEG leg2(2);   //initialize 2   Right    5:00
LEG leg3(3);   //initialize 3   Left     7:00
LEG leg4(4);   //initialize 4   Left     9:00
LEG leg5(5);   //initialize 5   Left    11:00

//***********************************************************************************
// unpower servo
//***********************************************************************************
void dePower(){
  for (int i = 0; i < 16; i++) {
    myServoL.setPWM(i, 0, 0);   //if not working try (idle, 4096, 0)
    myServoR.setPWM(i, 0, 0);   //if not working try (idle, 4096, 0)
  }
}

//***********************************************************************************
// setup
//***********************************************************************************
void setup() {
  //---------------------------------------------
  // setup ESP32 pins
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);       //LED OFF

  //---------------------------------------------
  // serial monitor beginning
  Serial.begin(115200);
  digitalWrite(LED,HIGH); delay(250); digitalWrite(LED,LOW); delay(250);
  digitalWrite(LED,HIGH); delay(250); digitalWrite(LED,LOW); delay(250);

  //---------------------------------------------
  // attach servo controllers
  myServoR.begin();                    //servos 0,1,2
  myServoR.setPWMFreq(SERVO_FREQ);     //SERVO_FREQ is set in servoHex.h
  myServoL.begin();                    //servos 3,4,5
  myServoL.setPWMFreq(SERVO_FREQ);

  //---------------------------------------------
  // initialize myData to off
  myData.state = stateOFF;
  myData.walkDir = walkForward;

  //---------------------------------------------
  // connect to gauntlet
  setupCOMrecv();

  //---------------------------------------------
  // controller is connected, setup is good
  digitalWrite(LED,HIGH);              //turn on ESP32 onboard LED
}

//***********************************************************************************
// loop, my main program. Get data from the Gauntlet to select a state or drive
//***********************************************************************************
void loop() {
  if (isConnected)
  {
    //--------------------------------------------------------------------
    // set direction  enum walkDir_t {Forward, Backward, Left, Right, Idle}
    //--------------------------------------------------------------------
    switch (myData.walkDir) {              //from helper_COM_recv
      case walkForward:  walkingVector=Vector2(80,0);  break;   //[x=(forward/back), y=(right/left)]
      case walkBackward: walkingVector=Vector2(-80,0); break;
      case walkLeft:     walkingVector=Vector2(0,80);  break;
      case walkRight:    walkingVector=Vector2(0,-80); break;
      case walkIdle:     walkingVector=Vector2(5, 0);  break;
    }

    //-------------------------------------------------------------------
    // pick the behaviour for the current state
    //-------------------------------------------------------------------
    switch (myData.state){
      case stateSTAND:
        state_Standing(); currentState = stateSTAND;
        break;
      case stateWALK:
        state_Car(); currentState = stateWALK;
        break;
      case stateFLATTEN:
        gesture_Flatten();
        myData.state = stateSTAND;      //go back to standing
        break;
      case stateHELLO:
        gesture_Hello();
        myData.state = stateSTAND;
        break;
      case stateSCARED:
        gesture_Scared();
        myData.state = stateSTAND;
        break;
      case stateON:
        dePower();
        delay(200);
        break;
    } //end switch
  }//end isConnected

  //---------------------------------------------
  // do nothing until the yellow button is pressed
  if (myData.state==stateON && !isConnected)
  {
    state_Initialize();
    delay(500);
    isConnected = 1;
    myData.state==stateSTAND;      //change to standing
  }
  return;
}

//***********************************************************************************
// helper functions
//***********************************************************************************
void resetMovementVectors(){
  walkingVector=Vector2(0,0);
}