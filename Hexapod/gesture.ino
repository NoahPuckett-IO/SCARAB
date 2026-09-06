//***********************************************************************************
// my code to set what gestures do
//***********************************************************************************
Vector3 vFront;                        //vectors have 3 numbers, (coxa, femur, tibia)
Vector3 vMid;
Vector3 vBack;
Vector3 vZero = Vector3(90,90,90);     //(coxa, femur, tibia)

void stateA2B(const Vector3& Af, const Vector3& Am, const Vector3& Ab,   //initial state
              const Vector3& Bf, const Vector3& Bm, const Vector3& Bb,   //final state
              int N, int dt);
void stateAngleSymmetric(Vector3 vFront, Vector3 vMid, Vector3 vBack);

//---------------------------------------------------------
// Cat, bend down like a happy cat stretching
//---------------------------------------------------------
void gesture_Stretch() {
  stateZero();                         //position (0,0,0)
  delay(500);

  //set Cat position
  vFront = Vector3(20,20,160);         //(coxa, femur, tibia)
  vMid   = Vector3(90,0,10);
  vBack  = Vector3(150,10,20);

  int N = 100;                         // subdivisions
  int dt = 5;                          // small pause, milliseconds

  stateA2B(vZero,vZero,vZero, vFront,vMid,vBack, N,dt);
  delay(2000);                         //hold pose
  stateA2B(vFront,vMid,vBack, vZero,vZero,vZero, N,dt);
  delay(500);
}

//---------------------------------------------------------
// Flatten
//---------------------------------------------------------
void gesture_Flatten() {
  stateZero();
  delay(500);

  //set flat position
  vFront = Vector3(90,150,0);
  vMid   = Vector3(90,150,0);
  vBack  = Vector3(90,150,0);

  int N = 50;
  int dt = 5;

  stateA2B(vZero,vZero,vZero, vFront,vMid,vBack, N,dt);
  delay(2000);                         //hold pose

  //bend femurs to help get up
  Vector3 vFront2 = Vector3(90,150,150);
  Vector3 vMid2   = Vector3(90,150,150);
  Vector3 vBack2  = Vector3(90,150,150);

  stateA2B(vFront,vMid,vBack, vFront2,vMid2,vBack2, N,dt);
  delay(500);                          //to get up
  stateA2B(vFront2,vMid2,vBack2, vZero,vZero,vZero, N,dt);
  stateZero();
  delay(500);
}

//---------------------------------------------------------
// Hello, pick up and wave one leg, the coxa moves 20 to 90 degrees
//---------------------------------------------------------
void gesture_Hello() {
  float femurAngle = 170;
  float tibiaAngle = 0;

  stateZero();                         //start at standing
  delay(500);

  leg0.moveToAngle(Vector3 (90,femurAngle,tibiaAngle));  //wave leg0 back and forward
  delay(500);
  leg0.moveToAngle(Vector3 (20,femurAngle,tibiaAngle));
  delay(500);
  leg0.moveToAngle(Vector3 (90,femurAngle,tibiaAngle));
  delay(500);
  leg0.moveToAngle(Vector3 (20,femurAngle,tibiaAngle));
  delay(500);                          //hold pose

  stateZero();
  delay(500);
}

//---------------------------------------------------------
// Scared (poofed up) like a cat
//---------------------------------------------------------
void gesture_Scared() {
  stateZero();
  delay(500);

  //set Scared (poofed up) position
  vFront = Vector3(90,0,0);
  vMid   = Vector3(90,0,0);
  vBack  = Vector3(90,0,0);

  int N = 50;
  int dt = 5;

  stateA2B(vZero,vZero,vZero, vFront,vMid,vBack, N,dt);
  delay(2000);                         //hold pose
  stateA2B(vFront,vMid,vBack, vZero,vZero,vZero, N,dt);
  delay(500);
}

//---------------------------------------------------------
// stateAngleSymmetric
// helper function to make left and right side behave the same
// set leg angles with input Vector3 for front, mid, back  (coxa, femur, tibia)
//---------------------------------------------------------
void stateAngleSymmetric(Vector3 vFront, Vector3 vMid, Vector3 vBack) {
  leg0.moveToAngle(vFront);            //right side
  leg1.moveToAngle(vMid);
  leg2.moveToAngle(vBack);

  //left side moves are 180 - theta
  Vector3 vBFront = Vector3(180-vFront.x, 180-vFront.y, 180-vFront.z);
  Vector3 vBMid   = Vector3(180-vMid.x,   180-vMid.y,   180-vMid.z);
  Vector3 vBBack  = Vector3(180-vBack.x,  180-vBack.y,  180-vBack.z);

  leg3.moveToAngle(vBBack);            //left side back
  leg4.moveToAngle(vBMid);             //left side middle
  leg5.moveToAngle(vBFront);           //left side front
}

//---------------------------------------------------------
// walk from pose A to pose B in N steps
//---------------------------------------------------------
void stateA2B(const Vector3& Af, const Vector3& Am, const Vector3& Ab,   //initial state
              const Vector3& Bf, const Vector3& Bm, const Vector3& Bb,   //final state
              int N, int dt)
{
  Vector3 xf = Af;                     //starting front
  Vector3 xm = Am;                     //starting mid
  Vector3 xb = Ab;                     //starting back

  Vector3 dABf = (Bf-Af)/N;            //increment, step a small amount
  Vector3 dABm = (Bm-Am)/N;
  Vector3 dABb = (Bb-Ab)/N;

  for (int i=0; i<N; i++)
  {
    xf = xf+dABf;
    xm = xm+dABm;
    xb = xb+dABb;
    stateAngleSymmetric(xf,xm,xb);
    delay(dt);
  }
}
