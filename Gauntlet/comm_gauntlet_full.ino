#include <Wire.h>
#include <helperButton.h>
#include <helper_DATA.h>      // message struct to store data
#include <helper_MPU.h>       // functions for MPU accelerometer / gyro
#include <helper_COM_send.h>  // comm functions, also contains message struct
//--------------------------------------------------------------------
// compile with DOIT ESP32 DEVKIT V1
//--------------------------------------------------------------------

// Create a struct_message called myData
struct_message_t myData     = {0,0,walkForward,stateOFF};   //initial values
struct_message_t myDataPrev = {0,0,walkForward,stateOFF};

unsigned long timePrev = 0;
unsigned long dtBounce = 500;   //don't change states faster than 500ms
unsigned long dt;

//--------------------------------------------------------------------
// setup buttons
//--------------------------------------------------------------------
int inPin0 = 13;  int outPin0 = 2;
int inPin1 = 25;  int outPin1 = 23;
int inPin2 = 26;  int outPin2 = 19;
int inPin3 = 27;  int outPin3 = 18;
int inPin4 = 14;  int outPin4 = 5;
int inPin5 = 12;  int outPin5 = 15;

Button button0;   //keep track of which buttons do what
Button button1;
Button button2;
Button button3;
Button button4;
Button button5;

//--------------------------------------------------------------------
// setup
//--------------------------------------------------------------------
void setup(void) {
  Serial.begin(115200);
  setupMPU();
  delay(200);

  // Set device as a Wi-Fi Station
  setupCOM();      //ESPNOW, see helper_COM_send.h

  initializeButton(button0, inPin0, outPin0, 0);
  initializeButton(button1, inPin1, outPin1, 1);
  initializeButton(button2, inPin2, outPin2, 2);
  initializeButton(button3, inPin3, outPin3, 3);
  initializeButton(button4, inPin4, outPin4, 4);
  initializeButton(button5, inPin5, outPin5, 5);

  //helperButtons
  bootupButtons(button0, button1, button2, button3, button4, button5);
  bootupButtons(button0, button1, button2, button3, button4, button5);
  turnLEDoffAll(button0, button1, button2, button3, button4, button5);

  //force these off
  button0.state = LOW;
  myData.state  = stateOFF;
}

//--------------------------------------------------------------------
// main loop
//--------------------------------------------------------------------
void loop() {
  getDataAndSend();
  delay(250);
}

//--------------------------------------------------------------------
// handle getting data from MPU and send with ESPNOW
//--------------------------------------------------------------------
void getDataAndSend() {
  //get data from buttons, set the led on/off
  readDataAndSetLED(button0);
  readDataAndSetLED(button1);
  readDataAndSetLED(button2);
  readDataAndSetLED(button3);
  readDataAndSetLED(button4);
  readDataAndSetLED(button5);

  //get data from MPU accelerometer, make our data structure
  getDataMPU(myData);
  updateStateFromButtons(button0, button1, button2, button3, button4, button5, myData);

  // Send message via ESP-NOW
  dt = millis() - timePrev;
  if (!isEqual(myData,myDataPrev) && dt>dtBounce){   //only output if the state changed
    sendDataCOM(myData);
    timePrev = millis();
    myDataPrev.state   = myData.state;
    myDataPrev.walkDir = myData.walkDir;
  }
  printData(myData);
}

//--------------------------------------------------------------------
// cycle through the buttons, turn each on and off
//--------------------------------------------------------------------
void bootupButtons(Button& b0, Button& b1, Button& b2, Button& b3, Button& b4, Button& b5)
{
  int dt = 200;
  digitalWrite(b0.outPin, HIGH); delay(dt); digitalWrite(b0.outPin, LOW); delay(dt);
  digitalWrite(b1.outPin, HIGH); delay(dt); digitalWrite(b1.outPin, LOW); delay(dt);
  digitalWrite(b2.outPin, HIGH); delay(dt); digitalWrite(b2.outPin, LOW); delay(dt);
  digitalWrite(b3.outPin, HIGH); delay(dt); digitalWrite(b3.outPin, LOW); delay(dt);
  digitalWrite(b4.outPin, HIGH); delay(dt); digitalWrite(b4.outPin, LOW); delay(dt);
  digitalWrite(b5.outPin, HIGH); delay(dt); digitalWrite(b5.outPin, LOW); delay(dt);

  turnLEDoffAll(button0, button1, button2, button3, button4, button5);
}

//--------------------------------------------------------------------
// check each button and state, turn off the others if a new button is pressed
//--------------------------------------------------------------------
void updateStateFromButtons(Button& b0, Button& b1, Button& b2, Button& b3,
                            Button& b4, Button& b5, struct_message_t& myData)
{
  if (button0.state==HIGH && myData.state != stateON)
  { myData.state = stateON;
    turnLEDoff(b1);turnLEDoff(b2);turnLEDoff(b3);turnLEDoff(b4);turnLEDoff(b5);}

  if (button1.state==HIGH && myData.state != stateWALK)
  { myData.state = stateWALK;
    turnLEDoff(b0);turnLEDoff(b2);turnLEDoff(b3);turnLEDoff(b4);turnLEDoff(b5);}

  if (button2.state==HIGH && myData.state != stateSTAND)
  { myData.state = stateSTAND;
    turnLEDoff(b0);turnLEDoff(b1);turnLEDoff(b3);turnLEDoff(b4);turnLEDoff(b5);}

  if (button3.state==HIGH && myData.state != stateHELLO)
  { myData.state = stateHELLO;
    turnLEDoff(b0);turnLEDoff(b1);turnLEDoff(b2);turnLEDoff(b4);turnLEDoff(b5);}

  if (button4.state==HIGH && myData.state != stateSCARED)
  { myData.state = stateSCARED;
    turnLEDoff(b0);turnLEDoff(b1);turnLEDoff(b2);turnLEDoff(b3);turnLEDoff(b5);}

  if (button5.state==HIGH && myData.state != stateFLATTEN)
  { myData.state = stateFLATTEN;
    turnLEDoff(b0);turnLEDoff(b1);turnLEDoff(b2);turnLEDoff(b3);turnLEDoff(b4);}

  //if all lights off
  if ((button0.state==LOW) && (button1.state==LOW) && (button2.state==LOW) &&
      (button3.state==LOW) && (button4.state==LOW) && (button5.state==LOW) )
  { myData.state = stateOFF;
    turnLEDoff(b0);turnLEDoff(b1);turnLEDoff(b2);turnLEDoff(b3);turnLEDoff(b4);turnLEDoff(b5);}
}