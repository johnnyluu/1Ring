#ifndef RTimer_h
#define RTimer_h

#include "Arduino.h"

class RTimer
{
	public:
		RTimer(int notify);
		struct reminder
			{
				bool exception;
				int colour;
				long startTimeLeft;
				long endTimeLeft;
			};
		void addReminder(int _c, long _tl, long _etl = -1);
		void removeReminder(int _pos);
		reminder getReminder(int _pos);
		long timeToLong(int hours, int minutes, int seconds);
		void timerTick();
		int getNumberOfReminders();
	private:
		int remindersLength;
		int notifyTime;
		reminder reminders[10];
};

#endif