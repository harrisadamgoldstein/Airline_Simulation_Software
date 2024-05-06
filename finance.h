/*
*   finance.h
*
*   Used to create financial summaries of days, 
*   flights, and total costs and revenue.
*
*   Objects will report costs and revenue to the 
*   finance object so that finance can keep track of 
*   the overall daily/total costs and revenues.
*
*
*
*
*
*/

#define B600LOAN 50;
#define B800LOAN 50;
#define A100LOAN 50;
#define A300LOAN 50;

#ifndef FINANCE_H
#define FINANCE_H

//#include "timeManager.h"
//#include "plane.h"
//#include "airport.h"
//#include "passenger.h"
//#include "airline.h"
//#include "input.h"
#include "logger.h"
#include "flight.h"

#include <iostream>
#include <fstream>
#include <mutex>
#include <unistd.h>

using namespace std;

class Finance {

    private:
//        void setAirlineObject(Airline *new_airline_obj);

        double totalDailyCost = 0;
        double totalDailyRevenue = 0;

        double totalOverallCost = 0;
        double totalOverallRevenue = 0;

        ofstream financeObj;

        const string financeFileName;

        //Debugging flag
        bool debugging = false;

        

    public:
        Finance();

//        Input Input_obj; //TODO: Supposed to be used for getModelCount(), but requires input.h which results in circular dependency

//        double calculateRevenue(Flight flight, Plane plane);
        void calculateDailyLoan();
        void reportPlaneCost(int planeID, double value);
        void reportPlaneRevenue(int planeID, int numOfPassengers);
        void reportDay(int day);
        void reportOverall();

        void setFlightVector(vector<Flight*> *All_flights);

        //Vector pointer to find flight info
        vector<Flight*>* All_flights;
};
#endif //FINANCE_H


