 //Open Loop Beggining 
  
  #include <Servo.h>
  Servo steeringServo;
  
  #define solenoidPin           2
  #define pinservo            6
  #define pinreed            3
  
  
  // How many fires before turning starts, Degrees to turn servo, Fires while turning 
  #define forwardFires         1
  #define degreeTurnServo       110
  #define turnFires            7
  
  long intervalPistonOff = 700;
  long intervalPistonOn = 300;
  
  int pistonState = 0;      // 0: retracted, 1: fired
         
  unsigned long previousMillis = 0;        // will store last time solenoid was updated
  unsigned long currentMillis = millis();

  int reedCount = 0;
  int preState = 0;
  float distanceTraveled = 0;
  float trackLength = 6;    // Length of track in inches 
  float radius = 1.5;        // Radius of wheel in inches
  int switchState = 0;       // Use an int because digitalRead returns HIGH or LOW
  int fireCounter = 0;
void setup() {
  Serial.begin(9600);

  
  pinMode(solenoidPin,OUTPUT);
  pinMode(pinservo,OUTPUT);
  pinMode(pinreed,INPUT_PULLUP);
  
  steeringServo.attach(pinservo);
  steeringServo.write(90); //Make servo straight

  
   
   
   
  
   
   unsigned long currentMillis = millis();
    
      
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
  

  
//START

void loop() {
  

}





//  FUNCTION: if the solenoid has been on/off for the set amount of time, and is not currently on/off, fire/retract 
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
