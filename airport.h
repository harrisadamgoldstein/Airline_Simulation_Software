/*
airport.h

Airport object header file

Airport - Airport represents a singular airport location which the client has 
access to. Airport will create both gate and passenger group objects, and
store them in vectors. Airport will receive information regarding flights via
the Airline, and will hold important data about itself such as geographical
location.

Who makes this object? Airline

Is it a time observer? Yes

What’s its goal? To hold passenger groups and gates

*/

// airport.h
#ifndef AIRPORT_H
#define AIRPORT_H


#include "timeObserver.h"
#include "clock.h"
#include "timeManager.h"
#include "gate.h"
#include "passenger.h"
#include "logger.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <mutex>

    //Mutex
    //extern mutex Airport_Con_Mutex;

using namespace std;   

class Airport : public TimeObserver {
private:
    //Variables
    int Airport_ID = -1; // Primary key for airport
    Clock Objects_clock; // Instance of the clock object for the airport, used to keep simulation synchronization
    //Clock Passenger_clock;
    string Airport_name; // Airport name

    //Location variables
    double longitude = 0;
    double latitude = 0;

    //Whether or not hub
    int hub_status = 0;

    //Pointers to nessesary objects
    TimeManager* time_manager = nullptr;
    Logger* Log_object = nullptr; 
    
    int day = 0; //Current day
    bool Airport_open; // Whether or not airport is open
    bool atGate = true; //Whether or not the passenger group is at their gate

    int universalPID; //FUTURE CHANGE, want a univeral passenger ID, so that they do not overlap

    //Debugging
    bool debugging = false;


public:

    //Vector for all passenger and gate objects
    vector<Passenger*> All_passenger_groups;
    
    //vector<Passenger*> passengersAtGate; //vector of copied passenger groups that have atGate == true

    vector<Passenger>& getAllPassengerGroups();
    vector<Passenger*> atGateGroups; 
    vector<Gate*> All_gates;
    //Constructor
    Airport(int airport_ID, string airport_name, double longitude, double latitude, int hub_status);

    // Register a passenger_group (to vector) and as an observer
    void registerPassengerGroup(Passenger* passengerGroupID);

    // Register a gate (to vector) and as an observer
    void registerGate(Gate* gateID);

    // Implement the TimeObserver interface
    virtual void onTimeUpdate(Clock& new_time) override;
    virtual void updateDay(int Day) override;


    // Getters
    int getAirportID(); // Return Airport ID
    string getAirportName(); // Return Airport name
    bool getAirportOpen(); // Return whether or not open
    int getHubStatus(); //Return whether airport is a hub or not, 0 means not and 1 means hub
    double getLat(); // Returns latitude
    double getLong(); //Returns longitude

    // Setters
    void setTimeManager(TimeManager *time_manager); //CRITICAL STEP - passes a a refernce to time manager
    void setLogObject(Logger *log_pointer); //CRITICAL STEP - passes a reference to logger object
    void setAirportID(int id); // Set Airport ID
    void setAirportName(string name); // Set Airport name

    // Multi-action
    int findGate(int GateID); // Finds gate based on Gate_ID, returns the position in vector

    //will "move" passenger grous to assigned gate
   

    //-----------------
    // Airport-gate-plane interaction methods
    //------------------

    void transferToGate(int gate_ID); 
    vector<Passenger> transferToPlane(int gate_ID);
    void freeGate(int gate_ID);
    void passengersToDeparture(int gate_ID, Passenger* passengerGroupID);
    
};


#endif // AIRPORT_H
