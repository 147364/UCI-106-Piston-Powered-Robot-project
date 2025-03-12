#define solenoidPin           6

long intervalPistonOff = 700;
long intervalPistonOn = 700;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0; 
int pistonState = 0; // 0 is currently off 1 is currently on

void setup() {
  Serial.begin(9600);
 pinMode(solenoidPin,OUTPUT);
}


 
 void loop() {
   currentMillis = millis();
  // put your main code here, to run repeatedly:
  //piston On
  if ((currentMillis - previousMillis >= intervalPistonOff) && (pistonState == 0)) {
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
