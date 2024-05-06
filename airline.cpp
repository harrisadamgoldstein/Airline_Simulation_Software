/*
airline.cpp

*/
// Airline.cpp
#include "airline.h"

// Constructor
//We assign the time manager here, so we can use it to register new observers
Airline::Airline(TimeManager *time_manager, string airline_name, Input Input_object, Logger *Log_object, Finance *Finance_obj)
        : time_manager(time_manager),  Objects_clock(0, 0, 0), Input_object(Input_object), Finance_obj(Finance_obj)
        , Log_object(Log_object) {
    this->Airline_name = airline_name;

    // ---------------------
    // Receive from input object/Register objects
    // ---------------------

    //Get plane vector from input object
    All_planes = Input_object.get_plane_vector();
    if(All_planes.empty()){
        Log_object->errorLog(2, "Critical Error! Plane vector is empty![AIRLINE.CPP][LINE 22]");
    }
    for(int j=0; j<All_planes.size(); j++){
        //Register with time manager
        registerPlane(All_planes[j]);
    }


    //Get airport vector from input object
    All_airports = Input_object.get_airport_vector();
    if(All_airports.empty()){
        Log_object->errorLog(2, "Critical Error! Airport vector is empty![AIRLINE.CPP][LINE 33]");
    }
    for(int j=0; j<All_airports.size(); j++){
        //register with time manager
        registerAirport(All_airports[j]);
    }

    //Add this object to be a timeObsever
    time_manager->addObserver(this);

    //Load flights and fill in missing info based off airport and plane data
    loadFlights();

    //Send flight vector pointer to finance
    Finance_obj->setFlightVector(&All_flights);

}

//Destructor
Airline::~Airline(){
    
}

//Register a plane as an observer
void Airline::registerPlane(Plane* plane) {
    //Add this object as an observer
    time_manager->addObserver(plane);

    //Fill its logger slot
    plane->setLogObject(Log_object);

    //Fill its finance obj slot
    plane->setFinanceObject(Finance_obj);
}

//Register an airport likewise
void Airline::registerAirport(Airport* airport) {
    //Add this object as an observer
    time_manager->addObserver(airport);

    //Fill its logger slot
    airport->setLogObject(Log_object);
}


//Schedule flights
void Airline::scheduleFlights(){
    //Want to acsess flight vector, send out flight info to all objects like 
    // planes and airports, then flip scheduled boolean to true

    //Move through vector, checking to see if its been scheduled, then schedule it
    //Set our temporary variables
    int tempDestID = -1, tempOriginID = -1;
    int planePositionInArray = 0;

    string plane_type = "";
    Clock tempArrivalTime, tempDepartTime;
    double tempDistance;

    //How many flights left?
    int pending_flights = 0;

    //Pointer to Target Airport
    Airport* Target_aiport_pointer = nullptr;
    //Pointer to Origin Airport
    Airport* Origin_aiport_pointer = nullptr;

    for(int i = 0; i < All_flights.size(); i++){
        //Check to see if the flight has already been scheduled
        if(All_flights[i]->getScheduled() == false){

            //STEPS
            //1. Iterate through flights
            //2. If !scheduled, loop through all planes of that type to see if availible
            //3. Grab first availible, assign flight
            //4. If time has already passed, set departure time to current time (modify both flight and plane here)
            //4. Call flight assignment on plane
            //5. Flip to scheduled (assignFlight takes care of saying the plane itsef is scheduled)
            //6. Print out the pending number of flights to log

            //Find out what plane is needed
            plane_type = All_flights[i]->getPlaneName();

            if(debugging){
                cerr << "Plane Type required: " << plane_type << endl;
            }

            //Variable declaration
            int j = 0;
            bool planeFound = false;

            //Iterate through all planes, finding matching plane type, and availible
            while((j < All_planes.size()) && !planeFound){

                //Check if plane is ready to receive a new assignment
                if((All_planes[j]->getIsReadyForAssignment()==true) && (All_planes[j]->getPlaneModel() == plane_type)){

                    
                    //Get our relevant information from flight
                    tempDestID = All_flights[i]->getDestAirptID();
                    tempOriginID = All_flights[i]->getOriginAirptID();
                    tempArrivalTime = All_flights[i]->getArrivalTime();
                    tempDepartTime = All_flights[i]->getDepartureTime();
                    tempDistance = All_flights[i]->getDistance();

                    if(debugging){
                        cerr << "DestID: " << tempDestID << "OriginID: " << tempOriginID << endl;
                    }

                    //Set our airport pointers
                    Target_aiport_pointer = All_airports[tempDestID];
                    Origin_aiport_pointer = All_airports[tempOriginID];

                    if(debugging){
                        cerr << "About to negotiate gate " << endl;
                    }

                    //Negotiate a destination and origin gate
                    // MOVED TO GATE
                    //negotiateGate(tempOriginID, tempDestID, j);

                    //Assign receieved values to plane, and set ready to assign as false
                    All_planes[j]->assignFlight(tempOriginID, tempDestID, tempArrivalTime, tempDepartTime, tempDistance, Origin_aiport_pointer, Target_aiport_pointer);

                    //Assign delay values (if there are any)
                    All_planes[j]->Gate_delay.minutes = All_flights[i]->getGateDelay();
                    All_planes[j]->Grounded_delay.minutes = All_flights[i]->getGroundedDelay();

                    //Set this flight in vector to scheduled
                    All_flights[i]->setScheduledTrue();

                    //Set planeID for flight
                    All_flights[i]->setPlaneID(j);

                    //Send scheduled notice to Logger
                    //Big nasty call, but gives flight info
                    Log_object->logFlightUpdate(All_flights[i]->getFlightID(), 4, All_flights[i]->getOriginAirptName(),  All_flights[i]->getDestAirptName(), tempDepartTime, tempArrivalTime);

                     if(debugging){
                        cerr << "Plane found and taken care of! ending loop " << endl;
                    }

                    //Break loop
                    planeFound = true;
 
                }

                //Increment j
                j++;

            }    
                
        }
            
                     
    }

    if(debugging){
                cerr << "Done with scheduling flights "  << endl;
            }        
}

//Cycle through planes and return matching ID, or if failure -1
int Airline::findPlaneID(string plane_name){

    //Iterator
    int i = 0;

    if(debugging)
    {
        cout << "DEBUGGING: findPlaneID, plane_name = " << plane_name << endl;
    }

    //Iterate through all planes, return position when found
    while (i < All_planes.size()){

        if(debugging)
        {
            cout << "DEBUGGING: getPlaneName() call returns: " << All_planes[i]->getPlaneName() << ", Index: " << i << endl;
        }


        if(All_planes[i]->getPlaneName() == plane_name){

            if(debugging)
            {
                cout << "DEBUGGING: Found! Plane name: " << All_planes[i]->getPlaneName() << ", Index: " << i << endl;
            }

            return i;
        }
        else
            i++;
    }

    //If ID doesn't exist
    return -1;
}
int Airline::findAirportID(string airport_name){
    //Iterator
    int i = 0;

    if(debugging)
    {
        cout << "DEBUGGING: findAirportID, airport_name = " << airport_name << endl;
    }

    //Iterate through all planes, return position when found
    while (i < All_airports.size()){

        if(debugging)
        {
            cout << "DEBUGGING: getAirportName() call returns: " << All_airports[i]->getAirportName() << ", Index: " << i << endl;
        }


        if(All_airports[i]->getAirportName() == airport_name){
            
            if(debugging)
            {
                cout << "DEBUGGING: Found! Airport name: " << All_airports[i]->getAirportName() << ", Index: " << i << endl;
            }
            
            return i;
            
        }
        else
            i++;
    }


    //If ID doesn't exist
    return -1;
}

void Airline::negotiateGate(int O_airport_ID, int D_airport_ID, int plane_ID){

    int tempGateID = -1;
    int i = 0;
    bool gate_found = false;

    //Find gate for origin airport
    while ((i < All_airports[O_airport_ID]->All_gates.size()) && !gate_found){

        //Get the gate ID
        tempGateID = All_airports[O_airport_ID]->All_gates[i]->getGateID();

        //Make sure gateID matches i
        if(tempGateID != i){
            Log_object->errorLog(1, "Error! Iterator does not match tempGateID [AIRLINE.CPP][LINE 228]");
        }
        
        //See if gate is open
        if(All_airports[O_airport_ID]->All_gates[i]->getInUse() == false){
            //TEMP - For now we just want to grab the first availible gate
            All_planes[plane_ID]->setOriginGate(i);

            //Change gate to in use
            All_airports[O_airport_ID]->All_gates[i]->setInUse(true);

            //Tell logger we reserved a gate
            Log_object->logAirportUpdate(O_airport_ID, 1, i, this->Objects_clock);

            //Set boolean to true
            gate_found = true;
        }
        i++;
    }

    //If we get to the end of this without finding a gate
    if(!gate_found){
        Log_object->errorLog(1, "Error! No suitable gate found! [AIRLINE.CPP][LINE 247]");
    }

    //---------------------------------------
    //Do again but for target airport
    tempGateID = -1;
    i = 0;
    gate_found = false;

    //Iterate through all gates of a given airport
    while ((i < All_airports[D_airport_ID]->All_gates.size()) && !gate_found){

        //Get the gate ID
        tempGateID = All_airports[D_airport_ID]->All_gates[i]->getGateID();

        //Make sure gateID matches i
        if(tempGateID != i){
            Log_object->errorLog(1, "Error! Iterator does not match tempGateID [AIRLINE.CPP][LINE 228]");
        }
        
        //See if gate is open
        if(All_airports[D_airport_ID]->All_gates[i]->getInUse() == false){
            //TEMP - For now we just want to grab the first availible gate
            All_planes[plane_ID]->setTargetGate(i);

            //Change gate to in use
            All_airports[D_airport_ID]->All_gates[i]->setInUse(true);

            //Tell logger we reserved a gate
            Log_object->logAirportUpdate(D_airport_ID, 1, i, this->Objects_clock);

            //Set boolean to true
            gate_found = true;
        }
        i++;
    }

    
    //If we get to the end of this without finding a gate
    if(!gate_found){
        Log_object->errorLog(1, "Error! No suitable destination gate found! [AIRLINE.CPP][LINE 247]");
    }
}

void Airline::loadFlights(){

    //Steps
    // 1. Establishes Dest Airport ID from name
    // 2. Establishes Origin ID from name
    // 3. Sorts using bubble sort flights based on departure time


    //Variable holders
    string temp_name = "";
    int tempID = -1;

    //Get flight vector from input object
    All_flights = Input_object.get_flight_vector();

    if(All_flights.empty()){
       Log_object->errorLog(2, "Critical Error! Flight vector is empty![AIRLINE.CPP][LINE 245]");
    }
   
    //Calculate remaining information based on airport and plane data
    for(int i=0; i<All_flights.size(); i++){
        //Debugging statement
        if(debugging){
            cout << "DEBUGGING: " << All_flights[i]->getPlaneName() << " To: " << 
            All_flights[i]->getDestAirptName() << " From: " << All_flights[i]->getOriginAirptID() << endl;
        }
        //Discover and set ID of origin airport
        temp_name = All_flights[i]->getOriginAirptName();
        tempID = findAirportID(temp_name);
        All_flights[i]->setOriginAirptID(tempID);

        if(debugging){
            cout << "DEBUGGING: Origin airport name: " << temp_name << ", Origin Airport ID" << tempID << endl;
        }

        //Discover and set ID of dest airport
        temp_name = All_flights[i]->getDestAirptName();
        tempID = findAirportID(temp_name);
        All_flights[i]->setDestAirptID(tempID);

        if(debugging){
            cout << "DEBUGGING: Dest airport name: " << temp_name << ", Dest Airport ID" << tempID << endl;
        }

    }

    //-------------------------
    //Sorting algorithim to sort based on depature time
    bool swapped;
    for(int i = 0; i < All_flights.size(); i++){
        swapped = false;   
        for(int j = 0; j<(All_flights.size() - i - 1); j++){
            if(All_flights[j]->Departure_time.hours > All_flights[j+1]->Departure_time.hours){
                swapFlights(j, j+1);
                swapped = true;
            }
        }

    }


        //USED TO BE DEBUGGING, NOW STANDARD
        //If debugging, print out our new vector
        string garbageValue;
        printFlightVector();
        cerr << "-----------------------------------------------" << endl;
        cerr << "Current flight vector, enter anything to confim" << endl;
        cin >> garbageValue;
    
    //Give logger information about flights
    Log_object->updateFlightVector(All_flights);
    
}

void Airline::swapFlights(int firstID, int secondID){
    
    //Swap their ID's first
    All_flights[secondID]->setFlightID(firstID);
    All_flights[firstID]->setFlightID(secondID);

    //Swap the two entires
    swap(All_flights[firstID], All_flights[secondID]);


}

void Airline::setComplication(int selection){

    if(debugging){
        cerr << " DEBUGGING Entering set complication" << endl;
    }

    //Error catch
    if(All_flights.empty()){
        Log_object->errorLog(2, "Error! Flight vector empty! [AIRLINE.CPP][LINE 390]");
    }

    if(selection == 0){
        //This is NO COMPLICATION
        return;
    }
    else if(selection == 1){
        //Bad weather
        //25% of flights encounter a delay while in the air
        //To handle this, we will increase the distance traveled to up to 15% of distance
        int num_of_flights_effected;
        double new_distance, percent_inc;

        //Get 25% of all flights
        num_of_flights_effected = All_flights.size() / 4;

        //Iterate through the first 25% of flights
        for(int i=0; i<num_of_flights_effected; i++){

            if(debugging){
                cerr << "DEBUGGING In flight iterations" << endl;
            }

            //Get original distance
            new_distance = All_flights[i]->getDistance();

             if(debugging){
                cerr << "DEBUGGING Old distance: " << new_distance << " FlightID: " << i << endl;
            }

            srand(time(nullptr));

            //Increase between 1 and 15
            percent_inc = (rand() % 15 + 1.0)/ 100.0;

            //Add increase
            new_distance = new_distance + (new_distance * percent_inc);

            //Call setter value
            All_flights[i]->setDistance(new_distance);

            //Tell Logger
            Log_object->logComplication(i, "Has been delayed due to bad weather.");

             if(debugging){
                string test;
                cerr << "DEBUGGING New distance: " << new_distance << " Percent Inc: " << percent_inc * 100 << "%" << endl;
                cerr << "DEBUGGING end loop flight iterations" << endl;
                cin >> test;
            }

        }

    }
    else if(selection == 2){
        //Icing
        //20% north of 40 degrees are delayed due to icing
        //TODO this one's bugged we dont hit the debugging statement at the end

        if(debugging){
                cerr << "DEBUGGING Entering selection 2" << endl;
        }

        int tempAirID = -1;
        double delay = 0;

        //Random value
        srand(time(nullptr));


        //Get all ID's above 40 degrees
        vector<int> IDarray;

        //Find all matching flights
        for(int i = 0; i<All_flights.size(); i++){
            //Get the ID of that origin airport
            tempAirID = All_flights[i]->getOriginAirptID();

            //Error catch
            if(tempAirID == -1){
                Log_object->errorLog(2, "Critical error! Airport not found in flight entry [AIRLINE.CPP][LINE 507]");
            }

            //Find out if that airport is above 40 degrees
            if(All_airports[tempAirID]->getLat() >= 40){
                //Add this flight ID to our array
                IDarray.push_back(i);
            }
        }

        //Loop through 20% of these to delay
        for(int i = 0; i < (IDarray.size() / 5); i++){
        
            //Random time between 10 and 45 min
            delay = (rand() % 45 + 10);

            //Set grounded delay for when flight is scheduled
            All_flights[IDarray[i]]->setGroundedDelay(delay);

            //Tell logger
            Log_object->logComplication(i, "Has been delayed due to icing.");

             if(debugging){
                string test;
                cerr << "DEBUGGING Delay given at gate: " << delay << endl;
                cerr << "DEBUGGING end loop flight iterations" << endl;
                cin >> test;
            }
        }

         if(debugging){
                cerr << "DEBUGGING Exiting selection 2" << endl;
        }
    }
    else if(selection == 3){
        //Jet stream
        //Due east extended by 12%, west DECREASED by same amount
        //TODO figure out what is DUE east versus kinda east

        int tempOID, tempDID;
        double new_distance, percent_inc;

        //Iterate through the all flights
        for(int i=0; i<All_flights.size(); i++){

            if(debugging){
                cerr << "DEBUGGING In flight iterations" << endl;
            }

            //Get original distance
            new_distance = All_flights[i]->getDistance();

             if(debugging){
                cerr << "DEBUGGING Old distance: " << new_distance << " FlightID: " << i << endl;
            }

            //Our percent increase will always be 12%
            percent_inc = 0.12;

            //Find out where this flight is travelling
            tempOID = All_flights[i]->getOriginAirptID();
            tempDID = All_flights[i]->getDestAirptID();

            //See if we are moving east or west
            if(All_airports[tempDID]->getLong() > All_airports[tempOID]->getLong()){
                //We are traveing east

                //Add distance
                new_distance = new_distance + (new_distance * percent_inc);

                //Call setter value
                All_flights[i]->setDistance(new_distance);

                //Tell Logger
                Log_object->logComplication(i, "Has been delayed due to Jet Stream.");
            }
            else{
                //Travelling west
                //DECREASE distance
                new_distance = new_distance - (new_distance * percent_inc);

                //Call setter value
                All_flights[i]->setDistance(new_distance);

                //Tell Logger
                Log_object->logComplication(i, "Has been hastened due to Jet Stream.");
            }

             if(debugging){
                string test;
                cerr << "DEBUGGING New distance: " << new_distance << " Percent Change: " << percent_inc * 100 << "%" << endl;
                cerr << "DEBUGGING end loop flight iterations" << endl;
                cin >> test;
            }

        }


    }
    else if(selection == 4){
        //Gate delay
        //5% of flights delayed by 5-90 min
        int num_of_flights_effected = 0;
        double time_inc;

        //random value
        srand(time(nullptr));

        //Get 5% of all flights
        num_of_flights_effected = All_flights.size() / 20;


        //Iterate through the first 5% of flights
        for(int i=0; i<num_of_flights_effected; i++){

            if(debugging){
                cerr << "DEBUGGING In flight iterations" << endl;
            }

            //Delay between 5 and 90 min
            time_inc = (rand() % 90 + 5);

            //Establish delay
            All_flights[i]->setGateDelay(time_inc);

            //Tell logger
            Log_object->logComplication(i, "Has been delayed at gate.");


             if(debugging){
                string test;
                cerr << "DEBUGGING Delay given at gate: " << time_inc << endl;
                cerr << "DEBUGGING end loop flight iterations" << endl;
                cin >> test;
            }

        }
    }
    else if(selection == 5){
        //Aircraft failure
        //For now, first aircraft in registry taken out of commison of the day
        int i = 0;
        All_planes[i]->sendToMaintenance();

        //Tell Logger
        Log_object->logPlaneUpdate(i, 6, this->Objects_clock);
    }
    else if(selection == 6){
        //West of 103 cancled
    }
    else{
        Log_object->errorLog(1, "Error! Complication input invalid [AIRLINE.CPP]{LINE 373}");
    }

     if(debugging){
        cerr << "Entering set complication" << endl;
    }

}

void Airline::updateDay(int Day){

    if(debugging){
        cerr << "DEBUGGING In update day" << endl;
    }

    //----------------------
    //Get ready for new day

    //Set new day
    this->day = Day;

    //Reset flights
    // This call resets, delays, distance, and scheduled status
    for(int i=0; i<All_flights.size(); i++){
        All_flights[i]->resetValues();
    }

    //---------------------
    //Logger neatness

    //Print out day divison in respective .txt files
    Log_object->newDayMarking(Day);

    //--------------------------
    //User complication selection

    //Reset selection
    string selection = "";

    bool boolTest = false;

    //Take use input
    while( (selection != "0") && (selection != "1") && (selection != "2") && (selection != "3")
        && (selection != "4") &&  (selection != "5")){

            if(selection == "9"){
                cout << "-----------------------------------" << endl;
                cout << "(0) No complication" << endl;
                cout << "(1) 25% Of flights encounter bad weather" << endl;
                cout << "(2) 20% of flights originating 40 degrees north delayed due to ice" << endl;
                cout << "(3) Strong jet stream (East/West)" << endl;
                cout << "(4) 5% of flights get random delay at gate" << endl;
                cout << "(5) Plane taken out of commision" << endl;
            }
            cout << "------------------------------------" << endl;
            cout << "//   AIRLINE SIMULATION   //////////" << endl;
            cout << "//   Current Day in simulation : " << day << endl;
            cout << "------------------------------------" << endl;
            cout << "Enter complication selection...     " << endl;
            cout << "------------------------------------" << endl;
            cout << "Enter (9) for option listing" << endl;

            cin >> selection;

        }

        if(debugging){
            cerr << " DEBUGGING Bout to call setComp" << endl;
        }

        //Convert to int
        if(selection == "0"){
            //Send choice to setComplication
            setComplication(0);
        }
        else if(selection == "1"){
            //Send choice to setComplication
            setComplication(1);
        }
        else if(selection == "2"){
            //Send choice to setComplication
            setComplication(2);
        }
        else if(selection == "3"){
            //Send choice to setComplication
            setComplication(3);
        }
        else if(selection == "4"){
            //Send choice to setComplication
            setComplication(4);
        }
        else if(selection == "5"){
            //Send choice to setComplication
            setComplication(5);
        }
        else if(selection == "9"){
            //Send choice to setComplication
            setComplication(9);
        }
        else{
            //Tell logger we hit a bad error
            Log_object->errorLog(2, "Error! Bad complication selection [AIRLINE.CPP]");
        }


}


void Airline::findEffectedFlights(){

}

void Airline::rescheduleFlight(){


    //Going to need to call the logger flight update method...

}

void Airline::onTimeUpdate(Clock& new_time) {

    //Start by setting done to false
    TimeObserver::setIsNotDone();

    //Check to see if flights need scheduling
    scheduleFlights();

    //Report time update to cout
    Objects_clock = new_time;
    cout << "Airline " << Airline_name << " updated its time to "
              << new_time.hours << ":" << new_time.minutes << ":" << new_time.seconds << endl;

    //Say that we are do3ne
    TimeObserver::setIsDone();
}

void Airline::printFlightVector(){
    //Mainly for debugging purposes

    for(int i=0; i<All_flights.size(); i++){
        All_flights[i]->printFlightDetails();
    }
}
