/*
timeObserver.h

Plane object header file

timeObserver - What every object who needs to be updated by the clock needs to 
inherit publicly. It has a basic onTimeUpdate method which will be overridden
to allow each object to perform its own action when it is called. Additionally,
it has a Is_done variable for error catching, that only is set to true after
all steps in the on timeUpdate are completed.

Who makes this object? timeManager

Is it a time observer? Yes (duh)

What’s its goal? Be able to receive updates from timeManager, and perform and action everytime it gets one

*/
// TimeObserver.h
#ifndef TIME_OBSERVER_H
#define TIME_OBSERVER_H
#include "clock.h"

using namespace std;

class TimeObserver {
public:

    virtual void onTimeUpdate(Clock& new_time) = 0;

    virtual void updateDay(int Day) = 0;

    //Return whether observer got all the way through the update
    bool getIsDone() {
        return Is_done;
    }

    //Set update sucsess
    void setIsDone() {
        Is_done = true;
    }

    //Set update failure or not ready
    void setIsNotDone() {
        Is_done = false;
    }

    //Set whether observer succeeded update
    bool Is_done = false;
};

#endif // TIME_OBSERVER_H
