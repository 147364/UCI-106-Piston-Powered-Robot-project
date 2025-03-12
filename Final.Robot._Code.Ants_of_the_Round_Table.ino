    // tell it what libraries to include (accelerameter and magnetometer servo)
    #include <Wire.h>
    #include <LIS3MDL.h>
    #include <LSM6.h> 
    #include <Servo.h>

  
      ///     PINS    ///
  #define solenoidPin           7
  #define pinServo              6
  #define pinReed               5

    //INPUT callibration vallues
    LIS3MDL::vector<int16_t> m_min = {-32767, -32767, -32767};
    LIS3MDL::vector<int16_t> m_max = {+32767, +32767, +32767};


    LIS3MDL mag;
    LSM6 imu;
    Servo steeringServo;
    
  
////////////////////////////    ADJUSTABLE VARIABLES   ///////////////////////////////////////////////////////
       
long intervalPistonOff = 700;
long intervalPistonOn = 700;


        //////// Adjustable, OPEN LOOP  //////////

     // How many fires before turning starts, Degrees to turn servo, Fires while turning 
  #define forwardFires         3
  #define degreeTurnServo       110
  #define turnFires            3

       ////////////////    Adjustable CLOSED LOOP ////////////

    float targetHeading = 90.0;       // Desired direction (adjustable)
    float adjustment = 20;        // Servo degrees turned to correct steering   
    float maxError = 5;               // Error before correction starts

       
//////////////////      FIXED  VARIABLES     //////////////////////////////////////////////////////////////////////
  
       ///// var for PISTON /// 
  unsigned long currentMillis = 0;
  unsigned long previousMillis = 0; 
  int pistonState = 0; // 0 is currently off 1 is currently on



      /// var for REED ///
  int reedCount = 0;
  int preState = 0; 
  float distanceTraveled = 0;
  float trackLength = 90;    // Length of track in inches 
  float radius = 1.5;        // Radius of wheel in inches
  int switchState = 0;       // Use an int because digitalRead returns HIGH or LOW

  int fireCounter = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  
  unsigned long currentMillis = millis();

  
  pinMode(solenoidPin,OUTPUT);
  pinMode(pinServo,OUTPUT);
  pinMode(pinReed,INPUT_PULLUP);
  
  steeringServo.attach(pinServo);
  steeringServo.write(90); //Make servo straight
  
/////////////   MAGNETOMETER INITIALIZING   ////////////////////////
    
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

////////////    OPEN LOOP RUNS      ///////////////
     
    while (fireCounter < forwardFires)
      {
        fire_piston();
      }

    fireCounter = 0;
    
    
    while (fireCounter < turnFires)
    {
      fire_piston();
       steeringServo.write(degreeTurnServo);
      
    }
    
    steeringServo.write(90);
    Serial.print ("Open loop ended. Closed loop initiating");
}
  
  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////START





void loop() {


  ////////////////   FIRE PISTON  /////////////////////////////////////////
  
  fire_piston();
  
//////////////////    REED SWITCH         /////////////////////////////////////////////////////////////////////////////////


                    // Compute distance traveled (verify if the factor is correct for your sensor)
            distanceTraveled = reedCount * radius * 3.14159265 / 2;
          
                     // Read the current state of the reed sensor
            switchState = digitalRead(pinReed);
            Serial.print("Switch State: ");
            Serial.println(switchState);
          
                     // Count the reed transitions (from HIGH to LOW)
          if (switchState == 0 && preState == 0) {
              reedCount++;
              preState = 1;
              delay(100);  
              }
        
                  // Reset the previous state when the switch goes HIGH
          if (switchState == 1 && preState == 1) {
              preState = 0;
              Serial.println("Reed switch released");
              delay(100);
          }
        
            Serial.print("Reed Count: ");
            Serial.println(reedCount);
            Serial.print("Distance Traveled: ");
            Serial.println(distanceTraveled);
          
          
                  // Halt program execution.
          if (distanceTraveled >= trackLength) {
               Serial.println("Reached end of track. Stopping.");
               while (1) {}
          }
        
       


//////////////////      CLOSED LOOP              /////////////////////////////////////////////// 

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









      ///   Function FIRE PISTON ///

      // if the solenoid has been on/off for the set amount of time, and is not currently on/off, fire/retract 

void fire_piston()
{
    currentMillis = millis();
         //piston On
  if ((currentMillis - previousMillis >= intervalPistonOff) && (pistonState == 0)) {
    ++fireCounter;
    previousMillis = currentMillis;
    digitalWrite(solenoidPin, HIGH);
    pistonState = 1;
    // Uncomment for debugging:
    Serial.print("Piston state: ");
    Serial.println("On");
  }
  
        //Piston off 
  else if ((currentMillis - previousMillis >= intervalPistonOn) && (pistonState == 1)) {
    previousMillis = currentMillis;
    digitalWrite(solenoidPin, LOW);
    pistonState = 0;
    // Uncomment for debugging:
    Serial.print("Piston state: ");
    Serial.println("Off");
  }
 
}    





      ///   Function HEADING MAGNETOMETER   /////

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
    
