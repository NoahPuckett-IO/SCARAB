//***********************************************************************************
// my code to set initialize state
//***********************************************************************************
void state_Initialize() {
  Vector3 va( 90,  90,  90);           //first state
  Vector3 vb( 20, 150, 150);           //second pose

  stateAngle(va);
  delay(500);
  stateAngle(vb);                      //position vb
  delay(2000);
  stateAngle(va);                      //back to first state
  delay(500);
}

//***********************************************************************************
// helper functions
//***********************************************************************************
// move all legs to same angle
void stateAngle(Vector3 vA) {
  leg0.moveToAngle(vA);                //right side
  leg1.moveToAngle(vA);
  leg2.moveToAngle(vA);

  //left side moves are 180 - theta
  Vector3 vB = Vector3(180-vA.x, 180-vA.y, 180-vA.z);
  leg3.moveToAngle(vB);                //left side
  leg4.moveToAngle(vB);
  leg5.moveToAngle(vB);
}

// move to (x,y,z)=(0,0,0) for each leg
void stateZero() {
  Vector3 v0( 0, 0, 0);
  leg0.CartesianMove(v0);
  leg1.CartesianMove(v0);
  leg2.CartesianMove(v0);
  leg3.CartesianMove(v0);
  leg4.CartesianMove(v0);
  leg5.CartesianMove(v0);
}