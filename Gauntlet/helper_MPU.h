//most of this is from : https://www.arduino.cc/reference/en/libraries/mpu6050/
#ifndef HELPER_MPU
#define HELPER_MPU
//--------------------------------------------------------------------
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <helper_DATA.h>
//--------------------------------------------------------------------
Adafruit_MPU6050 mpu;
//--------------------------------------------------------------------
// our threshold for tilting the accelerometer to find dir
float accelThresh = 6;    //acc threshold in m/s/s

//--------------------------------------------------------------------
// setup MPU accel/gyro
//--------------------------------------------------------------------
void setupMPU(void) {
  delay(1000);
  while (!Serial)
    delay(10);            // will wait until serial console opens

  //accelerometer readings from Adafruit MPU6050
  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);    /// 2, 4, 8, 16 G
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.println("");
  delay(100);
}

//--------------------------------------------------------------------
// read the sensor and pick a direction
//--------------------------------------------------------------------
void getDataMPU(struct_message_t& myData) {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  //set dir based on acceleration
  if (a.acceleration.x > accelThresh)
    dir = walkLeft;
  else if (a.acceleration.x < -accelThresh)
    dir = walkRight;
  else if (a.acceleration.y < -accelThresh)
    dir = walkForward;
  else if (a.acceleration.y > accelThresh)
    dir = walkBackward;
  else
    dir = walkIdle;

  //store data into struct
  myData.ax = a.acceleration.x;
  myData.ay = a.acceleration.y;
  myData.walkDir = dir;
}
//--------------------------------------------------------------------
#endif //HELPER_MPU