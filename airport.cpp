/*
airport.cpp

*/
// Airport.cpp

#include "airport.h"
#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>

using namespace std;

// Constructor
Airport::Airport(int airport_ID, string airport_name, double longitude, double latitude, int hub_status): Objects_clock(0, 0, 0)
{


    this->Airport_name = airport_name;
    this->longitude = longitude;
    this->latitude = latitude;
    this->hub_status = hub_status;

    // Initialize Airport_ID to a default value 
    Airport_ID = airport_ID;

    // Set the initial value of Airport_open 
    Airport_open = true;
    getHubStatus();

    //Depending on whether or not hub, this will change
    int num_of_gates;

    //Continue with object creation
    if(hub_status == 1){
        //Hubs have 11 gates
        num_of_gates = 11;
        for(int i = 0; i < num_of_gates; i++){
            //Creating and registering gates
            Gate* gateID = new Gate(i);
            registerGate(gateID);
        
            //Add five passenger groups to each gate
            for(int x=0; x <5; x++){
                Passenger* passengerGroupID = new Passenger(x); 
                //function call to push passengers onto departing_passenger vector
                passengersToDeparture(i, passengerGroupID);
            }     
        }
    }
    else{
        //Non hubs have 5 gates
        num_of_gates = 5;

        for(int i = 0; i < num_of_gates; i++){
            //Creating and registering gates
            Gate* gateID = new Gate(i);
            registerGate(gateID);
        
            //Create five passenger groups for each gate
            for(int x=0; x < 5; x++){
                Passenger* passengerGroupID = new Passenger(x); 
                //function call to push passengers onto departing_passenger vector
                passengersToDeparture(i, passengerGroupID);
            }     
        }

    }
}

//Add reference to TimeManager object, then complete object creation
void Airport::setTimeManager(TimeManager *time_manager){

    //Assign our time_manager
    this->time_manager = time_manager;

}

void Airport::updateDay(int Day){
    this->day = Day;

    //Iterate through gates and ensure they are free
    for(int i = 0; i < All_gates.size(); i++){
	    freeGate(All_gates[i]->getGateID());
    }
}

void Airport::setLogObject(Logger *log_pointer){

    //Assign our logger object
    this->Log_object = log_pointer;
    
}
//Register a passenger_group (to vector) and as an observer
void Airport::registerPassengerGroup(Passenger* passengerGroupID)
{
    //Only add to vector
    All_passenger_groups.push_back(passengerGroupID);

}
//Register a gate (to vector) and as an observer
void Airport::registerGate(Gate* gateID)
{
    //time_manager->addObserver(gateID);
    All_gates.push_back(gateID);
}

//When time gets updated
void Airport::onTimeUpdate(Clock& new_time) 
{

    //Start by setting done to false
    TimeObserver::setIsNotDone();

    //Report time update to cout
    Objects_clock = new_time;

    if(debugging){
    cout << "Airport " << Airport_name << " updated its time to "
              << new_time.hours << ":" << new_time.minutes << ":" << new_time.seconds << endl;
    }

    //loop checking to see if gates are full with passengers, if not
    //function is called to push more passengers onto departing_passenger vector
    int num_of_gates = 0;
    int difference = 0;

    if(hub_status == 1){
        //Hubs have 11 gates
        num_of_gates = 11;

        for(int i = 0; i < num_of_gates; i++){
            //If there are less than five passenger groups here
            if(All_gates[i]->Departing_passengers.size() < 5){

                //Find the difference
                int difference = 5 - All_gates[i]->Departing_passengers.size();
                //Send that many to gate
                for(int x = 0; x < difference; x++){
                    Passenger* passengerGroupID = new Passenger(x);
                    passengersToDeparture(i, passengerGroupID);
                }
            }
        }
    }
    else{
        //Non hubs have 5 gates
        num_of_gates = 5;

        for(int i = 0; i < num_of_gates; i++){
            //If there are less than five passenger groups here
            if(All_gates[i]->Departing_passengers.size() < 5){

                //Find the difference
                int difference = 5 - All_gates[i]->Departing_passengers.size();
                //Send that many to gate
                for(int x = 0; x < difference; x++){
                    Passenger* passengerGroupID = new Passenger(x);
                    passengersToDeparture(i, passengerGroupID);
                }
            }
        }
    }

    //Say that we are done
    TimeObserver::setIsDone();
}

//Getters

// Return Airport ID
int Airport::getAirportID() 
{
    return Airport_ID;
}

// Return Airport name
string Airport::getAirportName() 
{
    return Airport_name;
}

// Return whether or not open
bool Airport::getAirportOpen() 
{
    return Airport_open;
} 

// Return if hub or not
int Airport::getHubStatus()
{
    return hub_status;
}
double Airport::getLat(){
    return this->latitude;
}
double Airport::getLong(){
    return this->longitude;
}
//Setters

// Set Airport ID
void Airport::setAirportID(int id) 
{
    Airport_ID = id;
}

// Set Airport name
void Airport::setAirportName(string name) 
{
    Airport_name = name;
}

//Multi-action

// Finds gate based on Gate_ID, returns the position in vector so if i search for a specific gate id it will return position of vector
int Airport::findGate(int GateID) 
{
    // Iterate through all gates in the vector
    for (size_t i = 0; i < All_gates.size(); ++i) 
    {
        // Check if the Gate_ID of the current gate matches the provided GateID
        if (All_gates[i]->getGateID() == GateID) 
        {
            // Return the position (index) in the vector where the gate with the matching ID is found
            return static_cast<int>(i);
        }
    }
    // If no gate with the given ID is found, return -1 to indicate that the gate is not found
    return -1;
}

//Will move passenger groups to the target gate ID 

void Airport::transferToGate(int gate_ID){

    //Assign passed vector as the vector at gate
    All_gates[gate_ID]->Arriving_passengers = All_gates[gate_ID]->Departing_passengers;
    if(debugging){
        cout << "Size of Arriving_passengers vector: " << All_gates[gate_ID]->Arriving_passengers.size() <<endl;
    }
    All_gates[gate_ID]->Arriving_passengers.clear();

}

vector<Passenger> Airport::transferToPlane(int gate_ID){

    //Placeholder vector
    vector<Passenger> temp;

    //Assign vector to placeholder
    //temp = All_gates[gate_ID]->Departing_passengers;
    for(const auto& passPtr : All_gates[gate_ID]->Departing_passengers){
        temp.push_back(*passPtr);
    }
    if(debugging){
        string yeet;
        cout << "Size of temp: " << temp.size() << endl;
        cout << "Size of Departing: " << All_gates[gate_ID]->Departing_passengers.size() << endl;
        cin >> yeet;
    }
    //Clear vector at gate
    All_gates[gate_ID]->Departing_passengers.clear();
    
    //Return the original vector at gate
    return temp;


}
void Airport::freeGate(int gate_ID){
    //Set given gate to free
    All_gates[gate_ID]->setInUse(false);

    //Tell logger its been freed
    Log_object->logAirportUpdate(this->Airport_ID, 3, gate_ID, this->Objects_clock);
}

//This function will use input validation and then create a passenger group for a specified gate
//and push the group onto departing passenger vector
void Airport::passengersToDeparture(int gate_ID, Passenger* passengerGroupID){
    if(gate_ID >= 0 && gate_ID < All_gates.size()){
       All_gates[gate_ID]->Departing_passengers.push_back(passengerGroupID); 
    }
    else{
        Log_object->errorLog(0,"Error! Invalid gate ID [AIRPORT.CPP][LNE 238]");
    }
}
