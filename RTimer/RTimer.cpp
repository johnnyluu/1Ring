#include "Arduino.h"
#include "RTimer.h"

RTimer::RTimer(int notify)
{
	volatile reminder reminders[20];
	remindersLength = 0;
	notifyTime = notify;
}

int RTimer::addReminder(long _id, uint32_t _c, long _tl, long _etl)
{
	if(remindersLength >= 19){
		return 0;
	}
	if(_etl < 0){

		_etl = _tl;
	}

	for(int i = remindersLength; i >= 0; i --){
		if(reminders[i].startTimeLeft > _tl && i < 19){
			reminders[i + 1].id = reminders[i].id;
			reminders[i + 1].exception = reminders[i].exception;
			reminders[i + 1].colour = reminders[i].colour;
			reminders[i + 1].startTimeLeft = reminders[i].startTimeLeft;
			reminders[i + 1].endTimeLeft = reminders[i].endTimeLeft;
			if(i == 0){
				reminders[0].id = _id;
				reminders[0].exception = false;
				reminders[0].colour = _c;
				reminders[0].startTimeLeft = _tl;
				reminders[0].endTimeLeft = _etl;
				remindersLength++;
				return 0;
			}
		}
		else if(reminders[i].startTimeLeft == _tl &&
			reminders[i].endTimeLeft > _etl && i < 19){
			reminders[i + 1].id = reminders[i].id;
			reminders[i + 1].exception = reminders[i].exception;
			reminders[i + 1].colour = reminders[i].colour;
			reminders[i + 1].startTimeLeft = reminders[i].startTimeLeft;
			reminders[i + 1].endTimeLeft = reminders[i].endTimeLeft;
			if(i == 0){
				reminders[0].id = _id;
				reminders[0].exception = false;
				reminders[0].colour = _c;
				reminders[0].startTimeLeft = _tl;
				reminders[0].endTimeLeft = _etl;
				remindersLength++;
				return 0;
			}
		}
		else if(i == 0 && i == remindersLength){
			reminders[0].id = _id;
			reminders[0].exception = false;
			reminders[0].colour = _c;
			reminders[0].startTimeLeft = _tl;
			reminders[0].endTimeLeft = _etl;
			remindersLength++;
			return 0;
		}
		else if(reminders[i].startTimeLeft < _tl
			&& i < 19 && reminders[i].id != 0){
			reminders[i + 1].id = _id;
			reminders[i + 1].exception = false;
			reminders[i + 1].colour = _c;
			reminders[i + 1].startTimeLeft = _tl;
			reminders[i + 1].endTimeLeft = _etl;
			remindersLength++;
			return i + 1;
		}
	}

	return 0;

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

int RTimer::timerTick(){
	int notify = -1;
	for(int i = 0; i < remindersLength; i ++){
		if(reminders[i].startTimeLeft > notifyTime &&
			reminders[i].startTimeLeft - 1 <= notifyTime){
			notify = i;
		}
		reminders[i].startTimeLeft -= 1;
		if(reminders[i].startTimeLeft < 0){
			reminders[i].startTimeLeft = 0;
		}

		if(reminders[i].endTimeLeft > notifyTime &&
			reminders[i].endTimeLeft - 1 <= notifyTime){
			notify = i;
		}
		reminders[i].endTimeLeft -= 1;
		if(reminders[i].endTimeLeft < 0){
			reminders[i].endTimeLeft = 0;
		}
	}

	return notify;
}

long RTimer::timeToLong(int hours, int minutes, int seconds){
	return 3600 * hours + 60 * minutes + seconds;
}

int RTimer::getNumberOfReminders(){
	return remindersLength;
}