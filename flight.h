/*
Flight.h

Flight  - Essentially, flight is the basic idea of a plane going to an airport
from an airport, at a certain time, at a certain gate. This object will be made
by the scheduler, and used as a basic unit for things like recalculation. It
will be given to an airport, so as to allow for communication and consistency.

Who makes this object? Values are read in from the time table into Airline,
which puts them in a vector.

Is it a time observer? No

What’s its goal? Hold the base value of one flight, meaning one trip in between
two airports, and the information such as ticket cost associated with it.


*/
// Flight.h
#ifndef FLIGHT_H
#define FLIGHT_H

#include "clock.h"
#include <string>
#include <mutex>


    //Mutex
    //extern mutex Flight_Con_Mutex;

class Flight {
private:

    //Basic details that make up a flight
    //Assigned during construction
    int Flight_ID = 0; //The number that identifies the flight (index)
    string Flight_type_ID = ""; //Id given in csv file 
    float Ticket_cost = 0; //The cost of a ticket for a flight (currently all tickets for a given flight are the same price)
    string Flight_type = ""; //TODO: What is this?
    string Plane_name = ""; // 'Tail #' of given plane
    string Dest_airport_name = ""; //The name of the airport where a flight ends
    string Origin_airport_name = ""; //The name of the airport where a flight begins
    int Takeoff_duration = 0; //Estimated duration of takeoff
    int In_air_duration = 0; //Estimated duration of the plane flying in air
    int Deboard_duration = 0; //Estimated duration of deboarding process

    //First value is current distance, second is the one set during construction
    //Why is this important? Need a way to reset flight values after complication
    double Distance = 0;
    double Dflt_dist = 0;

    //Calculated during scheduling
    int Dest_airport_ID = 0; //The ID number of the airport for the plane to land at
    int Origin_airport_ID = 0; //The airport that a plane starts at when a flight begins
    int D_gate_ID = 0; //Destination gate at target airport
    int O_gate_ID = 0; //Origin gate at origin airport
    int Plane_ID = 0; //The ID number of the plane used for a flight

    //Delay values
    double grounded_delay = 0;
    double gate_delay = 0;
    
    

    // Has this already been scheduled?
    bool Scheduled = false;

    //Debugging
    bool debugging = false;


public:

    Clock Departure_time; //Scheduled departure time
    Clock Arrival_time; //Estimated arrival time at origin airport/gate

    //For calculating revenue
    double revenue = 0;
    double cost = 0;

    // Constructor
    Flight(int Flight_ID, double Ticket_cost, string Flight_type, string Flight_type_ID, string plane_name, string Dest_airport_name, string Origin_airport_name, int D_hour, int D_min, int D_sec, int A_hour, int A_min, int A_sec, double distance, int takeoff_time, int air_time, int deboard_time);
    
    //Getters
    //ADD MORE GETTERS HERE
    int getFlightID(); //Return flight ID
    string getFlightTypeID(); //return flight type id
    bool getScheduled(); //Returns whether the flight has been given a time to begin
    int getPlaneID(); //Return plane ID
    string getPlaneName(); //Returns 'tail number' of plane
    int getDestAirptID(); //Return target airport ID
    int getOriginAirptID(); //Return origin airport ID
    double getDistance(); //Returns distance to be flown
    string getDestAirptName(); //Return destination airport name
    string getOriginAirptName(); //Return origin airport name
    Clock getArrivalTime(); //Return estimated arrival time at target airport
    Clock getDepartureTime(); //Returns scheduled departure time from origin 
    double getTicketCost(); //Returns the cost for a ticket for the flight
    double getGroundedDelay(); //Gets delay for plane not at gate
    double getGateDelay(); //Gets delay for plane at gate

    //find way to set clock times with an override function

    //Setters
    //ADD MORE SETTERS HERE
    void setFlightID(int Flight_ID);//Sets a new flight ID
    void setPlaneID(int Plane_ID); //Sets the ID number of the plane to be used for the flight
    void setDestAirptID(int Dest_airport_ID); //Set ID dest airport
    void setOriginAirptID(int Origin_airport_ID); // Set ID of origin airport

    void setOGateID(int O_gate_ID); //Sets gate for the flight to depart from
    void setDGateID(int D_gate_ID); //Sets gate for the flight to arrive at
    void setDistance(int Distance); //Sets distance to fly
    void setArrivalTime(Clock New_arrival_time); //Sets estimated arrival time
    void setDepartureTime(Clock New_departure_time); //Sets scheduled departure time

    void setScheduledFalse(); //Sets “Scheduled" variable to false
    void setScheduledTrue(); //Sets “Scheduled" variable to true

    void setGroundedDelay(double delay); //Sets delay for plane not at gate
    void setGateDelay(double delay); //Sets delay for plane at gate

    //Reset distance and delay values (needed after complication changes)
    void resetValues();
    
    //Print method
    void printFlightDetails(); //Print out details of this flight entry
};

#endif // Flight_H
