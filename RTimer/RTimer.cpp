#include "Arduino.h"
#include "RTimer.h"

RTimer::RTimer(int notify)
{
	volatile reminder reminders[10];
	remindersLength = 0;
	notifyTime = notify;
}

void RTimer::addReminder(int _c, long _tl, long _etl)
{
	if(remindersLength >= 10){
		return;
	}
	if(_etl < 0){

		_etl = _tl;
	}

	reminders[remindersLength].exception = false;
	reminders[remindersLength].colour = _c;
	reminders[remindersLength].startTimeLeft = _tl;
	reminders[remindersLength].endTimeLeft = _etl;

	remindersLength++;

}

void RTimer::removeReminder(int _pos)
{
	for(int i = _pos; i < remindersLength; i ++){
		if(i < remindersLength - 1){
			reminders[i] = reminders[i + 1];
		}
	}
	remindersLength --;
}

RTimer::reminder RTimer::getReminder(int _pos){
	if(_pos >= remindersLength || _pos < 0){
		reminder r;
		r.exception = true;
		return r;
	}
	return reminders[_pos];
}

void RTimer::timerTick(){
	bool notify = false;
	for(int i = 0; i < remindersLength; i ++){
		if(reminders[i].startTimeLeft > notifyTime &&
			reminders[i].startTimeLeft - 1 <= notifyTime){
			notify = true;
		}
		reminders[i].startTimeLeft -= 1;
		if(reminders[i].startTimeLeft < 0){
			reminders[i].startTimeLeft = 0;
		}
	}

	//return notify;
}

long RTimer::timeToLong(int hours, int minutes, int seconds){
	return 3600 * hours + 60 * minutes + seconds;
}

int RTimer::getNumberOfReminders(){
	return remindersLength;
}