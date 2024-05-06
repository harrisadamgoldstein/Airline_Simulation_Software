/*
main.cpp

Descr: This file is the main file for the sim, it is responsible for creating
a TimeManager and Airline object. It also updates TimeManager by controlling
the tick-rate (currently every 10 min is 1 second for sim).
*/


// main.cpp
#include "timeManager.h"
#include "plane.h"
#include "airport.h"
#include "passenger.h"
#include "airline.h"
#include "input.h"
#include "logger.h"
#include "finance.h"
#include <unistd.h>

using namespace std;

int main() {
    // -------------------------
    // Create objects
    // -------------------------

    //This is our time manager, which will be passed to all objects	
    TimeManager time_manager;
    //Object creation
    Input input;
    Logger log;
    Finance finance_obj;

    sleep(1);


    //At first, we only create our Airline
    // Airline -> Airports + Planes
    // Airports -> Gates + Passengers
    Airline airline(&time_manager, "Comfort Airlines", input, &log, &finance_obj);

    // -------------------------
    // Start 'er up!
    // -------------------------
    //Send finance reference
    time_manager.setFinanceObject(&finance_obj);
    
    // Start simulation
    time_manager.startSimulation();

    return 0;
}
