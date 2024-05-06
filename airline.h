/*
airline.h

Airline object header file

Airline - Airline is the overarching object with all the sum knowledge of the
simulation. It will create and store airport, plane and flight vectors. It
will be in charge of scheduling flights via the information read in from the
timetable, and making dynamic decisions such as rescheduling.

Who makes this object? Main

Is it a time observer? Yes

What’s its goal? Manage the whole simulation
testing testing
*/

// airport.h
#ifndef AIRLINE_H
#define AIRLINE_H


#include "timeObserver.h"
#include "clock.h"
#include "timeManager.h"
#include "flight.h"
#include "plane.h"
#include "airport.h"
#include "input.h"
#include "logger.h"

#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <cstdlib>
#include <ctime>


using namespace std;   


class Airline : public TimeObserver {

private:
    //Airlines name
    string Airline_name;

    //Output file name
     ofstream flightLog;

    //Vector for planes and airports
    //These are the planes the airline owns, and places it can go
    vector<Flight*> All_flights;
    vector<Plane*> All_planes;
    vector<Airport*> All_airports;

    bool scheduleNeeded = false;
    bool debugging = false;

    //How many complications loaded (used by set complication)
    int comp_count = 0;

    int day = 0;

public:

    //Public variables
    //Time manager object
    TimeManager* time_manager;

    //Logger object
    Logger* Log_object;

    //Finance object
    Finance* Finance_obj;

    //Input object
    Input Input_object;

    //Our clock object
    Clock Objects_clock;


    // --------------
    // Constructor/Destructor
    // --------------

    Airline(TimeManager *time_manager, string airport_name, Input Input_object, Logger *Log_object, Finance *Finance_obj);

    //Destructor
    ~Airline();

    // --------------
    // Creation/Registration methods
    // --------------

    // Register a plane as an observer
    void registerPlane(Plane* plane);

    // Register an airport as an observer
    void registerAirport(Airport* airport);


     // --------------
    // "TimeObserver" methods
    // --------------
    // Implement the TimeObserver interface
    virtual void onTimeUpdate(Clock& new_time) override;
    
    //Implement the ovveride of updateDay to allow for user input
    virtual void updateDay(int Day) override;

    // --------------
    // "Scheduler" methods
    // --------------

    void scheduleFlights(); // Schedule all unscheduled flights
    void swapFlights(int firstID, int secondID);
    void loadFlights();//Loads flights from vector (passed by input object), then fill out rest of missing info

    // --------------
    // "Complication" methods
    // --------------
    //TODO: Add way to track which day to flight
    void setComplication(int selection); //Adds the vector of complications from input and their given day, to trigger
    void scheduleDailyComplication(); //Called every 24 hours when time == 0:0:0, establishes complication for that day
    void findEffectedFlights(); //Finds all flights effected by this change, then calls reschedule flight
    void rescheduleFlight(); //Edits single entry in flight 

    // ---------------
    // "Searching" methods
    // ---------------
    //Search plane ID, returns position in array
    int findPlaneID(string plane_name);
    int findAirportID(string airport_name);

    // ---------------
    // "Gate negotiation" methods
    // ---------------
    void negotiateGate(int O_airport_ID, int D_airport_ID, int plane_ID); //Assigns plane a new availible target gate

    // -------------------
    // "Print" methods
    // -------------------
    void printFlightVector(); //Iterate through and print details of each flight
    void printPlaneVector(); //Iterate through and print details of each plane
    void printAirportVector();//Iterate through and print details of each airport
    int getModelCount(string model); //Returns the count of the specified plane model registered to the airline

    
};

#endif // AIRLINE_H
