#include <TimeAlarms.h>
#include <Time.h>
#include <Adafruit_NeoPixel.h>
#include <Event.h>
#include <Timer.h>
#include <RTimer.h>
#include <SoftwareSerial.h>

RTimer timer(15);
Timer t;

#define PIN 6

// LED for testing.
int red = 12;

// Saves each incoming byte
int incomingByte;

// Saves command after * and before #
String command;

// Receiving command
int receivingC = 0;

// Receiving recording
int receivingR = 0;

// Setting time
int settingTime = 0;


// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);

//List of preset colours
uint32_t hourColour = strip.Color(255, 0, 0);

uint32_t minColour = strip.Color(0, 0, 255);
uint32_t minColour4 = strip.Color(63, 63, 255);
uint32_t minColour3 = strip.Color(130, 130, 255);
uint32_t minColour2 = strip.Color(163, 163, 255);
uint32_t minColour1 = strip.Color(255, 255, 255);

uint32_t bothColour = strip.Color(255, 0, 255);
uint32_t bothColour4 = strip.Color(255, 0, 205);
uint32_t bothColour3 = strip.Color(255, 0, 155);
uint32_t bothColour2 = strip.Color(255, 0, 105);
uint32_t bothColour1 = strip.Color(255, 0, 55);

uint32_t noColour = strip.Color(0, 0, 0);

//Define's the current "mode" of the device
//0 = timeLeft
//1 = currentTime
//2 = tasks
int displayMode = 1;

//String used to read serail messages
String inMessage = "";
//Whether the time has been set
bool timeIsSet = false;

//Pin and reading from softpot
int softpotPin = A9;
int softpotReading;

//Last clock position (0 - 11) from the softpot
int lastReading;

//Disregards first 5 values after being touched
int noiseBuffer = 0;
int bufferLimit = 5;

//Pin used by button
int buttonPin = 10;

//the values for a new task being added
int startHour = -1;
int startMin = -1;
int endHour = -1;
int endMin = -1;
int selected = 0; //1 = hours 2= minutes 3 = endhours 4 = endminutes
//Flag to check if a task will be removed
bool negTime = false;

void setup() {
  strip.begin();
  strip.setBrightness(8);
  strip.show(); // Initialize all pixels to 'off'
  
  //Sets softpot and button
  digitalWrite(softpotPin, HIGH);
  pinMode(buttonPin, INPUT_PULLUP);

  
  t.every(1000, tick);
  
  //Default reminder added
  timer.addReminder(4, 13805);
  
  // initialize the digital pin as an output.
  pinMode(red, OUTPUT); 
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for Leonardo only
//   }
  Serial.println("Goodnight moon!");

  // set the data rate for the bluetooth
  Serial1.begin(9600);
  Serial1.println("Hello, world?");
  
  setTime(11,30,00,01,01,2000);
  timeIsSet = true;
  //Serial.println(now());
}

void loop() {
  
    //send command to phone
  while (Serial.available()) {
    Serial1.write(Serial.read());
  }
  
  // receiving commands from phone
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
    } else if(command == "time synced"){
      settingTime = 0;
      Serial.println(command);
      digitalWrite(red, HIGH);
      delay(100);
      digitalWrite(red, LOW);
    }else {
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
  
  // start receiving reminder
  if (command == "new reminder"){
    digitalWrite(red, HIGH);
    command = "Setting new reminder...";
    receivingR = 1;
  }
  
  //sync time with phone
  if(command == "time;"){
    settingTime = 1;
    command = "";
  }
  
  // parse command and set time
  if (settingTime == 1 && command.length() == 19){
    int timeParts[6];
    int count = 0;
    char byteMessage[19];
    command.toCharArray(byteMessage, 19);
    char* part =  strtok(byteMessage, ";");
    while(part != NULL){
      timeParts[count] = atoi(part);
      count ++;
      part = strtok(NULL, ":");
    }
    setTime(timeParts[0], timeParts[1], timeParts[2], timeParts[3],
      timeParts[4], timeParts[5]);
    command = "time synced";
    //timeIsSet = true;
    Serial.println(now());
  }
//  if (command == "voice reminder"){
//    
//  //  countdown & play mario theme song
//    digitalWrite(red, HIGH);   // turn the LED on (HIGH is the voltage level)
//    Serial.write("5");
//    delay(1000);               // wait for a second
//    digitalWrite(red, LOW);  // turn the LED off by making the voltage LOW
//    Serial.write("4");
//    delay(1000);  // wait for a second
//    digitalWrite(red, HIGH);
//    Serial.write("3");
//    delay(1000);
//    digitalWrite(red, LOW);
//    Serial.write("2");
//    delay(1000);
//    noTone(buzzer);
//    digitalWrite(red, HIGH);
//    Serial.println("Super Mario!");
//    tone(buzzer,660,100);
//    delay(75);tone(buzzer,660,100);
//    delay(150);tone(buzzer,660,100);
//    delay(150);tone(buzzer,510,100);
//    delay(50);tone(buzzer,660,100);
//    delay(150);tone(buzzer,770,100);
//    delay(275);tone(buzzer,380,100);
//    delay(287);tone(buzzer,510,100);
//    delay(225);tone(buzzer,380,100);
//    delay(200);tone(buzzer,320,100);
//    delay(250);tone(buzzer,440,100);
//    delay(150);tone(buzzer,480,80);
//    delay(165);tone(buzzer,450,100);
//    delay(75);tone(buzzer,430,100);
//    delay(150);tone(buzzer,380,100);
//    delay(100);tone(buzzer,660,80);
//    delay(100);tone(buzzer,760,50);
//    delay(75);tone(buzzer,860,100);
//    delay(150);tone(buzzer,700,80);
//    delay(75);tone(buzzer,760,50);
//    delay(175);tone(buzzer,660,80);
//    delay(150);tone(buzzer,520,80);
//    delay(75);tone(buzzer,580,80);
//    delay(75);tone(buzzer,480,80);
//    delay(175);tone(buzzer,510,100);
//    digitalWrite(red, LOW);
//    command = "Voice reminder recorded.";
//  }
}

  // softpot
  softpotReading = (analogRead(softpotPin)/10) * 10;
  t.update();
 
  //Goes to setTask view once time is set, this will be changed later
  if(displayMode == 1){
    setTask();
  }
  
  if(displayMode == 0){
    if (detectSwitch()) {
      changeView();
      setTask();
    }
  }
  
}

//Gets time left in seconds and displays the amount of time left
void timeLeftToDisplay(int s){
  int hrsLeft = s/3600;
  int minsLeft = (s - hrsLeft * 3600)/300;
  int fineMinsLeft = ((s - hrsLeft * 3600) - minsLeft * 300) / 60;
  
  for(int i = 0; i < strip.numPixels(); i++){
    if(hrsLeft >= i + 1 && minsLeft >= i + 1){

      if(strip.getPixelColor(i) != bothColour){
        strip.setPixelColor(i, bothColour);
        strip.show();
        
      }
    }
    else if(hrsLeft >= i + 1){
      if(strip.getPixelColor(i) != hourColour){
        strip.setPixelColor(i, hourColour);
        strip.show();
      }
    }
    else if(minsLeft >= i + 1){
      if(strip.getPixelColor(i) != minColour){
        strip.setPixelColor(i, minColour);
        strip.show();
      }
    }
    else{
      if(minsLeft >= i && minsLeft < i + 1){
        if(hrsLeft >= i + 1){
          switch(fineMinsLeft){
          case 1:
            if(strip.getPixelColor(i) != bothColour1){
              strip.setPixelColor(i, bothColour1);
              strip.show();
            }
            break;
          case 2:
            if(strip.getPixelColor(i) != bothColour2){
              strip.setPixelColor(i, bothColour2);
              strip.show();
            }
            break;
          case 3:
            if(strip.getPixelColor(i) != bothColour3){
              strip.setPixelColor(i, bothColour3);
              strip.show();
            }
            break;
          case 4:
            if(strip.getPixelColor(i) != bothColour4){
              strip.setPixelColor(i, bothColour4);
              strip.show();
            }
            break;
          }
        }
        else{
          switch(fineMinsLeft){
          case 1:
            if(strip.getPixelColor(i) != minColour1){
              strip.setPixelColor(i, minColour1);
              strip.show();
            }
            break;
          case 2:
            if(strip.getPixelColor(i) != minColour2){
              strip.setPixelColor(i, minColour2);
              strip.show();
            }
            break;
          case 3:
            if(strip.getPixelColor(i) != minColour3){
              strip.setPixelColor(i, minColour3);
              strip.show();
            }
            break;
          case 4:
            if(strip.getPixelColor(i) != minColour4){
              strip.setPixelColor(i, minColour4);
              strip.show();
            }
            break;
          }
        }
      }
      else if(strip.getPixelColor(i) != noColour){
        strip.setPixelColor(i, noColour);
        strip.show();
      }
    }
    
  }
  
  
}

//Function that is called every second
void tick(){
  //yep
  if(timeIsSet){
    //Timeleft for each reminder counts down
    timer.timerTick();
    if(displayMode == 0){
      timeLeftToDisplay(timer.getReminder(timer.getNumberOfReminders()-1).startTimeLeft);
    }
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}


//Deprecated
//From a time, displays the clock face
void timeToDisplay(int hours, int minutes){
  int hr;
  
  if(hours >= 12){
    hr = hours-12;
  }
  else{
    hr = hours;
  }
  
  if(hr == 12){
    hr = 0;
  }
  
  for(int i = 0; i < 12; i++){
    if(hr == i && minutes/5 == i){
      if(strip.getPixelColor(i) != bothColour){
        strip.setPixelColor(i, bothColour);
        strip.show();
      }
    }
    else if(hr == i){
      if(strip.getPixelColor(i) != hourColour){
        strip.setPixelColor(i, hourColour);
        strip.show();
      }
    }
    else if(minutes/5 == i){
      if(strip.getPixelColor(i) != minColour){
        strip.setPixelColor(i, minColour);  
        strip.show();
      }
    }
    else{
      if(strip.getPixelColor(i) != noColour){
        strip.setPixelColor(i, noColour);
        strip.show();
      }
    }
  }
  
  
  
}

//Receives a time message in the format HH:MM:SS:DD:MM:YYYY and sets the current time
//void receiveTime(){
//  if(Serial.available() > 0){
//    int inByte = Serial.read();
//    if(inByte != 10){
//      char inChar = inByte;
//      inMessage += inChar;
//    }
//    else if(inByte == 10){
//      
//      int timeParts[6];
//      int count = 0;
//      char byteMessage[20];
//      inMessage.toCharArray(byteMessage, 20);
//      char* part =  strtok(byteMessage, ":");
//      while(part != NULL){
//        timeParts[count] = atoi(part);
//        count ++;
//        part = strtok(NULL, ":");
//      }
//      setTime(timeParts[0], timeParts[1], timeParts[2], timeParts[3],
//        timeParts[4], timeParts[5]);
//      inMessage="";
//      timeIsSet = true;
//      Serial.println(now());
//    }
//  }
//}

// Change to the next view
void changeView(){
  if (displayMode >=2){
    displayMode = 0;
  } else {
    displayMode += 1;
  }
}
  
// detect if pressing on hour, probably need to add endtime
boolean detectSwitch(){
  int touchPoint = softpotToStrip();
  if (touchPoint != hour() && touchPoint != hour()-12 && touchPoint != -1){
    return true;
  } else {
    return false;
  }
}

//Calculates behaviour and display when a new task is being set
void setTask(){
  //Basic Clock
  int touchPoint = softpotToStrip();
  Serial.println(touchPoint);
  
  int hours = hour();
  if(hours >= 12){
    hours = hours-12;
  }
  if(hours == 12){
    hours = 0;
  }
    
  if(touchPoint != -1){
    
    if(touchPoint == hours && startHour < 0 && selected ==0 ){
      startHour = touchPoint;
      selected = 1;
      return;
    }
    else if(touchPoint == minute()/5 && startMin < 0 && selected == 0){
      startMin = touchPoint;
      selected = 2;
      return;
    }
    else if(touchPoint == startHour && selected == 0){
      
      selected = 3;
      return;
    }
    else if(touchPoint == startMin && selected == 0){
      selected = 4;
      return;
    }
    
    switch (selected){
      case 1:
        if(startHour == hours && touchPoint < startHour){
          negTime = true;
        }
        else if(startHour == hours && startHour == 0 && touchPoint != 1){
          negTime = true;
        }
        else{
          startHour = touchPoint;
        }
        break;
        
      case 2:
        if(startMin == minute()/5 && touchPoint < startMin){
          negTime = true;
        }
        else if(startMin == minute()/5 && startMin == 0 && 
          touchPoint != 1){
          negTime = true;
        }
        else{
          startMin = touchPoint;
        }
        break;
        
      case 3:
        if(touchPoint < startHour){
          selected = 1;
          
          if(startHour == hours && touchPoint < startHour){
            negTime = true;
          }
          else if(startHour == hours && startHour == 0 && touchPoint != 1){
            negTime = true;
          }
        }
        else{
          endHour = touchPoint;
        }
        break;
        
      case 4:
        if(touchPoint < startMin){
          selected = 2;
          
          if(startMin == minute()/5 && touchPoint < startMin){
            negTime = true;
          }
          else if(startMin == minute()/5 && startMin == 0 && 
            touchPoint != 1){
            negTime = true;
          }
        }
        else{
          endMin = touchPoint;
        }
       
    }
    
    
    
  }
  
  else{
    if((selected == 1 || selected == 2) && negTime){
      cancelSetReminder();
      return;
    }
    negTime = false;
    selected = 0;
    
    if(digitalRead(buttonPin) == LOW){
      
      int startTimeLeft = 0;
      if(startHour < hour()){
        startTimeLeft += (12 - (hour() - startHour)) * 3600;
      }
      else{
        startTimeLeft += (startHour - hour()) * 3600;
      }
      if(startMin < minute()/5){
        startTimeLeft += (12 - (minute()/5 - startMin)) * 300;
      }
      else{
        startTimeLeft += (startMin - minute()/5) * 300;
      }
      
      int endTimeLeft = 0;
      if(endHour != -1){
          if(endHour < hour()){
            endTimeLeft += (12 - (hour() - endHour)) * 3600;
          }
          else{
            endTimeLeft += (endHour - hour()) * 3600;
          }
      }
      if(endMin != -1){
        if(endMin < minute()/5){
        endTimeLeft += (12 - (minute()/5 - endMin)) * 300;
        }
        else{
          endTimeLeft += (endMin - minute()/5) * 300;
        }
      }
      
      //Adds with default color for now
      if(endTimeLeft > 0){
        timer.addReminder(1, startTimeLeft, endTimeLeft);
      }
      else{
        timer.addReminder(1, startTimeLeft);
      }
      
      startHour = -1;
      startMin = -1;
      endHour = -1;
      endMin = -1;
      selected = 0;
      negTime = false;
      displayMode = 0;
      
      
      
      
      
    }
  }
  
  //Displays the task being set
  for(int i = 0; i < 12; i ++){
    if(i == startMin || i == minute()/5 || i == endMin){
      if(i == startHour || i == hours || i == endHour){
        if(strip.getPixelColor(i) != bothColour){
          strip.setPixelColor(i, bothColour);
          strip.show();
        }
      }
      else{
        if(strip.getPixelColor(i) != minColour){
          strip.setPixelColor(i, minColour);
          strip.show();
        }
      }
    }
    else if(i == startHour || i == hours || i == endHour){
      if(strip.getPixelColor(i) != hourColour){
        strip.setPixelColor(i, hourColour);
        strip.show();
      }
    }
    else{
      if(strip.getPixelColor(i) != noColour){
        strip.setPixelColor(i, noColour);
        strip.show();
      }
    }
  }
  
  
}

//Resets variables and goes back to timeleft view
void cancelSetReminder(){
  startHour = -1;
  startMin = -1;
  endHour = -1;
  endMin = -1;
  selected = 0;
  negTime = false;
  displayMode = 0;
}

//Converts softpotReading to a strip position (0-11)
//Returns -1 for nothing being touched
int softpotToStrip(){
  if((softpotReading > 500 && softpotReading < 600) || softpotReading > 750){
    lastReading = -1;
    noiseBuffer = 0;
    return -1;
  }
  else{
    if(noiseBuffer < bufferLimit){
      noiseBuffer ++;
    }
    else{
      if(softpotReading >=720 || softpotReading <= 460){
        lastReading = 0;
        return 0;
      }
      if(softpotReading == 710){
        lastReading = 11;
        return 11;
      }
      if(softpotReading == 690){
        lastReading = 10;
        return 10;
      }
      if(softpotReading == 680){
        lastReading = 9;
        return 9;
      }
      if(softpotReading == 650){
        lastReading = 8;
        return 8;
      }
      if(softpotReading == 630){
        lastReading = 7;
        return 7;
      }
      if(softpotReading == 610){
        lastReading = 6;
        return 6;
      }
      if(softpotReading == 580){
        lastReading = 5;
        return 5;
      }
      if(softpotReading == 550){
        lastReading = 4;
        return 4;
      }
      if(softpotReading == 530){
        lastReading = 3;
        return 3;
      }
      if(softpotReading == 510){
        lastReading = 2;
        return 2;
      }
      if(softpotReading == 480){
        lastReading = 1;
        return 1;
      }
    }
  
  }
  
  return lastReading;
}

