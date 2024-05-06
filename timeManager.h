/*
timeManager.h

Time manager header

timeManager - Controls the time of the simulation, pushes changes in the time 
to other objects. Hold a vector of all observers, or objects that will be
affected by the time update. This will be the only object that actually
“progresses time”, so to speak to avoid inconsistencies.

Who makes this object? Main

What’s its goal? Hold a vector of all observers, and send out updates of the new time.

*/
// timeManager.h
#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H
#include "timeObserver.h"
#include "clock.h"
#include "finance.h"

#include <vector>
#include <iostream>
#include <thread>

using namespace std;

class TimeManager {
private:
    Clock simulation_time; // Holds current world time
    vector<TimeObserver*> observers;

    //Finance object
    Finance* Finance_obj;


public:
    // Constructor
    TimeManager();

    //Basic methods
    void startSimulation(); //Initializes all time values to 0 and does some basic error checking
    void updateSimulationTime(int hours, int minutes, int seconds); // Notifies all objects with clock objects that time has progressed by a given increment. Uses a try catch, if it fails should 
    void addObserver(TimeObserver* observer); //Adds an observer to the vector of time observers
    void notifyObservers(); //Notify all observers of a time update, called by updateSimulationTime

    // Setter method
    void setFinanceObject(Finance *Finance_ptr);
};

#endif // TIME_MANAGER_H
