#ifndef RTimer_h
#define RTimer_h

#include "Arduino.h"

class RTimer
{
	public:
		RTimer(int notify);
		struct reminder
			{
				long id;
				bool exception;
				uint32_t colour;
				long startTimeLeft;
				long endTimeLeft;
			};
		int addReminder(long _id, uint32_t _c, long _tl, long _etl = -1);
		void removeReminder(int _pos);
		reminder getReminder(int _pos);
		long timeToLong(int hours, int minutes, int seconds);
		int timerTick();
		int getNumberOfReminders();
	private:
		int remindersLength;
		int notifyTime;
		reminder reminders[20];
};

#endif