int buzzer = 5;
int red = 4;
int green = 2;
int blue = 3;
int incomingByte;
String command;
int receivingC = 0;
int receivingR = 0;



// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(buzzer, OUTPUT); 
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  if (Serial.available()){
    incomingByte = Serial.read();
    
    
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
        digitalWrite(blue, HIGH);
        delay(100);
        digitalWrite(blue, LOW);
      } else if(receivingR == 1){ 
        Serial.println(command);
        receivingR = 0;      
        digitalWrite(red, LOW);
        digitalWrite(green, HIGH);
        delay(50);
        digitalWrite(green, LOW);
        delay(50);
        digitalWrite(green, HIGH);
        delay(50);
        digitalWrite(green, LOW);
        delay(50);
        digitalWrite(green, HIGH);
        delay(50);
        digitalWrite(green, LOW);
      } else {
        Serial.println(command);
        digitalWrite(blue, HIGH);
        delay(100);
        digitalWrite(blue, LOW);
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
      digitalWrite(green, LOW);
      Serial.write("5");
      delay(1000);               // wait for a second
      digitalWrite(red, LOW);  // turn the LED off by making the voltage LOW
      digitalWrite(green, HIGH);
      Serial.write("4");
      delay(1000);  // wait for a second
      digitalWrite(red, HIGH);
      digitalWrite(blue, HIGH);
      Serial.write("3");
      delay(1000);
      digitalWrite(green, LOW);
      Serial.write("2");
      delay(1000);
      noTone(buzzer);
      digitalWrite(blue, LOW);
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
      delay(275);tone(buzzer,380,100);
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
      delay(250);tone(buzzer,500,100);
      delay(150);tone(buzzer,760,100);
      delay(50);tone(buzzer,720,100);
      delay(75);tone(buzzer,680,100);
      delay(75);tone(buzzer,620,150);
      delay(150);tone(buzzer,650,150);
      delay(150);tone(buzzer,380,100);
      delay(75);tone(buzzer,430,100);
      delay(75);tone(buzzer,500,100);
      delay(150);tone(buzzer,430,100);
      delay(75);tone(buzzer,500,100);
      delay(50);tone(buzzer,570,100);
      delay(110);tone(buzzer,500,100);
      delay(150);tone(buzzer,760,100);
      delay(50);tone(buzzer,720,100);
      delay(75);tone(buzzer,680,100);
      delay(75);tone(buzzer,620,150);
      delay(150);tone(buzzer,650,200);
      delay(150);tone(buzzer,1020,80);
      delay(150);tone(buzzer,1020,80);
      delay(75);tone(buzzer,1020,80);
      delay(150);tone(buzzer,380,100);
      delay(150);tone(buzzer,500,100);
      delay(150);tone(buzzer,760,100);
      delay(50);tone(buzzer,720,100);
      delay(75);tone(buzzer,680,100);
      delay(75);tone(buzzer,620,150);
      delay(150);tone(buzzer,650,150);
      delay(150);tone(buzzer,380,100);
      delay(75);tone(buzzer,430,100);
      delay(75);tone(buzzer,500,100);
      delay(150);tone(buzzer,430,100);
      delay(75);tone(buzzer,500,100);
      delay(50);tone(buzzer,570,100);
      delay(110);tone(buzzer,500,100);
      delay(150);tone(buzzer,760,100);
      delay(50);tone(buzzer,720,100);
      delay(75);tone(buzzer,680,100);
      delay(75);tone(buzzer,620,150);
      delay(150);tone(buzzer,650,200);
      delay(150);tone(buzzer,1020,80);
      delay(150);tone(buzzer,1020,80);
      delay(75);tone(buzzer,1020,80);
      delay(150);tone(buzzer,380,100);
      delay(150);tone(buzzer,500,100);
      delay(150);tone(buzzer,760,100);
      delay(50);tone(buzzer,720,100);
      delay(75);tone(buzzer,680,100);
      delay(75);tone(buzzer,620,150);
      delay(150);tone(buzzer,650,150);
      delay(150);tone(buzzer,380,100);
      delay(75);tone(buzzer,430,100);
      delay(75);tone(buzzer,500,100);
      delay(150);tone(buzzer,430,100);
      delay(75);tone(buzzer,500,100);
      delay(50);tone(buzzer,570,100);
      delay(210);tone(buzzer,585,100);
      delay(275);tone(buzzer,550,100);
      delay(210);tone(buzzer,500,100);
      delay(180);tone(buzzer,380,100);
      delay(150);tone(buzzer,500,100);
      delay(150);tone(buzzer,500,100);
      delay(75);tone(buzzer,500,100);
      delay(150);tone(buzzer,500,60);
      delay(75);tone(buzzer,500,80);
      delay(150);tone(buzzer,500,60);
      delay(175);tone(buzzer,500,80);
      delay(75);tone(buzzer,580,80);
      delay(175);tone(buzzer,660,80);
      delay(75);tone(buzzer,500,80);
      delay(150);tone(buzzer,430,80);
      delay(75);tone(buzzer,380,80);
      delay(300);tone(buzzer,500,60);
      delay(75);tone(buzzer,500,80);
      delay(150);tone(buzzer,500,60);
      delay(175);tone(buzzer,500,80);
      delay(75);tone(buzzer,580,80);
      delay(75);tone(buzzer,660,80);
      delay(225);tone(buzzer,870,80);
      delay(162);tone(buzzer,760,80);
      delay(300);tone(buzzer,500,60);
      delay(75);tone(buzzer,500,80);
      delay(150);tone(buzzer,500,60);
      delay(175);tone(buzzer,500,80);
      delay(75);tone(buzzer,580,80);
      delay(175);tone(buzzer,660,80);
      delay(75);tone(buzzer,500,80);
      delay(150);tone(buzzer,430,80);
      delay(75);tone(buzzer,380,80);
      delay(300);tone(buzzer,660,100);
      delay(75);tone(buzzer,660,100);
      delay(150);tone(buzzer,660,100);
      delay(150);tone(buzzer,510,100);
      delay(50);tone(buzzer,660,100);
      delay(150);tone(buzzer,770,100);
      delay(225);tone(buzzer,380,100);
      digitalWrite(red, LOW);
      command = "Voice reminder recorded.";
    }
    
      
  }
  
  
    if (command == "send data"){
      Serial.println ("Sending data for testing...");
      delay(1000);
    }
  


}
