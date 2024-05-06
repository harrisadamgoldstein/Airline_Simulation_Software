/*
timeManager.cpp

Time manager implementation file
*/
// TimeManager.cpp
#include "timeManager.h"

TimeManager::TimeManager() : simulation_time(0, 0, 0) {}

void TimeManager::startSimulation(){
    
    int day = 0;
    //Setup: Calculate daily loan cost for Finance to use
//    Finance_obj->calculateDailyLoan();
    //THIS IS THE MAIN SIMULATION LOOP FOR TIME

    //Loop for the week
    for(int j = 0; j < 14; j++){

        day++;

        //Update the observers days
        //THIS ALSO CALLS THE USER PROMPT BY WAY OF AIRLINE OVVERIDING
        for (auto observer : observers) {

            //Update time
            observer->updateDay(day);
        }
        //Loop for one day
        //Each iteration is 10 minutes
        int hours = 0;
        int minutes = 0;
        int seconds = 0;
        for(int i = 0; i < 144; i++){
            // Hold values of hours, minutes, and seconds,
            // if minutes exceeds 60 add 1 to hours and reset minutes to 0
            minutes += 10;
            if(minutes >= 60){
                hours++;
                minutes = 0;
            }

            //Update time
            updateSimulationTime(hours, minutes, 0);

            //Basic error check
            for (auto observer : observers) {
                //Check if all objects are done
                if(observer->getIsDone() == false){
                    cerr << "ERROR: Object " << observer << " did not respond with done!" << endl;
                }

            }


        }

        //Send end of day report to finance
        Finance_obj->reportDay(day);
    }

    //Call our end report for the finance log
    Finance_obj->reportOverall();

    //End simulation print out
    cout << "-------------------------------------------------------------------------" << endl;
    cout << " SIMULATION COMPLETED                              " << endl;
    cout << "-------------------------------------------------------------------------" << endl;
}
void TimeManager::updateSimulationTime(int hours, int minutes, int seconds) {
    simulation_time = Clock(hours, minutes, seconds);
    cout << "--------------------------------------------------------" << endl;
    cout << "               TIME UPDATE   [" << simulation_time.hours << ":" << simulation_time.minutes << ":" << simulation_time.seconds << "]" << endl;
    cout << "--------------------------------------------------------" << endl;

    //Send out update, wait to progess until all objects have updated
    notifyObservers();
}

void TimeManager::addObserver(TimeObserver* observer) {
    observers.push_back(observer);
}

void TimeManager::notifyObservers() {
    for (auto observer : observers) {

        //Update time
        observer->onTimeUpdate(simulation_time);
    }
}

void TimeManager::setFinanceObject(Finance *Finance_ptr){
    this->Finance_obj = Finance_ptr;
}
