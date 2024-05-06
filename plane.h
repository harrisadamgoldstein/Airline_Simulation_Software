/*
Plane.h


Plane- This object represents a single plane owned by the customer. It needs to
 have all the appropriate variables listed in the handout, i.e. Make/model and
 corresponding passenger capacities. Plane receives a vector of passengers from
 its origin gate, then flies to the next airport to unload this vector of
 passengers. On each time update, plane may be flying, waiting at a gate, or in
 maintenance. The expenses for each plane at the end of the day need to be
 recorded by the airline object.

Who makes this object? Airline

Is it a time observer? Yes

What’s its goal? Be scheduled for a flight, and transport passengers in between
Airports by interacting with gates.


Plane object header file
*/
// Plane.h
#define FUELCOST 6.19
#ifndef PLANE_H
#define PLANE_H
#include "timeObserver.h"
#include "passenger.h"
#include "airport.h"
#include "logger.h"
#include "finance.h"

#include <string>
#include <fstream>

using namespace std;

class Plane : public TimeObserver {
private:

    /* BEGIN VARIABLES */

    bool debugging = false;
    
    //Basic plane attributes
    int Plane_ID; //The ID number of the plane (Primary Key)
    string Plane_name; //Tail number
    string Plane_model; //The make of the plane, e.g. “Boeing 757”
    Clock Objects_clock; //Instance of the clock object for the plane, used to keep simulation synchronization
    Clock Arrival_time;
    Clock Departure_time;

    Clock Zero_clock; //Clock of empty values or no time

    int day = 0;

    
    
    //Object references
    Logger* Log_object = nullptr;
    Finance* Finance_object = nullptr;

    //Target airport pointer
    Airport* Target_airport_object = nullptr;
    Airport* Origin_airport_object = nullptr;

    //Variables for particular time update
    double duration;

    //Fuel and travel variables
    float Fuel_tank; //How much fuel does the plane have remaining?
    float Max_fuel; //How much fuel can the plane hold at maximum?
    float Burn_rate; //How much fuel does the plane use up? Liters per kilometer.
    float Current_velocity; //KM per hour
    float Max_velocity; //Maximum velocity in KM/hour, ideal velocity i.e. what the plane can be expected to spend most of a flight at when not in takeoff/landing is 90% of this.

    //Plane Status 
    bool isFlying = false; //This is the status of the plane. Declared True if the plane has taken off. False if the plane is not flying
    bool isGrounded = true; //This is the status of the plane if its grounded or has not taken off.
    bool isMaintenance = false; //This is the maintenance status of the plane
    bool isWaiting = true; //This is the waiting status of the plane 
    bool isBoarding = false; //This is the boarding status of the plane
    bool isUnboarding = false; //This is the unboarding status of the plane.
    bool isAboutToTakeoff = false; //Waiting on tarmac about to leave
    int untilMaintDone; //Time until maintenance is done, in minutes. 0 if plane is not in maintenance.

    //Availability bools
    bool Is_ready_for_assignment; //Is the plane ready for next scheduler assignment?
    bool Is_operable; //Can the plane currently be used for transport?
    bool Is_seats_open; //Is the plane not completely full of passengers?
    int waitingTime; //Variable used for waiting time

    //Travel distance trackers
    double Odometer; //How many miles the plane has flown, in total.
    double Trip_odometer; //How many miles the plane has flown in its current flight.
    double Target_airport_location_distance = -1; // Km to reach airport
    double Until_maint; //How long until the plane needs maintenance. Starts at 200 (hours of flight time).

    //Passenger variables
    int Max_passengers;//How many passengers the plane can carry
    //int Onboard; //Count of passengers //TODO: Make this *actually* a passenger vector, not just an int
    int Count_of_passengers = 0; // Number of passengers onboard

    //Costs
    double Daily_cost; //The overall cost to operate a plane, per day, in USD. Includes loan cost, fuel cost etc. Calculated by calcCost.
    double Loan_cost; //The amount of the loan cost per plane per month. Paid even if the plane is not in usage.
 
    //Location, origin, destination
    double Range; //KILOMETERS of travel possible remaining
    int Origin_airport_ID = -1; //The airport the plane is starting from on a flight.
    int Target_airport_ID = -1; //The airport the plane is scheduled to land at next.
    int Origin_gate_ID = 0;//Gate from origin airport
    int Target_gate_ID = 0; //The gate of the targeted airport at which the plane is scheduled to land.

    //Finance
    Finance* Finance_obj; //Reference for the Finance object, for passing finance information to
    /* END VARIABLES */


public:

    //Delay objects
    Clock Gate_delay; //Delays at gate
    Clock Grounded_delay; //Delay grounded but not at gate

    //Temp - just gonna make this public for now
    vector<Passenger> Onboard;
    vector<Passenger*> passenger_vector; 
    /* BEGIN GETTERS */
    int getPlaneID(); //Returns PlaneID
    string getPlaneName(); //Returns 'tail number' of plane
    string getPlaneModel(); //Returns plane model
    double getMaintenance(); //Returns Until_maintenance
    double getOdometer(); //Returns Odometer
    double getTripOdometer(); //Returns Trip_odometer
    bool getIsReadyForAssignment(); //Checks if Is_transit is true
    bool getIsOperable(); //Checks if Is_operable is true
    bool getIsSeatsOpen(); //Checks if Is_seats_open is true
    string getLocation(); //Returns distance from target airport //TODO: This should be a location object, set as a string for now for ease-of-testing other functions
    float getFuelLevel(); //Returns Fuel_tank
    double getDailyCost(); //Returns Daily_cost
    int getTargetGate(); //Returns the value of Target_gate
    int getTargetAirport();  //Returns the value of Target_airport
    int getPassengerCount(); //Returns the number of passengers in Onboard

    /* BEGIN SETTERS */
    void setPassengerVectorFromAirport(const vector<Passenger*>& allPassengers);
    void setLogObject(Logger *log_pointer); //CRITICAL STEP - passes a reference to logger object
    void setFinanceObject(Finance *New_finance_obj); //Sets the Finance object to report to, should be common with all other modules
    void resetTripOdometer(); //Sets Trip_odometer to 0
    void setIsReadyForAssignment(bool isReady); //Set ready/not ready for next assignment
    void setFuelTank(float fuel); //Sets Fuel_Tank to a specific value.
    void setAirportObject(Airport *airport_pointer);
    void refuelToFull(); //Sets Fuel_tank to be equal to Max_fuel
    void setMaintStatus(bool isInMaint); //Sets whether the plane is in maintenance or not
    void setArrivalTime(Clock newTime); //Set clock object for arrival time
    void setDepartureTime(Clock newTime);//Set clock for departure time

    void setTargetGate(int gateID); //Sets the value of Target_gate
    void setOriginGate(int gateID);
    void setTargetAirport(int airportID); //Sets the value of Target_airport
    void boardPassengers(); //Add all passengers waiting to board to Onboard. This should be called by the airport. //TODO: Currently set as an int for prototyping purposes, needs to be changed to vector of passenger objects at some point
    void disembarkPassengers(); //Remove all passengers from Onboard. Called by the airport //TODO: Currently set as an int for prototyping purposes, needs to be changed to a vector of passenger objects at some point..
    /* END SETTERS */

    /*BEGIN OPERATION FUNCTIONS*/
    void fly(); //Called from checkStatus if flying is true, progress towards the target airport’s location by amount specified by velocity variable. This method will affect the values of many variables such as fuel level, odometer, distance to target airport, etc.
    void sendToMaintenance(); //Assign plane to maintenance for a given time
    void doMaintenance(); //Chips away at maintenance time, eventually sets to false after it hits zero
    void goTakeOff();// The takeoff method. Checking to see if plane is flying 
    void goLanding(); // The landing method. Called when the plane is not flying
    void inWaitingTime(); // The Waiting method. Called when the plane is grounded and waiting. 
    void topOffTank(); //If not at max fuel, refill
    //void landAndDock(); //Docks plane at airport, requests passenger operations, fuel operations
    /*END OPERATION FUNCTIONS*/

    /* BEGIN MISCELLANEOUS FUNCTIONS */
    /*double*/ void calcCost(); //Calculates the value of Daily Cost, takes in information from fuel cost, loan etc. //TODO: As-is, this doesn't actually calculate anything, just reports to Finance object       
    void planeStatus(); // The method used for checking the plane status and making right decison
    void checkFuelLevel(); // The Checkfuel level method. It will check the fuel level and update the fuel level.
    double findDuration(Clock& new_time); //Find duration of THIS update
    /* END MISCELLANEOUS FUNCTIONS */

    /* Scheduling and communication*/
    void assignFlight(int origin_airport_ID,int target_airport_ID, Clock arrivalTime, Clock departTime, 
            double distance, Airport* origin_airport_pointer, Airport* target_airport_pointer); //Called by scheduler, gives needed values to assign a new flight.
    void negotiateGate(int selection); //(Moved here from airline) Selection = 1(Origin Negotiate) or =2 (Dest Negotiate)

    /* Time Manager Methods*/
    virtual void updateDay(int Day) override;
    virtual void onTimeUpdate(Clock& new_time) override; //Implements time update ability, inherited from observer

    /* Delay Objects*/
    void addDelay(int selection, int delay_min);//Selection 0 = delay AT gate, selection 1 = NOT at gate
    void decrementDelay(int selection);//Selection 0 = delay AT gate, selection 1 = NOT at gate


    //Constructor/destructor
    Plane(int Plane_ID, string Plane_name, string Plane_model, float Max_fuel, float Burn_rate, float Max_velocity, int Max_passengers);
    ~Plane();

    
};

#endif // PLANE_H

//TODO: Add origin airport ID, don't worry about timings
