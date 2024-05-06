/*
*   finance.cpp
*
*   Implementation of finance.h, used to give a financial summary
*   of each day, flight, and overall costs and revenue
*
*
*/
#include "finance.h"
#include <sstream>


Finance::Finance() : financeFileName("financeLog.txt")
{
    financeObj.open(financeFileName);

	 financeObj << "-------------------------------------------------------------------------" << endl;
    financeObj << "FINANCELOG                              " << endl;
    financeObj << "-------------------------------------------------------------------------" << endl;

}

void Finance::setFlightVector(vector<Flight*>* flights_ptr){
	//Set our pointer to this vector
	All_flights = flights_ptr;
}

/*
int Input::getModelCount(string model){
    int modelCount = 0;
//    vector<Plane*> All_planes = Input_object->get_plane_vector(); //TODO: Requires plane.h, which causes a circular dependency.

    for(int i=0; i<All_planes.size(); i++){
        if (All_planes[i]->getPlaneModel() == model)
        {
           modelCount++;
        }
    }
    if (debugging)
        cerr << "modelCount for " << model << ": " << modelCount << endl;
    return modelCount;
}

float calculateDailyLoan()
{
   float loanPerDay = 0;

   loanPerDay+=Airline::getModelCount("B600")*B600LOAN;
   loanPerDay+=Airline::getModelCount("B800")*B800LOAN;
   loanPerDay+=Airline::getModelCount("A100")*A100LOAN;
   loanPerDay+=Airline::getModelCount("A300")*A300LOAN;

   return loanPerDay;
}
*/

//Report a cost of a plane to the finance object
void Finance::reportPlaneCost(int planeID, double value)
{
	bool planeFound = false;

	if(value > 0) 
	{
		if(debugging){
			financeObj << "Plane " << planeID << " reports a cost of: " << value << endl;
		}
		totalDailyCost += value; 
		totalOverallCost += value;

		//Add this value to the flight's vector
		if(All_flights){
			for(int i=0; i < All_flights->size(); i++){
				if(planeID == (*All_flights)[i]->getPlaneID()){
					planeFound = true;
					(*All_flights)[i]->cost += value;
				}
			}
			if(!planeFound){
				cerr << "Error! No planeID found" << endl;
			}

		}
		else{
			cerr << "Error! No flight vector in finance" << endl;
		}
	}
	else
	{
		cerr << "Error! Negative cost not allowed. " << endl;
	}
}
//Report a revenue of a plane to the finance object
void Finance::reportPlaneRevenue(int planeID, int numOfPassengers)
{
	bool planeFound = false;
	double value = 0;

	if(numOfPassengers > 0) 
	{ 
		//Add this value to the flight's vector
		if(All_flights){
			for(int i=0; i < All_flights->size(); i++){
				if(planeID == (*All_flights)[i]->getPlaneID()){
					//We found our plane!
					planeFound = true;

					// ---------------------
					//Find out ticket price
					value = (*All_flights)[i]->getTicketCost();

					//Multiply by number of passengers
					//Remember, passenger group is ten people
					value = value * (numOfPassengers * 10);

					(*All_flights)[i]->revenue += value;

					if(debugging){
						financeObj << "Plane " << planeID << " reports a revenue of: " << value << endl;
					}
						totalDailyRevenue += value;
						totalOverallRevenue += value;
				}
			}
			if(!planeFound){
				cerr << "Error! No planeID found" << endl;
			}

		}
		else{
			cerr << "Error! No flight vector in finance" << endl;
		}
	}
	else
	{
		cerr << "Error! Negative passengers not allowed. " << endl;
	}

	if(debugging){
		string testValue;
		cerr << "DEBUGGING: Plane reporting revenue ID=" << planeID << endl;
		cerr << "Number of passengers: " << numOfPassengers << " Revenue: " << value << endl;
		cin >>testValue;
	}
}
//Write a day into the financial summary log
void Finance::reportDay(int day)
{
	financeObj << "-----------------------------------------------" << endl;
	financeObj << "Simulation Day " << day << " Financial Summary" << endl;
	financeObj << "-----------------------------------------------" << endl;
	financeObj << endl;

	financeObj << "Total Cost: $" << totalDailyCost << endl;
	financeObj << "Total Revenue: $" << totalDailyRevenue << endl;
	
	
	if(totalDailyCost > totalDailyRevenue) 
	{
		financeObj << "A total of $" << totalDailyCost-totalDailyRevenue << " was LOST during day " << day << "." << endl;	
	}
	else if(totalDailyRevenue > totalDailyCost)
	{
		financeObj << "A total of $" << totalDailyRevenue-totalDailyCost << " was GAINED during day " << day << "." << endl;
	}
	else if(totalDailyRevenue == totalDailyCost)
	{
		financeObj << "The daily cost and revenue were equal, resulting in a net gain of $0.00. " << endl;
	}
	else
	{
		cerr << "Unexpected error in reporting daily financial summary. Day: " << day << endl;
		financeObj << "Error getting difference in revenue and cost." << endl;
	}

	financeObj << "--------------------------------" << endl;
	financeObj << "Flight breakdown V V V" << endl;
	financeObj << "--------------------------------" << endl;

	//Print out cost of each flight
	//Add this value to the flight's vector
		if(All_flights){
			for(int i=0; i < All_flights->size(); i++){
				//Print out daily cost and revenue
				financeObj << "Flight: " << (*All_flights)[i]->getFlightID() << " Revenue = "
					<< (*All_flights)[i]->revenue << " Cost = " << (*All_flights)[i]->cost << endl;

				//Reset values
				(*All_flights)[i]->revenue = 0;
				(*All_flights)[i]->cost = 0;
            totalDailyRevenue = 0;
            totalDailyCost = 0;
			}

		}
		else{
			cerr << "Error! No flight vector in finance" << endl;
		}

	financeObj << "-----------------------------------------------" << endl;
}
//Write the overall report to the financial summary log
void Finance::reportOverall()
{
	financeObj << "-----------------------------------------------" << endl;
	financeObj << "Simulation Overall Financial Summary" << endl;

	financeObj << "Total Cost: $" << totalOverallCost << endl;
	financeObj << "Total Revenue: $" << totalOverallRevenue << endl;

	if(totalOverallCost > totalOverallRevenue) 
	{
		financeObj << "A total of $" << totalOverallCost-totalOverallRevenue << " was LOST during the simulation." << endl;	
	}
	else if(totalOverallRevenue > totalOverallCost)
	{
		financeObj << "A total of $" << totalOverallRevenue-totalOverallCost << " was GAINED during the simulation." << endl;
	}
	else if(totalOverallRevenue == totalOverallCost)
	{
		financeObj << "The daily cost and revenue were equal, resulting in a net gain of $0.00. " << endl;
	}
	else
	{
		cerr << "Unexpected error in reporting overall financial summary." << endl;
		financeObj << "Error getting difference in revenue and cost." << endl;
	}
	
	financeObj << "-----------------------------------------------" << endl;
}
