int softpotPin = A9;
int softpotReading;

int errorMargin = 0;
int dragDistance = 20;
int dumbDelay = 4;

int startValue = -1;
int lastValue = -1;
int dragDirection = 0;
int pendingDelay = 0;

void setup(){
  digitalWrite(softpotPin, HIGH);
  Serial.begin(9600);
}

void loop(){
  softpotReading = (analogRead(softpotPin)/10) * 10;
  Serial.println(softpotReading);
  delay(200);
}

int softpotDirection(){
  if(softpotReading < 750){
    if(lastValue == -1){
      lastValue = softpotReading;
      startValue = softpotReading;
      pendingDelay ++;
      return 0; 
    }
    
    if(pendingDelay < dumbDelay){
      pendingDelay ++;
      if(pendingDelay >= dumbDelay){
       lastValue = softpotReading;
       startValue = softpotReading; 
       return 0;
      }
    }
    else{
    
      int difference = softpotReading - lastValue;
      //Serial.println(softpotReading - startValue);
      //If has gone right
      if(difference > errorMargin){
        //Wasn't going in a direction before
        if(dragDirection == 0){
          dragDirection = 1;
        }
        //Has continued going right
        else if(dragDirection == 1){
          if(softpotReading - startValue > dragDistance){
            Serial.print("StartValue ");
            Serial.println(startValue);
            Serial.print("softpotReading ");
            Serial.println(softpotReading);
            Serial.println(1);
            Serial.println("");
            startValue = softpotReading;
            lastValue = softpotReading;
            return 1;
          }
        }
        //Gone left
        else{
          dragDirection = -1;
          startValue = lastValue;
        }
        
        lastValue = softpotReading;
        return 0;
      }
      
      else if(difference < -errorMargin){
        //Wasn't going in a direction before
        if(dragDirection == 0){
          dragDirection = -1;
        }
        //Has continued going left
        else if(dragDirection == -1){
          if(softpotReading - startValue < -dragDistance){
            Serial.print("StartValue ");
            Serial.println(startValue);
            Serial.print("softpotReading ");
            Serial.println(softpotReading);
            Serial.println(-1);
            Serial.println("");
            startValue = softpotReading;
            lastValue = softpotReading;
            
            return -1;
          }
        }
        //Gone right
        else{
          dragDirection = 1;
          startValue = lastValue;
        }
        
        lastValue = softpotReading;
        return 0;
      }
      
      return 0;
      
    }
    
    return 0;
  
  }
  
  startValue = -1;
  lastValue = -1;
  dragDirection = 0;
  pendingDelay = 0;
  return 0;
  
  
}
