#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

#define LED 4
int counter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED, HIGH);
  Serial.println("LED is on");
  delay(1000);
  digitalWrite(LED, LOW);
  Serial.println("LED is off");
  delay(1000);
  counter++;
  if(counter == 5){
    while(1){}
  }
}