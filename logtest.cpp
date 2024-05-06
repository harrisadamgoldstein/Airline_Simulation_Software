// This is a program to test that logger works by providing manual input for the logger to enter into logs
// logtest.cpp
#include "logger.h"

int main() {
    // Define the data
    std::string fid = "UA567";
    std::string aid = "LAX";
    std::string pid = "Boeing747";
    int plane_status = 1; // Assuming plane status 1 means delayed
    int flight_status = 6; // Assuming flight status 6 means delayed
    int airport_status = 0; // Assuming default airport status

    // Create a logger object
    Logger myLogger;

    // Simulate logging updates
    myLogger.logPlaneUpdate(pid, plane_status, Clock(1, 30, 0), Clock()); // Assuming a delay of 1 hour and 30 minutes
    myLogger.logFlightUpdate(fid, flight_status, Clock(), Clock());
    myLogger.logAirportUpdate(aid, airport_status, Clock(), Clock());

    // Export logs to respective files
    //myLogger.exportLogsToFile(1); // Export plane logs
    //myLogger.exportLogsToFile(2); // Export flight logs
    //myLogger.exportLogsToFile(3); // Export airport logs

    return 0;
}
