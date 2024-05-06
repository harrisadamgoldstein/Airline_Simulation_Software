#include "logger.h"
#include <iostream>
#include <sstream>

using namespace std;

//constructor
Logger::Logger() : planeFileName("planeLog.txt"), flightFileName("flightLog.txt"), airportFileName("airportLog.txt"), errorFileName("errorLog.txt") 
{

    // Open respective files
    planeLogFile.open(planeFileName);
    airportLogFile.open(airportFileName);
    flightLogFile.open(flightFileName);
    errorLogFile.open(errorFileName);


    // Check if files opened successfully
    if (!planeLogFile.is_open() || !airportLogFile.is_open() || !flightLogFile.is_open()) 
    {
        cerr << "ERROR: Unable to open log files for writing." << endl;
    }

    //Write opener for all files
    planeLogFile << "--------------------------------------------------------------------------" << endl;
    planeLogFile << "PLANELOG                                 " << endl;
    planeLogFile << "--------------------------------------------------------------------------" << endl;

    airportLogFile << "-------------------------------------------------------------------------" << endl;
    airportLogFile << "AIRPORTLOG                              " << endl;
    airportLogFile << "-------------------------------------------------------------------------" << endl;

    flightLogFile << "--------------------------------------------------------------------------" << endl;
    flightLogFile << "FLIGHTLOG                              " << endl;
    flightLogFile << "--------------------------------------------------------------------------" << endl;

    errorLogFile << "---------------------------------------------------------------------------" << endl;
    errorLogFile << "ERRORLOG                              " << endl;
    errorLogFile << "---------------------------------------------------------------------------" << endl;

}


//destructor
Logger::~Logger() 
{
    // Close respective files
    planeLogFile.close();
    airportLogFile.close();
    flightLogFile.close();
    errorLogFile.close();
}


// File that reads methods sent by plane.cpp to determine status of the plane and export it into a text file as a log 
void Logger::logPlaneUpdate(int pid, int p_status, const Clock& first_time) {
    
    //This allows only one print out statement for the progrm at a time, it slowws things down a bit,
    // but ensures saftey in our printouts
    lock_guard<mutex> lock(log_mutex);

    
    switch (p_status)
        {
        case 1:
            //plane is delayed as well as how long it is delayed for
            planeLogFile << "PLANE STATUS: PLaneID" << pid << " is delayed by " << first_time << endl;
            break;
        case 2:
            //boarding
            planeLogFile << "PLANE STATUS: PLaneID" << pid << " is boarding passengers @" << first_time << endl;
            break;
        case 3:
            //unboarding
            planeLogFile << "PLANE STATUS: PLaneID" << pid << " is unboarding passengers @" << first_time << endl;
            break;
        case 4:
            //departed time and expected arrival
            planeLogFile << "PLANE STATUS: PLaneID" << pid << " has departed @" << first_time << endl;
            break;
        case 5:
            //has arrived and time of arrival
            planeLogFile << "PLANE STATUS: PLaneID" << pid << " has arrived at its destination @" << first_time << endl;
            break;
        case 6:
            //undergoing maintenance
            planeLogFile << "PLANE STATUS: PLaneID" << pid << " is undergoing maintenance @" << first_time << endl;
            break;
        case 7:
            //refueling
            planeLogFile << "PLANE STATUS: PLaneID" << pid << " is refueling @" << first_time << endl;
            break;
        case 8:
            //cancelled
            planeLogFile << "PLANE STATUS: PLaneID" << pid << " has been cancelled" << endl;
            break;
        case 9:
            //on time
            planeLogFile << "PLANE STATUS: PLaneID" << pid << " is currently on time" << endl;
            break;
        default:
            //no updates
            planeLogFile << "PLANE STATUS: PLaneID" << pid << " has no updates at the moment" << endl;
            break;
    }

   
}

// File that reads methods sent by flight.cpp to determine status of the flight and export it into a text file as a log 
void Logger::logFlightUpdate(int fid, int f_status, string a_name_1, string a_name_2, const Clock& first_time, const Clock& second_time) {

     //This allows only one print out statement for the progrm at a time, it slowws things down a bit,
    // but ensures saftey in our printouts
    lock_guard<mutex> lock(log_mutex);

    switch (f_status) 
    {
        case 1:
            //on time
            flightLogFile << "FLIGHT STATUS: " << "Flight " << fid << " is currently on time";
            break;
        case 2:
            //cancelled
            flightLogFile << "FLIGHT STATUS: " << "Flight " << fid << " has been cancelled";
            break;
        case 3:
            //flying
            flightLogFile << "FLIGHT STATUS: " << "Flight " << fid << " is in the air";
            break;
        case 4:
            //Scheduled
            flightLogFile << "FLIGHT STATUS: " << "Flight " << fid << " Created! Scheduled to depart from " << a_name_1 << " @" << first_time << " and arrives at " << a_name_2 << " @" << second_time << endl;
            break;
        case 5:
            //has landed
            flightLogFile << "FLIGHT STATUS: " << "Flight " << fid << " has landed";
            break;
        case 6:
            //delayed
            flightLogFile << "FLIGHT STATUS: " << "Flight " << fid << " is delayed";
            break;
        default:
            //no updates
            flightLogFile << "FLIGHT STATUS: " << "Flight " << fid << " has no updates at the moment";
            break;
    }

   
}
void Logger::logComplication(int fid, string msg){

    lock_guard<mutex> lock(log_mutex);

    flightLogFile << "FLIGHT STATUS: " << "Flight " << fid << " " << msg << endl;
}
// File that reads methods sent by airport.cpp to determine status of the airport and export it into a text file as a log 
void Logger::logAirportUpdate(int aid, int a_status, int gate_id, Clock first_time = Clock())
{

    //This allows only one print out statement for the progrm at a time, it slowws things down a bit,
    // but ensures saftey in our printouts
    lock_guard<mutex> lock(log_mutex);


    switch (a_status) 
    {
        case 1:
            //gate reserved through negotiation
            airportLogFile << "AIRPORT STATUS: " << first_time << "Airport " << aid << " has negotiated Gate: " << gate_id << endl;
            break;
        case 2:
            //gate change
            airportLogFile << "AIRPORT STATUS: " << "Airport " << aid << " pulse check @" << first_time << endl;
            break;
        case 3:
            //gate freed
            airportLogFile << "AIRPORT STATUS: " << first_time << "Airport " << aid << " has freed Gate: " << gate_id << endl;
            break;
        default:
            //No updates
            airportLogFile << "AIRPORT STATUS: " << "Airport " << aid << " has no updates at the moment" << endl;
            break;
    }
    
}

void Logger::errorLog(int severity, string message) //TODO: Integrate with general logger functionality- add errorLog file and export functionality. Make abort() function actually do something.
{
    //This allows only one print out statement for the progrm at a time, it slowws things down a bit,
    // but ensures saftey in our printouts
    lock_guard<mutex> lock(log_mutex);

   errorLogFile << "ERROR: " << message << ", severity " << severity << endl;
   if(severity == 2){
      abort();
   }
   else if(severity == 1){
        error1Count++;
   }
   else if(severity == 0){
        //Do nothing
   }
   else{
         errorLogFile << "ERROR: Bad value passed to error logger" << endl;
   }

   if (error1Count >= 5 || severity == 2)
   {
        //Print out
        cerr << "/////// MAXIMUM AMOUNT OF SEVERITY 1 ERRORS REACHED /////////" << endl;
        abort();
   }
}

void Logger::newDayMarking(int Day){
    lock_guard<mutex> lock(log_mutex);

    airportLogFile << "-----------------------------------" << endl;
    airportLogFile << "Starting Day: " << Day << "   V V V" << endl;
    airportLogFile << "-----------------------------------" << endl;

    flightLogFile << "-----------------------------------" << endl;
    flightLogFile << "Starting Day: " << Day << "   V V V" << endl;
    flightLogFile << "-----------------------------------" << endl;

    planeLogFile << "-----------------------------------" << endl;
    planeLogFile << "Starting Day: " << Day << "   V V V" << endl;
    planeLogFile << "-----------------------------------" << endl;

    errorLogFile << "-----------------------------------" << endl;
    errorLogFile << "Starting Day: " << Day << "   V V V" << endl;
    errorLogFile << "-----------------------------------" << endl;

}

void Logger::updateFlightVector(vector<Flight*> New_vector){
    this->All_flights = New_vector;
}
