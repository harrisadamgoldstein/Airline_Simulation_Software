/*
gate.h

Gate object header file

Gate - The basic intermediary for the airport, holds things like passenger
count, flight and whether the plane is docked. Gate will not initiate
interactions, but instead be called by either plane or airport.

Who makes this object? Airport

Is it a time observer? Yes

What’s its goal? To act as an intermediary in between Airports and Planes for
transporting passenger groups.

*/
#ifndef GATE_H
#define GATE_H

#include "timeObserver.h"

#include <iostream>

#include <random>
#include <string>
#include <iomanip>
#include <vector>
#include "passenger.h"
#include "clock.h" // Include the header for Clock class
#include "timeObserver.h" // Include the header for TimeObserver class

using namespace std;

/*
    GATE
    Constructor - just establishes variables
*/
struct passengerMovement
{
    int groupID;
    int startGateID;
    int targetGateID;
    int remainingTime; 
};
class Gate : public TimeObserver{

private:

    int Gate_ID; // Primary key/identifier for gate
    bool In_use = false; // Is the gate currently in the process of boarding or onboarding?
    bool Plane_docked = false; // Is a plane currently docked at the gate?

    Clock Objects_clock; // Instance of the clock object for the gate, used to keep simulation synchronization

    int Plane_ID; // Which plane is docked at/using the gate?
    
    int day = 0;
public:

    // Constructor
    Gate(int gateID);

    // Implement the TimeObserver interface
    virtual void onTimeUpdate(Clock& new_time) override;
     virtual void updateDay(int Day) override;

    // Getters
    int getGateID(); // Return Gate ID (primary key)
    bool getInUse(); // Return In use var
    bool getPlaneDocked(); // Return if it is docked
    int getPlane_ID(); // Return plane ID
    int getPassengersAtGate(); // Return size of vector

    // Setters
    void setGateID(bool GateID); // Set in use variable
    void setInUse(bool inUse); // Set in use variable
    void setPlaneDocked(bool planeDocked); // Set if the plane is docked
    void setPlane_ID(int planeID); // Set plane ID

    // Multi-action methods
    //void board(vector<Passenger>& passengers); // Call plane board option and send the passenger vector
    //void disembark(); // Call disembark option on the plane and fill the passenger vector

//vectors for gates and passenger groups
    vector<Passenger*> Departing_passengers; // The number of passengers currently at the gate. Number sent from the airport.
    vector<Passenger*> Arriving_passengers; //People being unloaded from plane
    vector<Gate> gates;
    vector<int> passengerGroups;
    vector<passengerMovement> passengerMovements;

};

#endif // GATE_H
