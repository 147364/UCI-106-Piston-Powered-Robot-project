
  #define pinReed 5
  
  int reedCount = 0;
  int preState = 0;
  float distanceTraveled = 0;
  float trackLength = 90;    // Length of track in inches 
  float radius = 1.5;        // Radius of wheel in inches
  int switchState = 0;       // Use an int because digitalRead returns HIGH or LOW

void setup() {
  pinMode(pinReed, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
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
      delay(100);  // Basic debouncing delay
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

}
