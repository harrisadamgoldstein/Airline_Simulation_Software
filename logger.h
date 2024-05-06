/*
logger.h

Logger header file

Logger - This is object which each "logging" object will have a reference to. Eventually, it will
hold outputs for flightLog, planeLog, airportLog, financeLog, and errorLog. Objects using this 
will need to be passed a pointer to one singular logger file, to avoid conflicts.

*/

#ifndef LOGGER_H
#define LOGGER_H

#include "clock.h"
#include "flight.h"

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>

using namespace std;

class Logger 
{
public:
    // Constructor
    Logger();

    // Destructor
    ~Logger();

    // Methods to receive updates from planes, flights, and airports
    void logPlaneUpdate(int pid, int p_status, const Clock& first_time);
    void logFlightUpdate(int fid, int f_status, string a_name_1, string a_name_2, const Clock& first_time, const Clock& second_time);
    void logComplication(int fid, string msg);
    void logAirportUpdate(int aid, int a_status, int gate_ID, Clock first_time );
    void errorLog(int severity, string message); //Logs an error- 3 severity levels, 0, 1, and 2. 5 level 2 errors abort the program, 1 level 2 error immediately aborts. (Level 0 errors do not count towards aborting program)
    void updateFlightVector(vector<Flight*> New_vector); //Called by scheduler when theres been a flight change
    void newDayMarking(int Day); //Prints a neat day divison in log files

private:

    
    //Mutex lock
    mutex log_mutex;

    // Strings to hold log updates for each entity
    string plane_log;
    string flight_log;
    string airport_log;

    // Ofstream variables
    ofstream planeLogFile;
    ofstream flightLogFile;
    ofstream airportLogFile;
    ofstream errorLogFile;

    // Names of files to be printed to
    const string planeFileName;
    const string flightFileName;
    const string airportFileName;
    const string errorFileName;


    // Tracks how many level 1 errors have been encounters- 5 calls for the program to abort()
    int error1Count = 0;

    //Information for flight reporting
    vector<Flight*> All_flights;
    
};

#endif //LOGGER_H
