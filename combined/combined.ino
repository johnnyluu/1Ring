#include <TimeAlarms.h>
#include <Time.h>
#include <Adafruit_NeoPixel.h>
#include <Event.h>
#include <Timer.h>
#include <RTimer.h>
#include <SoftwareSerial.h>

RTimer timer(5);
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
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

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
//3 = notification
int displayMode = 0;

//currentReminder being shown
int currentReminder = 0;

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
unsigned long taskId = -1;
//Flag to check if a task will be removed
bool negTime = false;
bool recording =  false;
bool idSet =  false;
bool setColour = false;
uint32_t currentColour = 0;
unsigned long startTimeLeft = 0;
unsigned long endTimeLeft = 0;

//the values used for a notification
int previousMode = -1;
uint32_t notifyColour;
unsigned long startTime = 0;
//length in seconds
const int notifyLength = 3;

//Is that button down dawg?
boolean buttonDown = false;
//How long has that button been down to clown?
unsigned long downTimer = 0;
unsigned long buttonDownTime = 0;

boolean reminderBrowsing = false;
boolean touchDown = false;
unsigned long touchTimer = 0;
unsigned long touchDownTime = 0;
int lastPoint = -1;

void setup() {
  strip.begin();
  strip.setBrightness(10);
  /*strip.setPixelColor(12, hourColour);
  strip.setPixelColor(13, hourColour);
  strip.setPixelColor(14, hourColour);
  strip.setPixelColor(15, hourColour);*/
  strip.show(); // Initialize all pixels to 'off'
  
  //Sets softpot and button
  digitalWrite(softpotPin, HIGH);
  pinMode(buttonPin, INPUT_PULLUP);

  
  t.every(1000, tick);
  
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
  //Serial1.println("Hello, world?");
  
  setTime(12,30,00,01,01,2000);
  timeIsSet = true;
  //Serial.println(now());
  //Default reminder added
  timer.addReminder(now(), minColour, 30);
  timer.addReminder(now() + 1, minColour, 6000);
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
    handleMessage(command);
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
  

}

  // softpot
  softpotReading = (analogRead(softpotPin)/10) * 10;
  
  t.update();
   
  if(displayMode == 3){
    int duration = millis() - startTime;
    if(duration / 1000 > notifyLength){
      displayMode = previousMode;
      previousMode = -1;
      startTime = 0;
    }
    else{
      blinkDisplay(duration, notifyLength * 1000, 10, notifyColour, noColour);
    }
  }
  //Goes to setTask view once time is set, this will be changed later
  if(displayMode == 1){
    if(!idSet){
      taskId = now();
      idSet = true;
    }
    if(!setColour){
      setTask();
    }
    else{
      chooseColour();
    }
  }
  
  if(displayMode == 0){
    if(strip.getPixelColor(12) != noColour && timer.getNumberOfReminders() == 0){
      strip.setPixelColor(12, noColour);
      strip.show();
    }
    else if(strip.getPixelColor(12) != timer.getReminder(currentReminder).colour &&
      timer.getNumberOfReminders() != 0){
        strip.setPixelColor(12, timer.getReminder(currentReminder).colour);
        strip.show();
    }
    int point = softpotToStrip();
    if(point != -1){
      if(!touchDown){
        touchDownTime = millis();
      }
      if(!reminderBrowsing){
        touchDown = true;
        touchTimer = millis() - touchDownTime;
        if(touchTimer >= 1000 && !reminderBrowsing){
          //begin browsing
          reminderBrowsing = true;
          lastPoint = point;
        }
      }
      else{
        if((point > lastPoint && (point != 11 && lastPoint != 0)) || 
          (point == 0 && lastPoint == 11)){
          currentReminder ++;
          if(currentReminder > timer.getNumberOfReminders() -1){
            currentReminder = 0;
          }
        }
        else if((point < lastPoint && (point != 0 && lastPoint != 11)) || 
          (point == 11 && lastPoint == 0)){
          currentReminder --;  
          if(currentReminder < 0){
            currentReminder = timer.getNumberOfReminders() -1;
          }
        }
        
        if(point != lastPoint){
          lastPoint = point;
          if(timer.getNumberOfReminders() != 0){
            timeLeftToDisplay(timer.getReminder(currentReminder).startTimeLeft);
          }
          else{
            timeLeftToDisplay(0);
          }
        }
      }
    }
    else if(touchDown){
      touchDown = false;
      downTimer = 0;
      if(!reminderBrowsing){
        changeView();
        setTask();
      }
      reminderBrowsing = false;
    }
    else if(digitalRead(buttonPin) == LOW){
      if(!buttonDown){
        buttonDownTime = millis();
      }
      buttonDown = true;
      downTimer = millis() - buttonDownTime;
      if(downTimer >= 1000){
        deleteDisplay(downTimer - 1000, 2000, hourColour);
      }
      
    }
    else if(buttonDown){
      buttonDown = false;
      if(downTimer > 3000){
       sendMessage("*deletereminder;" + String(timer.getReminder(currentReminder).id) + "#");
       timer.removeReminder(currentReminder);
      }
      else if(downTimer < 1000){
        sendMessage("*playstart;" + String(timer.getReminder(currentReminder).id) + "#");
      }
      downTimer = 0;
      if(timer.getNumberOfReminders() != 0){
        timeLeftToDisplay(timer.getReminder(currentReminder).startTimeLeft);
      }
      else{
        timeLeftToDisplay(0);
      }
    }
  }
  
  for(int i = 1; i < 4; i ++){
    int reminderPos = 0;
    if(i >= timer.getNumberOfReminders()){
      if(strip.getPixelColor(12 + i) != noColour){
        strip.setPixelColor(12 + i, noColour);
        strip.show();
      }
    }
    else if(currentReminder + i >= timer.getNumberOfReminders()){
      reminderPos = timer.getNumberOfReminders() - (currentReminder + i);
      uint32_t c = timer.getReminder(reminderPos).colour;
      if(strip.getPixelColor(12 + i ) != c){
        strip.setPixelColor(12 + i, c);
        strip.show();
      }
    }
    else{
      reminderPos = currentReminder + i;
      uint32_t c = timer.getReminder(reminderPos).colour;
      if(strip.getPixelColor(12 + i ) != c){
        strip.setPixelColor(12 + i, c);
        strip.show();
      }
    }
  }
  
}

//Gets time left in seconds and displays the amount of time left
void timeLeftToDisplay(int s){
  int hrsLeft = s/3600;
  int minsLeft = (s - hrsLeft * 3600)/300;
  int fineMinsLeft = ((s - hrsLeft * 3600) - minsLeft * 300) / 60;
  
  for(int i = 0; i < 12; i++){
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
          default:
            if(strip.getPixelColor(i) != noColour){
              strip.setPixelColor(i, noColour);
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
          default:
            if(strip.getPixelColor(i) != noColour){
              strip.setPixelColor(i, noColour);
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
    int notify = timer.timerTick();
    if(notify != -1){
      
      notification(displayMode, timer.getReminder(notify).colour);
    }
    if(displayMode == 0 && !buttonDown){
      if(timer.getNumberOfReminders() != 0){
        timeLeftToDisplay(timer.getReminder(currentReminder).startTimeLeft);
      }
      else{
        timeLeftToDisplay(0);
      }
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
  //Serial.println(touchPoint);
  
  int hours = hour() - 1;
  if(hours >= 11){
    hours = hours-11;
  }
  if(hours == 11){
    hours = 0;
  }
  
    
  if(touchPoint != -1){
    
    if(touchPoint == hours && selected ==0 ){
      startHour = touchPoint;
      selected = 1;
      return;
    }
    else if(touchPoint == minute()/5  && selected == 0){
      startMin = touchPoint;
      selected = 2;
      return;
    }
    else if((touchPoint == startHour || touchPoint == endHour) && selected == 0){
      
      selected = 3;
      return;
    }
    else if((touchPoint == startMin || touchPoint == endMin) && selected == 0){
      selected = 4;
      return;
    }
    
    switch (selected){
      case 1:
        if(startHour == hours && startHour == 11 && touchPoint == 0){
          startHour = touchPoint;
        }
        else if(startHour == hours && startHour == 0 && touchPoint == 11){
          negTime = true;
        }
        else if(startHour == hours && touchPoint < startHour){
          negTime = true;
        }
        else{
          startHour = touchPoint;
        }
        break;
        
      case 2:
        if(startMin == minute()/5 && startMin == 11 && touchPoint == 0){
          startMin = touchPoint;
        }
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
          endHour = -1;
          
          
          if(startHour == hours && startHour == 11 && touchPoint == 0){
          startHour = touchPoint;
          }
          else if(startHour == hours && startHour == 0 && touchPoint == 11){
            negTime = true;
          }
          else if(startHour == hours && touchPoint < startHour){
            negTime = true;
          }
          else{
            startHour = touchPoint;
          }
        }
        else{
          endHour = touchPoint;
        }
        break;
        
      case 4:
        if(touchPoint < startMin){
          selected = 2;
          endMin = -1;
          
          if(startMin == minute()/5 && startMin == 11 && touchPoint == 0){
          startMin = touchPoint;
          }
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
        }
        else{
          endMin = touchPoint;
        }
        break;
       
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
      if(!buttonDown){
        buttonDownTime = millis();
        buttonDown = true;
      }
      
      downTimer = millis() - buttonDownTime;
      if(downTimer > 500 && !recording){
        sendMessage("*recordstart;" + String(taskId) + "#");
        recording = true;
        for(int i = 0; i < 12; i ++){
          if(strip.getPixelColor(i) != minColour1){
            strip.setPixelColor(i, minColour1);
            strip.show();
          }
        }
      }
    }
    //Button released
    else if(buttonDown){
      buttonDown = false;
      buttonDownTime = 0;
      if(downTimer > 500){
        sendMessage("*recordend#");
        recording = false;
      }
      else{
      
        if(startHour < hours && startHour != -1){
          startTimeLeft += (12 - (hours - startHour)) * 3600;
        }
        else if(startHour != -1){
          startTimeLeft += (startHour - hours) * 3600;
        }
        if(startMin < minute()/5 && startMin != -1){
          startTimeLeft += (12 - (minute()/5 - startMin)) * 300;
        }
        else if(startMin != -1){
          startTimeLeft += (startMin - minute()/5) * 300;
        }
        
        if(endHour != -1){
            if(endHour < hours){
              endTimeLeft += (12 - (hours - endHour)) * 3600;
            }
            else{
              endTimeLeft += (endHour - hours) * 3600;
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
        if(startTimeLeft > 0){
          setColour = true;
        }
        else{
          cancelSetReminder();
        }
        
        startHour = -1;
        startMin = -1;
        endHour = -1;
        endMin = -1;
        selected = 0;
        negTime = false;
        buttonDown = false;
        buttonDownTime = 0;
        downTimer = 0;
        
      }
    
    }
  }
  
  //Displays the task being set
  if(!recording){
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
  
  if(strip.getPixelColor(12) != noColour){
    strip.setPixelColor(12, noColour);
    strip.show();
    
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
  taskId = -1;
  idSet = false;
  buttonDownTime = 0;
  downTimer = 0;
}

void chooseColour(){
  int point = softpotToStrip();
  if(point != -1){
    currentColour = Wheel(point * 21);
  }
  if(digitalRead(buttonPin) == LOW){
      if(!buttonDown){
        buttonDown = true;
      }
      
  }
  else if(buttonDown){
    buttonDown = false;
    if(currentColour != 0){
      if(endTimeLeft > 0){
        currentReminder = timer.addReminder(taskId, currentColour, startTimeLeft, endTimeLeft);
        sendMessage("*addreminder;" + String(taskId) + ";" + String(currentColour) + ";" +
        String(startTimeLeft) + ";" + String(endTimeLeft) = "#");
      }
      else{
        currentReminder = timer.addReminder(taskId, currentColour, startTimeLeft);
        sendMessage("*addreminder;" + String(taskId) + ";" + String(currentColour) + ";" +
        String(startTimeLeft) + ";-1#");
      }
      startTimeLeft = 0;
      endTimeLeft = 0;
      displayMode = 0;
      setColour = false;
      currentReminder = 0;
      taskId = -1;
      idSet = false;
    }
  }
  for(int i = 0; i < 12; i++){
    if(strip.getPixelColor(i) != Wheel(i*21)){
      strip.setPixelColor(i, Wheel(i*21));
      strip.show();
    }
  }
  
  if(strip.getPixelColor(12) != currentColour){
    strip.setPixelColor(12, currentColour);
    strip.show();
  }
}

void deleteDisplay(unsigned long time, unsigned long length, uint32_t colour){
  if(time < length){
    int progress = 0;
    progress = time/(length /12);;
    for(int i = 0; i < 12; i++){
      if(i <= progress){
        if(strip.getPixelColor(i) != colour){
          strip.setPixelColor(i, colour);
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
  else{
    blinkDisplay(time, length, 6, hourColour, noColour);
  }
}

void notification(int dMode, uint32_t colour){
  displayMode = 3;
  previousMode = dMode;
  notifyColour = colour;
  startTime = millis();
}

void blinkDisplay(unsigned long time, unsigned long length, int frequency, 
   uint32_t colour1, uint32_t colour2){
  uint32_t colour;
  if(frequency < 1){
    colour = colour1;
  }
  else if((time/(length/frequency)) % 2){
    colour = colour2;
  }
  else{
    colour = colour1;
  }
  
  for(int i = 0; i < 12; i++){
    if(strip.getPixelColor(i) != colour){
      strip.setPixelColor(i, colour);
      strip.show();
    }
  }
}

//Converts softpotReading to a strip position (0-11)
//Returns -1 for nothing being touched
int softpotToStrip(){
  if(softpotReading > 750){
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
        if(lastReading == 0 || lastReading == 11){
          return lastReading;
        }
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

void sendMessage(String s){
  char message[64];
  s.toCharArray(message, 64);
  Serial1.print(message);
  
}

void handleMessage(String s){
    char* parts[7];
    int count = 0;
    char byteMessage[64];
    s.toCharArray(byteMessage, 64);
    char* part =  strtok(byteMessage, ";");
    while(part != NULL){
      parts[count] = part;
      count ++;
      part = strtok(NULL, ";");
    }
    if(strncmp(parts[0], "time", 4) == 0){
      setTime(atoi(parts[1]), atoi(parts[2]), atoi(parts[3]), atoi(parts[4]),
        atoi(parts[5]), atoi(parts[6]));
      Serial.println("Time set");
      Serial.println(now());
    }
    else if(strncmp(parts[0], "newreminder", 11) == 0){
      timer.addReminder(atoi(parts[1]), atoi(parts[2]), atoi(parts[3]), 
      atoi(parts[4]));
      Serial.println("Reminder added");
    }
    else if(strncmp(parts[0], "deletereminder", 14) == 0){
      
    }
    
    Serial.println((int) parts[0][5]);
    //command = "time synced";
    //timeIsSet = true;
}

