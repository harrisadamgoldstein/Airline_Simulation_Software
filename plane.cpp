/*
Plane.cpp

Plane object implementation file
*/
// Plane.cpp
#include "Plane.h"
#include "finance.h"
#include <iostream>
using namespace std;

// hello

Plane::Plane(int Plane_ID, string Plane_name, string Plane_model, float Max_fuel, float Burn_rate, 
    float Max_velocity, int Max_passengers) : Objects_clock(0, 0, 0){

    // Check for invalid initialization parameters
    if (Max_fuel <= 0 || Burn_rate <= 0 || Max_velocity <= 0 || Max_passengers <= 0 || Plane_ID < 0) 
    {
        Log_object->errorLog(1, "Error! Invalid initialization parameters for Plane object [plane.cpp][Line 20]");
    }

    //Set the plane's main attributess
    this->Plane_ID = Plane_ID;
    this->Plane_name = Plane_name;
    this->Plane_model = Plane_model;

    //Set starting values
    this->Max_fuel = Max_fuel;
    this->Fuel_tank = Max_fuel;
    this->Burn_rate = Burn_rate;
    this->Max_velocity = Max_velocity;
    this->Max_passengers = Max_passengers;

    //Defualt values
    this->Current_velocity = 0;
    this->Odometer = 0;
    this->Trip_odometer = 0;
    this->Until_maint = 999999999;

    this->Is_ready_for_assignment = true;
    this->Is_operable = true;
    this->Is_seats_open = true;
    this->Daily_cost = 100;
    this->Loan_cost = 50;
    this->Range = 99999999999;

    //To be set by scheduler
    this->Target_airport_location_distance = 0;

    //Start plane off by waiting on the tarmac
    this->isFlying = false;
    this->isGrounded = true;
    this->isMaintenance = false;
    this->isWaiting = true;
    this->isBoarding = false;
    this->isUnboarding = false;

    //Initalize our zero clock values
    Zero_clock = Clock(0,0,0);


}

//Destructor
Plane::~Plane(){
}
void Plane::onTimeUpdate(Clock& new_time) {

    /*
    WHAT THIS METHOD DOES
    1. Sets its done boolean to false
    2. Updates time
    3. Calculates time change (useful for later calculations)
    4. Prints log (temporary)
    5. Makes decsion based on status by calling plane status
    6. If the current tine is the start of a new day, report daily cost and revenue
    7. Sets done boolean to true
    */

    //Start by setting done to false
    TimeObserver::setIsNotDone();

     //Find duration of update
    this->duration=findDuration(new_time);

    //Update the plane's time
    Objects_clock = new_time;

    if(debugging){
    cout << "Plane " << Plane_ID << " updated its time to "
              << new_time.hours << ":" << new_time.minutes << ":" << new_time.seconds << endl;
    cout << "Variables: " << Plane_model << " " << Max_fuel << " " << Burn_rate << " " << Max_passengers << " " << Current_velocity << " " << Odometer << endl;
    }
    //Check status and make decison
    planeStatus();


    //Say we are done
    TimeObserver::setIsDone();
}
void Plane::updateDay(int Day){
    this->day = Day;

    this->Is_ready_for_assignment = true;

    //TEMP Reset values for the day so every plane is again in a waiting state
    this->isFlying = false; 
    this->isGrounded = true; 
    this->isMaintenance = false; 
    this->isWaiting = true; 
    this->isBoarding = false; 
    this->isUnboarding = false; 
    this->isAboutToTakeoff = false;

    //For now, we say maintence is done
    untilMaintDone = 0;



}
void Plane::setLogObject(Logger *log_pointer){

    //Assign our logger object
    this->Log_object = log_pointer;
    
}

//void Plane::setAirportObject(Airport *airport_pointer)
//{
//    this->Airport_object = airport_pointer;
//}

double Plane::findDuration(Clock& new_time){

    if (new_time.hours < 0 || new_time.minutes < 0 || new_time.seconds < 0) 
    {
        Log_object->errorLog(1, "Error! Invalid time parameter provided for duration calculation [plane.cpp][Line 119]");
    }

    //Getting the difference in hours, minutes, and seconds 
    int diff_hours = new_time.hours - Objects_clock.hours;
    int diff_minutes = new_time.minutes - Objects_clock.minutes;
    int diff_seconds = new_time.seconds - Objects_clock.seconds; 

    //converting and storing the time difference to be referenced. 
    double duration = diff_hours * 60.0 + diff_minutes + diff_seconds/ 60.0;

    return duration;
}
void Plane::planeStatus(){
   

    if(isFlying){
        //Want to start by checking if it is flying
        fly();     
    }
    else if(isGrounded){
        //See why we are grounded
        if(isWaiting){
            //Decrement time in waiting
            inWaitingTime();
            
        }     
        else if(isBoarding){
            if(Gate_delay.minutes > 0){

                //Send a decrement of the given interval
                decrementDelay(0);
            }
            else{
                //Board passengers
                boardPassengers();
            }             
        }
        else if(isAboutToTakeoff){
            //Try to takeoff
            if(Grounded_delay.minutes > 0){

                //Send a decrement of the given interval
                decrementDelay(1);
            }
            else{
                //Board passengers
                goTakeOff();
            }             
        }
        else if(isUnboarding){
            //Unboard passengers
            disembarkPassengers();
        }
        else if(isMaintenance){
            //TEMP = Is down for maintence for the day
            doMaintenance();
        }
        else{
            Log_object->errorLog(1, "Error! Invalid plane state (Grounded tree) [plane.cpp][Line 155]");
        }
    }
    else{
        Log_object->errorLog(1, "Error! Invalid plane state [plane.cpp][Line 159]");
    }

    
}

void Plane::fly(){

    if(debugging){
        cerr << "Entering [fly]" << endl;
    }

    // Decrement fuel based on flight duration
    checkFuelLevel(); 
     
    //Decrement distance based on flight duration. Trip Odometer represents How many miles the plane has flown in its current flight.
    double distanceTraveled = this->Current_velocity * (this->duration / 60.0); 

    //Set our odometer values
    this->Odometer += distanceTraveled;
    this->Trip_odometer += distanceTraveled;


    //Change our distance to target
    this->Target_airport_location_distance = this->Target_airport_location_distance - distanceTraveled;

    if(debugging){
        cout << "Plane " << Plane_ID <<" is flying "<< Objects_clock.hours <<":"<< Objects_clock.minutes << endl;
    }

    if(debugging){
        cout << "PLANE " << Plane_ID << " Current velocity: " << Current_velocity << endl;
        cout << "PLANE " << Plane_ID << " distance TRAVELED: " << distanceTraveled << endl;
        cout << "PLANE " << Plane_ID << " distance to target: " << Target_airport_location_distance << endl;
    }

    // Check if the distance has hit zero
    if (this->Target_airport_location_distance <= 0) {
        //Land and prepare to unboard
        goLanding();
    }

    if(debugging){
        cerr << "Exiting [fly]" << endl;
    }   
        
}
void Plane::goTakeOff(){
    
    //Top off fuel just in case
    topOffTank();

    //Send log of departure
    Log_object->logPlaneUpdate(this->Plane_ID, 4, this->Objects_clock);

    //Set is flying to ture for NEXT update
    this->isAboutToTakeoff = false; 
    this->isFlying = true;

    //TEMP
    //Set velocity
    this->Current_velocity = this->Max_velocity;
    
}
void Plane::goLanding(){

    if(debugging){
        cerr << "Entering [goLanding]" << endl;
    }
    
    //Begin landing process
    //-----------------------

    //First, negotiate a gate
    // 2 - Selection means target airport
    negotiateGate(2);

    //Send log message
    Log_object->logPlaneUpdate(this->Plane_ID,5,this->Objects_clock);

    this->isFlying = false;
    this->Current_velocity = 0;

    //Set docked and unboarding for NEXT update
    this->isUnboarding = true;

    //Refuel
    topOffTank();

    if(debugging){
        cerr << "Exiting [goLanding]" << endl;
    }
    
}
void Plane::boardPassengers(){

    if(debugging){
        cerr << "Entering [boardPassengers]" << endl;
    }

    //Negotiate a gate
    // 1 = Origin airport
    negotiateGate(1);

    //Send boarding message to logger
    Log_object->logPlaneUpdate(this->Plane_ID,2,this->Objects_clock);

    //INSTEAD - Call the airline transferToPlane method here
    this->Onboard = Origin_airport_object->transferToPlane(this->Origin_gate_ID);

    //Disable booleans
    isBoarding = false;
    
    //Free this origin gate
    Origin_airport_object->freeGate(this->Origin_gate_ID);

    //For now, we set our boolean to take off next tick
    isAboutToTakeoff = true;


     if(debugging){
        cerr << "Exiting [boardPassengers]" << endl;
    }
}
void Plane::disembarkPassengers(){

    if(debugging){
        cerr << "Entering [disembarkPassengers]" << endl;
    }

    //Send unboarding message to logger
    Log_object->logPlaneUpdate(this->Plane_ID,3,this->Objects_clock);
    
    
    if(debugging){
        int garbage;
        cerr << "CHECK" << endl;
        cerr << "Target airport obj: " << Target_airport_object << endl;
        cerr << "Target GateID: " << Target_gate_ID << endl;
        cerr << "Number of passenger groups: " << Onboard.size() << endl;
        //cin >> garbage;
    }

    Finance_obj->reportPlaneRevenue(this->Plane_ID, Onboard.size());

    Target_airport_object->transferToGate(this->Target_gate_ID);

    Trip_odometer = 0; //Resetting the trip odometer back to 0


    //Plane is waiting for next assignment
    isWaiting = true;
    Is_ready_for_assignment = true;
    isUnboarding = false;

    //Important step! We let airport know this gate is now availible
    //Free the target gate up
    Target_airport_object->freeGate(this->Target_gate_ID);

    if(debugging){
        cerr << "Exiting [disembarkPassengers]" << endl;
    }

}
void Plane::inWaitingTime(){
    //This is a temp fix, but for right now plane is going to wait until 10 min before
    // takeoff then board, then fly
    //The reason for this is waiting is its base state
    //cout << "Plane " <<  Plane_ID << " is in waiting state." << endl;

    //Will transition to flying if..
    // 1. It has been assigned a upcoming flight (ready for assignment = false)
    // 2. It is time to leave (departure time check)
    if((this->Objects_clock >= Departure_time) && (this->Is_ready_for_assignment == false)){
        //Go board passengers
        isBoarding = true;
        isWaiting=false;
        
    }

}
void Plane::assignFlight(int origin_airport_ID,int target_airport_ID, Clock arrivalTime, Clock departTime, 
    double distance, Airport* origin_airport_pointer, Airport* target_airport_pointer){
    //Assign our old flight target ID to be our new origin
    //this->Origin_airport_ID = this->Target_airport_ID;

    //Assign values passed in 
    this->Origin_airport_ID = origin_airport_ID;
    this->Target_airport_ID = target_airport_ID;
    this->Arrival_time = arrivalTime;
    this->Departure_time = departTime;
    this->Target_airport_location_distance = distance;

    //Set our airport pointers
    this->Origin_airport_object = origin_airport_pointer;
    this->Target_airport_object = target_airport_pointer;

    //Flip assigned to false
    this->Is_ready_for_assignment = false;
}

void Plane::negotiateGate(int selection){
    int tempGateID = -1;
    int i = 0;
    bool gate_found = false;

    //Error catch for else
    //Selection = 1 sets origin = 2 sets target
    if(Origin_airport_object && Target_airport_object){
        if(selection == 1){
            //Find gate for origin airport
            while ((i < Origin_airport_object->All_gates.size()) && !gate_found){

                //Get the gate ID
                tempGateID = Origin_airport_object->All_gates[i]->getGateID();

                //Make sure gateID matches i
                if(tempGateID != i){
                    Log_object->errorLog(1, "Error! Iterator does not match tempGateID [PLANE.CPP][LINE 378]");
                }
        
                //See if gate is open
                if(Origin_airport_object->All_gates[i]->getInUse() == false){
                    //TEMP - For now we just want to grab the first availible gate
                    setOriginGate(i);

                    //Change gate to in use
                    Origin_airport_object->All_gates[i]->setInUse(true);

                    //Tell logger we reserved a gate
                    Log_object->logAirportUpdate(Origin_airport_object->getAirportID(), 1, i, this->Objects_clock);

                    //Set boolean to true
                    gate_found = true;
                }
                
                i++;
            }

            //If we get to the end of this without finding a gate
            if(!gate_found){
                Log_object->errorLog(1, "Error! No suitable gate found! [PLANE.CPP][LINE 401]");
            }

        }
        else if(selection == 2){
            //Find gate for target airport
            while ((i < Target_airport_object->All_gates.size()) && !gate_found){

                //Get the gate ID
                tempGateID = Target_airport_object->All_gates[i]->getGateID();

                //Make sure gateID matches i
                if(tempGateID != i){
                    Log_object->errorLog(1, "Error! Iterator does not match tempGateID [PLANE.CPP][LINE 414]");
                }
        
                //See if gate is open
                if(Target_airport_object->All_gates[i]->getInUse() == false){
                    //TEMP - For now we just want to grab the first availible gate
                    setOriginGate(i);

                    //Change gate to in use
                    Target_airport_object->All_gates[i]->setInUse(true);

                    //Tell logger we reserved a gate
                    Log_object->logAirportUpdate(Target_airport_object->getAirportID(), 1, i, this->Objects_clock);

                    //Set boolean to true
                    gate_found = true;
                }
                
                i++;
            }

            //If we get to the end of this without finding a gate
            if(!gate_found){
                Log_object->errorLog(1, "Error! No suitable gate found! [PLANE.CPP][LINE 437]");
            }
        }
        else{
            Log_object->errorLog(0, "Error! Bad negotiation selection [PLANE.CPP][Line 371]");
        }

    }
    else{
        Log_object->errorLog(1, "Error! Origin or Target Airport object not set! [PLANE.CPP][Line 376]");
    }
    
}

void Plane::checkFuelLevel(){

    if(isFlying == true){
        if(debugging){
            cout <<"Fuel level currently is: " << this->Fuel_tank << endl;
        }

        double fuelused; 
        
        
        if(Plane_model == "B600" || Plane_model == "B800"){
        Burn_rate=3217;
        } 
        else if(Plane_model == "A100"){
        Burn_rate=2479;
        }
        else if(Plane_model == "A300"){
        Burn_rate=2600;
        }
        else{
            Log_object->errorLog(1, "Plane model invalid");
        }
        fuelused = this->duration * (Burn_rate/60.0);
        this->Fuel_tank -= fuelused;
//        Finance_obj->reportPlaneCost(Plane_ID, ((fuelused*0.264172)*FUELCOST)); //Reports the cost of fuel used to the Finance object, converting liters of fuel to gallons since we're using cost-per-gallon //MOVED TO TopOffTank()
        
        if (Fuel_tank <= 0){
            Fuel_tank=0;
            Log_object->errorLog(0, " Fuel Tank is empty. Refuel before takeoff");
        }
        else{
            //cout << "Fuel used" << fuelused << endl;
            //cout << " Fuel Tank Level: " << this->Fuel_tank << "liters" << endl;
        }


    }
    
}
void Plane::topOffTank(){
    if(this->Fuel_tank != Max_fuel){
        Finance_obj->reportPlaneCost(Plane_ID, (((Max_fuel - Fuel_tank)*0.264172)*FUELCOST)); //Reports the cost of fuel used to refill the plane to the Finance object, converting liters of fuel to gallons since we're using cost-per-gallon //TODO: Double-check/test the math on this
        this->Fuel_tank = Max_fuel;

        Log_object->logPlaneUpdate(this->Plane_ID, 7, this->Objects_clock);
    }
}

   /* BEGIN GETTERS */

int Plane::getPlaneID(){
    return this->Plane_ID;
}
string Plane::getPlaneName(){
    return this->Plane_name;
}
string Plane::getPlaneModel(){
    return this->Plane_model;
}
double Plane::getMaintenance()
{

    return this->Until_maint;
}

double Plane::getOdometer()
{

    return this->Odometer;
}

double Plane::getTripOdometer()
{

    return this->Trip_odometer;
}

bool Plane::getIsReadyForAssignment()
{

    return this->Is_ready_for_assignment;
}

bool Plane::getIsOperable()
{

    return this->Is_operable;
}

bool Plane::getIsSeatsOpen()
{

    return this->Is_seats_open;
}

string Plane::getLocation() //TODO: This needs to return an actual location
{

    return "TEMPLATE";
}

float Plane::getFuelLevel()
{

    return this->Fuel_tank;
}

double Plane::getDailyCost()
{

    return this->Daily_cost;
}

int Plane::getTargetGate()
{

    return this->Target_gate_ID;
}

int Plane::getTargetAirport()
{

    return this->Target_airport_ID;
}

int Plane::getPassengerCount()
{

    return this->Count_of_passengers;
}
   /* END GETTERS */

   /* BEGIN SETTERS*/
void Plane::setPassengerVectorFromAirport(const vector<Passenger*>& allPassengers) {
    passenger_vector = allPassengers;
}
void Plane::resetTripOdometer()
{
     this->Trip_odometer = 0;
}

void Plane::setFuelTank(float fuel)
{
     this->Fuel_tank = fuel;
}   

void Plane::refuelToFull()
{
     this->Fuel_tank = this->Max_fuel;
//     Finance_obj->reportPlaneCost(Plane_ID, (((Max_fuel - Fuel_tank)*0.264172)*FUELCOST)); //Reports the cost of fuel used to refill the plane to the Finance object, converting liters of fuel to gallons since we're using cost-per-gallon
}   

void Plane::setMaintStatus(bool maintStatus)
{
    this->isMaintenance = maintStatus;
}

void Plane::setArrivalTime(Clock newTime){
    this->Arrival_time = newTime;
}

void Plane::setDepartureTime(Clock newTime){
    this->Departure_time = newTime;
}
void Plane::setIsReadyForAssignment(bool isReady){
    this->Is_ready_for_assignment = isReady;
}
void Plane::setTargetGate(int gate){
    this->Target_gate_ID = gate;
}   
void Plane::setOriginGate(int gate){
    this->Origin_gate_ID = gate;
} 

void Plane::setTargetAirport(int airportID){
    this->Target_airport_ID = airportID;
}

void Plane::setFinanceObject(Finance *New_finance_obj){
   this->Finance_obj = New_finance_obj;
}

    /* END SETTERS*/

    /* BEGIN MISCELLANEOUS FUNCTIONS */
/*double*/ void Plane::calcCost() //TODO: As-is, this doesn't actually calculate anything, just reports to Finance object
{
   Finance_obj->reportPlaneCost(Plane_ID, 10); //TODO: Pass an actual value to ReportPlaneCost, 10 is just a placeholder
}

void Plane::doMaintenance()
{
    //WE NEED TWO DIFFERENT METHODS FOR MAINTENCE
    //1. Sets maintence time, and sets maintence to true
    //2. Chips away at maintence time then eventually sets to false

    //If in maintenance, remove 10 minutes from the timer. If the timer hits zero, set the plane to no longer be in maintenance.
    //Note: This is about 36 hours, so once it goes down it goes down for the day
    if (untilMaintDone > 0)
    {
       untilMaintDone -= 10;
       if (untilMaintDone <= 0)
       {
          setMaintStatus(false);

          this->Is_ready_for_assignment = true;
       }
    }

}
void Plane::sendToMaintenance(){
     this->untilMaintDone = 2160;
     this->isMaintenance = true;

     //We are not ready for assignment
     this->Is_ready_for_assignment = false;

     //Set booleans
     this->isGrounded = true;
     this->isFlying = false;
     this->isBoarding = false;
     this->isUnboarding = false;
     this->isWaiting = false;
     this->isAboutToTakeoff = false;
}
    /* END MISCELLANEOUS FUNCTIONS */

void Plane::addDelay(int selection, int delay_min){
    //Based on selection, add delay to correct delay clock
    if(selection == 0){
        //Selectin for delay AT gate
        Gate_delay.minutes = Gate_delay.minutes + delay_min;

        //Account for borrowing
        if(Gate_delay.minutes >= 60){
            Gate_delay.minutes = Gate_delay.minutes - 60;
            Gate_delay.hours = Gate_delay.hours + 1;
        }
    }
    else if(selection == 1){
        //Selection for delay NOT at gate
        Grounded_delay.minutes = Grounded_delay.minutes + delay_min;

        //Account for borrowing
        if(Grounded_delay.minutes >= 60){
            Grounded_delay.minutes = Grounded_delay.minutes - 60;
            Grounded_delay.hours = Grounded_delay.hours + 1;
        }
    }
    else{
        Log_object->errorLog(0, "Error! Bad delay selection [PLANE.CPP][Line 509]");
    }
}
void Plane::decrementDelay(int selection){

    //TO DO
    //We need to convert this to using purley clock objects

    //Based on selection, add delay to correct delay clock
    if(selection == 0){
        //Selectin for delay AT gate
        Gate_delay.minutes = Gate_delay.minutes - duration;

    }
    else if(selection == 1){
        //Selection for delay NOT at gate
        Grounded_delay.minutes = Grounded_delay.minutes - duration;
    }
    else{
        Log_object->errorLog(0, "Error! Bad decrement selection [PLANE.CPP][Line 509]");
    }
}
