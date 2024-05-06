/*
*	input.h
*
*	Used to read in inputs from input files for 
*	objects such as flight, plane, and airport.
*
*	Airport constructor has issues regarding time_manager
*   parameter, giving seg faults when attempted access in read_airports().
*
*   
*
*
*/
#ifndef INPUT_H
#define INPUT_H

#include "TimeObserver.h"
#include "Clock.h"
#include "TimeManager.h"
#include "flight.h"
#include "Plane.h"
#include "Airport.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <unistd.h>

using namespace std;

class Input {

private:
    Flight* flight; //Flight object to access setters
    Plane* plane; //Plane object to access setters
    Airport* airport; //Airport object to access setters
    vector<Flight*> All_flights; //Create a vector to handle all flights
    vector<Plane*> All_planes; //Create a vector to handle all planes
    vector<Airport*> All_airports; //Createa a vector to handle all airports

    bool debugging = false; //debugging bool in order to output debug information if set to true
    static mutex input_lock;

public:
    TimeManager* time_manager; //TimeManager object to add observers per new object
    Clock Objects_clock; //Clock object
    Input(); //Basic constructor that calls all 3 read input functions.
    //~Input(); //Destructor?
	      
    void read_airports(); //Read in inputs for airport data. Airport method needs setters for longitude and latitude.
    void register_airport(Airport* airport); //Add each new airport to be an observer and add it to the vector All_airports.

    void read_planes(); //Read in inputs for plane data. Plane method needs setters for PlaneID, PlaneModel, MaxFuel, BurnRate, MaxSpeed, and MaxPassengers.
    void register_plane(Plane* plane); //Add each new plane to be an observer and add it to the vector All_planes.

    void read_flights(); //Read in inputs for flight data. I'm not sure if I went in the right direction with this one since I hadn't seen the scheduleFlights object in Flight.
			 //For current read_flights function, need setters for FlightID, Origin, Destination, TotalTime, Distance, DepartureTime, ArrivalTime, TicketPrice.
    void register_flight(Flight* flight); //Add each new flight to the vector All_flights. No need to add as an observer.

    vector<Airport*>& get_airport_vector();
    vector<Plane*>& get_plane_vector();
    vector<Flight*>& get_flight_vector();
};

#endif // INPUT_H
