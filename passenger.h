/*
    passenger.h

    Passenger object header file

    Passenger_group - Passenger group represents a group of passengers (right
    now 10 people), who have the same origin and destination. When they get
    a flight, the corresponding ticket cost is recorded as revenue for the
    airline. Additionally, the number of passenger groups transported also
    needs to be communicated to the end report. Passenger group doesn’t do much
    by itself, but is instead passed between gates and planes until they reach
    their final destination, in which case the object will be destroyed.

    Who makes this object? Airport

    Is it a time observer? Yes

    What’s its goal? Be transported through gates and planes to the final destination.

*/
// passenger.h
#ifndef PASSENGER_H
#define PASSENGER_H
#include "timeObserver.h"
#include <string>
#include <vector>
#include <iostream> 

using namespace std;

class Passenger {
private:
    int Passenger_group_ID; //The ID of a group of passengers, for tracking
    //Clock Objects_clock; //Instance of the clock object for the passenger, used to keep simulation synchronization
    Clock delay; // How long to transit between gates, set by airport, i.e. gate 10 to 30 = 15 min delay
    int targetGateID = 3; //Assigned target gate for passenger groups

public:
    int assignedGate; 
    bool atGate = false; // Are they at a gate?
    Passenger(int Passenger_group_ID); //Constructor
    void decrementPassengeDelay(Clock durationOfUpdate); //Called by airport to decrement time
    int assignedTargetGate; // Target gate number



    /*//Will assign the start gate to passenger group - used in airport
    void setStartGate(Gate* startGate);

    //Will assign the target gate to passenger group - used in airport
    void setTargetGate(Gate* targetGate);*/

    // Implement the TimeObserver interface
    // virtual void onTimeUpdate(Clock& new_time) override;
};

#endif // PASSENGER_H
