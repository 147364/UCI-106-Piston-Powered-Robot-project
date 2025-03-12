   // tell it what to include (accelerameter and magnetometer)
    #include <Wire.h>
    #include <LIS3MDL.h>
    #include <LSM6.h>
    #include <Servo.h>
    
    Servo steeringServo;
    
    LIS3MDL mag;
    LSM6 imu;
    
    
    //INPUT callibration vallues
    LIS3MDL::vector<int16_t> m_min = {-32767, -32767, -32767};
    LIS3MDL::vector<int16_t> m_max = {+32767, +32767, +32767};
    
    const int pinservo = 6;           // Pin for servo
    
    float targetHeading = 90.0;       // Desired direction (adjustable)
    float adjustment = 10;              // Servo degrees turned to correct steering   
    float maxError = 5;               // Error before correction starts
    

void setup() {
    Serial.begin(9600);
    Wire.begin();
   
    steeringServo.attach(pinservo);

    //Initialize magnetometer     
      if (!mag.init())
    {
      Serial.println("Failed to detect and initialize LIS3MDL magnetometer!");
      while (1);
    }
    mag.enableDefault();
  
    if (!imu.init())
    {
      Serial.println("Failed to detect and initialize LSM6 IMU!");
      while (1);
    }
    imu.enableDefault();

}



void loop() {
  
    mag.read();
    imu.read();
  
    float currentHeading =  computeHeading();
    
    if (currentHeading < 0) currentHeading += 360; // Normalize heading 

    Serial.println(currentHeading);
 
    float error = currentHeading - targetHeading;   //Discrepancy between desired direction and actual
    
    if (error > maxError) {
        // going left, Turn Right
        steeringServo.write(90 - adjustment);
        Serial.println("Turn Right");
    } 
    else if (error < -maxError) {
        // going right, Turn Left
         steeringServo.write(90 + adjustment);
         Serial.println("Turn Left");
  
    } 
    else {
        // Stop Motor (Within range)
         steeringServo.write(90); // Make servo straight
         Serial.println("Go straight");

    }

    delay(50);  // Small delay for stability
}










//heading function for magnetometer

template <typename T> float computeHeading(LIS3MDL::vector<T> from)
{
  LIS3MDL::vector<int32_t> temp_m = {mag.m.x, mag.m.y, mag.m.z};

  // copy acceleration readings from LSM6::vector into an LIS3MDL::vector
  LIS3MDL::vector<int16_t> a = {imu.a.x, imu.a.y, imu.a.z};

  // subtract offset (average of min and max) from magnetometer readings
  temp_m.x -= ((int32_t)m_min.x + m_max.x) / 2;
  temp_m.y -= ((int32_t)m_min.y + m_max.y) / 2;
  temp_m.z -= ((int32_t)m_min.z + m_max.z) / 2;

  // compute E and N
  LIS3MDL::vector<float> E;
  LIS3MDL::vector<float> N;
  LIS3MDL::vector_cross(&temp_m, &a, &E);
  LIS3MDL::vector_normalize(&E);
  LIS3MDL::vector_cross(&a, &E, &N);
  LIS3MDL::vector_normalize(&N);

  // compute heading
  float heading = atan2(LIS3MDL::vector_dot(&E, &from), LIS3MDL::vector_dot(&N, &from)) * 180 / PI;
  if (heading < 0) heading += 360;
  return heading;
}

/*
Returns the angular difference in the horizontal plane between a
default vector (the +X axis) and north, in degrees.
*/
float computeHeading()
{
  return computeHeading((LIS3MDL::vector<int>){1, 0, 0});
}
