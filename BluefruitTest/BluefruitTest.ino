/*
 Software serial multple serial test

Receives from the hardware serial, sends to software serial.
Receives from software serial, sends to hardware serial.

The circuit: 
* RX is digital pin 10 (connect to TX of other device)
* TX is digital pin 11 (connect to RX of other device)

Note:
Not all pins on the Mega and Mega 2560 support change interrupts, 
so only the following can be used for RX: 
10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

Not all pins on the Leonardo support change interrupts, 
so only the following can be used for RX: 
8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

created back in the mists of time
modified 25 May 2012
by Tom Igoe
based on Mikal Hart's example

This example code is in the public domain.

*/
#include <SoftwareSerial.h>
int buzzer = 9;
int red = 6;
int incomingByte;
String command;
int receivingC = 0;
int receivingR = 0;

void setup()  
{
  // initialize the digital pin as an output.
  pinMode(red, OUTPUT);
  pinMode(buzzer, OUTPUT); 
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
   }
  Serial.println("Goodnight moon!");

  // set the data rate for the bluetooth
  Serial1.begin(9600);
  Serial1.println("Hello, world?");
}

void loop() // run over and over
{
  //send command to phone
  while (Serial.available()) {
    Serial1.write(Serial.read());
  }
  
  if (Serial1.available()){
  incomingByte = Serial1.read();
  
  
  //stop receiving command and print out received command after reading #
  if ((char)incomingByte == '#'){
    receivingC = 0;
    if (command == "Setting new reminder..."){
      Serial.println(command);
      command = "New reminder set: ";
    } else if(command == "New reminder set: cancel"){
      command = "New reminder cancelled.";
      Serial.println(command);
      receivingR = 0;
      digitalWrite(red, LOW);
      digitalWrite(red, HIGH);
      delay(100);
      digitalWrite(red, LOW);
    } else if(receivingR == 1){ 
      Serial.println(command);
      receivingR = 0;      
      digitalWrite(red, LOW);
      digitalWrite(red, HIGH);
      delay(50);
      digitalWrite(red, LOW);
      delay(50);
      digitalWrite(red, HIGH);
      delay(50);
      digitalWrite(red, LOW);
      delay(50);
      digitalWrite(red, HIGH);
      delay(50);
      digitalWrite(red, LOW);
    } else {
      Serial.println(command);
      digitalWrite(red, HIGH);
      delay(100);
      digitalWrite(red, LOW);
    }
      
  }
  
  
  //record command
  if (receivingC == 1){
    command += (char)incomingByte;
  }
  
  
  //clear command and start receiving command after reading *
  if((char)incomingByte == '*'){
    receivingC = 1;
    if(receivingR == 0){
      command = "";
      Serial.write("Receiving command...");
    }
  }
  if (command == "new reminder"){
    digitalWrite(red, HIGH);
    command = "Setting new reminder...";
    receivingR = 1;
  }
  if (command == "voice reminder"){
    
  //  countdown & play mario theme song
    digitalWrite(red, HIGH);   // turn the LED on (HIGH is the voltage level)
    Serial.write("5");
    delay(1000);               // wait for a second
    digitalWrite(red, LOW);  // turn the LED off by making the voltage LOW
    Serial.write("4");
    delay(1000);  // wait for a second
    digitalWrite(red, HIGH);
    Serial.write("3");
    delay(1000);
    digitalWrite(red, LOW);
    Serial.write("2");
    delay(1000);
    noTone(buzzer);
    digitalWrite(red, HIGH);
    Serial.println("Super Mario!");
    tone(buzzer,660,100);
    delay(75);tone(buzzer,660,100);
    delay(150);tone(buzzer,660,100);
    delay(150);tone(buzzer,510,100);
    delay(50);tone(buzzer,660,100);
    delay(150);tone(buzzer,770,100);
    delay(275);tone(buzzer,380,100);
    delay(287);tone(buzzer,510,100);
    delay(225);tone(buzzer,380,100);
    delay(200);tone(buzzer,320,100);
    delay(250);tone(buzzer,440,100);
    delay(150);tone(buzzer,480,80);
    delay(165);tone(buzzer,450,100);
    delay(75);tone(buzzer,430,100);
    delay(150);tone(buzzer,380,100);
    delay(100);tone(buzzer,660,80);
    delay(100);tone(buzzer,760,50);
    delay(75);tone(buzzer,860,100);
    delay(150);tone(buzzer,700,80);
    delay(75);tone(buzzer,760,50);
    delay(175);tone(buzzer,660,80);
    delay(150);tone(buzzer,520,80);
    delay(75);tone(buzzer,580,80);
    delay(75);tone(buzzer,480,80);
    delay(175);tone(buzzer,510,100);
    digitalWrite(red, LOW);
    command = "Voice reminder recorded.";
  }
}
}
