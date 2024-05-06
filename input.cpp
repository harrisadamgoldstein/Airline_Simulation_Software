/*
* input.cpp
*
* Header file: input.h
*
* Read from input files for airport, plane, and flight objects
*
*/



#include "input.h"
#include <fstream>

std::mutex Input::input_lock;


Input::Input()
{
	//Mutex lock
	//lock_guard<mutex> lock(input_lock);

    read_airports(); // Calls to read in input for airports, planes, and flights
	sleep(1);

	if(All_airports.size() < 2)
	{
		cout << "Not enough valid airports for simulation to run properly. Please restart the simulation." << endl;

		sleep(1);
	}

	

    read_planes();
	sleep(1);

	if(All_planes.size() < 1)
	{
		cout << "No valid planes detected. Please restart the simulation." << endl;

		sleep(1);
	}

	

    read_flights();
	sleep(1);

	if(All_flights.size() < 1)
	{
		cout << "No valid flights detected. Please restart the simulation." << endl;

		sleep(1);
	}
}
void Input::read_airports()
{
    //Mutex lock
	lock_guard<mutex> lock(input_lock);

    string default_file = "airportinput.csv"; //Create a default file name to look for before prompting the user for a file.
    string user_input_file; //Used in case of not finding the default file.
    int line_count;
	string line;

	bool eof_flag = false;


    //Variable being read in
    string airport_name; 
	double longitude, latitude;
	int hub_status;

	//count the number of hubs to make sure it does not exceed maximum # of hubs
	int hub_count = 0;

	//hub_max to make sure the maximum number of hubs allowed is not exceeded
	const int hub_max = 4;
	
    //Attempt to open the default file
    ifstream infile(default_file);

    //If it opens, read in from default.
    if(infile.is_open())
    {
        cout << "Default file '" << default_file << "' found. Processing input." << endl;
	}

    	//If the default file is not found, prompt the user for a file until it is found.
    else
    {
        	cout << "Default file '" << default_file << "' not found. Please insert the input file name for airport info." << endl;
			cin >> user_input_file;

		//Attempt to open file
		infile.open(user_input_file);

		//While file not found, ask until it is found.
		while(!infile.is_open())
		{
	    	cout << "Input file '" << user_input_file << "' not found. Please insert another file name." << endl;
		    cin >> user_input_file;

		    infile.open(user_input_file);
		}
		cout << "Input file '" << user_input_file << "' found. Processing input." << endl;
		}
		//Same read in as before

		infile >> line_count;
		//cout << line_count;
		//confirm at least 2 airports for flights
		if(line_count <= 1)
		{
			cout << "Line count less than or equal to 1. Errors will be found in flight inputs. Please restart the simulation." << endl;
		}

		//getline to skip line_count line
		getline(infile, line);

		for(int i = 0; i < line_count; i++)
		{
			if(debugging){
				cout << "Index: " << i << endl;
			}

			// initialize a bool to check for all valid inputs
			// change to false if any input is invalid
			bool valid_line = true; 

			if(infile.eof())
			{
				valid_line = false;
				eof_flag = true;
			}

			//getline for airport_name since it's the only variable on the line

			getline(infile, line);



			istringstream iss(line);
			char comma;

			getline(iss, airport_name, ',');
			iss >> latitude >> comma >> longitude >> comma >> hub_status;


			/*//Trim the last variable on each line to remove white space
			size_t end = airport_name.length();
			while(end > 0 && isspace(airport_name[end-1]))
			{
				end--;
			}
			//Reassign airport_name once it has been trimmed
			airport_name = airport_name.substr(0, end);*/

			//PROOFING

			//confirm there are no duplicate airport names
			if(All_airports.size() > 0)
			{
				if(debugging)
				{
					cout << "Searching airport_name..." << endl;
				}
				for(int j = 0; j < All_airports.size(); j++)
				{
					if(airport_name == All_airports[j]->getAirportName())
					{
						cout << "Duplicate airport name detected. Invalid input." << endl;
						valid_line = false;
					}
				}
			}

			if(debugging)
			{
				cout << "Checking latitude..." << endl;
			}


			//Confirm latitude is within valid range
			if(latitude < -90 || latitude > 90)
			{
				cout << "Latitude out of range. Invalid input." << endl;
				valid_line = false;
			}

			if(debugging)
			{
				cout << "Checking longitude..." << endl;
			}
			//Confirm longitude is within valid range
			//Longitude can either be from -180 to 180 or from 0 to 360, we are using the former
			//Include this specification in user manual and input file formats
			if(longitude < -180 || longitude > 180)
			{
				cout << "Longitude out of range. Invalid input." << endl;
				valid_line = false;
			}
			
			if(debugging)
			{
				cout << "Validating hub value..." << endl;
			}
			//Confirm hub status is either 1 or 0
			if(hub_status != 1 && hub_status != 0)
			{
				cout << "Hub status out of range. Invalid input." << endl;
				valid_line = false;
			}

			if(debugging)
			{
				cout << "Checking for hub max..." << endl;
			}
			//Confirm maximum hub count is not exceeded
			//Can make hub_max a global const?
			//If hub max is not reached, add to hub_count
			//Else, it's exceeding max
			if(hub_status == 1 && hub_count < hub_max)
			{
				hub_count++;
			}
			else if(hub_status == 1 && hub_count >= hub_max)
			{
				cout << "Maximum number of hubs exceeded. Invalid input." << endl;
				valid_line = false;
			}
			

			if(debugging){
			cout << "Airport name: " << airport_name << ", " << latitude << ", " << longitude << ", " << hub_status << endl;
			}
			//valid_line is initially true and changed to false if any of the above confirmations are not met,
			//which will make valid_line false, which will not create a new airport with invalid info.
			if(valid_line == true)
			{
	    		Airport* airport = new Airport(i, airport_name, longitude, latitude, hub_status);
		   		register_airport(airport);
			}
			else
			{
				cout << "Invalid input discovered. Skipping line number " << i+2 << endl;
			}
		   	//if end of file is reached, end it early
			if(eof_flag)
			{
				cout << "End of file reached. Ending..." << endl;
				break;
			}

		}
		
	cout << "Airport processing complete." << endl;

}
void Input::register_airport(Airport* airport)
{
	if(debugging){
		cout << "Registering Airport"  << endl;
	}
    //Add the parameter airport as an observer then push it onto the vector	 
    All_airports.push_back(airport);
}
void Input::read_planes()
{
	//Mutex lock
	lock_guard<mutex> lock(input_lock);

    //Same process, just with plane variables and input file instead.
    string default_file = "planeinput.csv";
    string user_input_file;
    int line_count;
	string line;
	bool eof_flag = false;

	//plane variables
	string plane_name;
    string plane_model;
    int max_passengers;
    double max_fuel;
    double burn_rate;
    double max_speed;

    ifstream infile(default_file);
	//search if the default file exists
    if(infile.is_open())
    {
        cout << "Default file '" << default_file << "' found. Processing input." << endl;
    }
    else
    {
		cout << "Default file '" << default_file << "' not found. Please insert the input file name for plane info." << endl;
		cin >> user_input_file;

		//Attempt to open file
		infile.open(user_input_file);

		//While file not found, ask until it is found.
		while(!infile.is_open())
		{
	    	cout << "Input file '" << user_input_file << "' not found. Please insert another file name." << endl;
		    cin >> user_input_file;

		    infile.open(user_input_file);
		}
		cout << "Input file '" << user_input_file << "' found. Processing input." << endl;
	}

		infile >> line_count;
		//confirm at least 1 plane for flights
		if(line_count < 1)
		{
			cout << "Line count less than 1. No planes found. Errors will be found in flight inputs. Please restart the simulation." << endl;
		}

		//getline to skip the line_count line
		getline(infile, line);


		for(int i = 0; i < line_count; i++)
		{
			if(debugging){
				cout << "Index: " << i << endl;
			}

			bool valid_line = true;
			if(infile.eof())
			{
				valid_line = false;
				eof_flag = true;
			}

			//get the whole line as a string
			getline(infile, line);

			istringstream iss(line);
			char comma;

			//getline for plane_name, ending at comma
			getline(iss, plane_model, ',');
			//getline for plane_model, ending at comma
			getline(iss, plane_name, ',');
			//read in other variables, skipping commas
			iss >> max_passengers >> comma >> max_fuel >> comma >> burn_rate >> comma >> max_speed;

			//PROOFING

			//confirm there are no duplicate plane_names
			if(All_planes.size() > 0)
			{
				if(debugging) 
				{
					cout << "Searching plane_name..." << endl;
				}
				for(int j = 0; j < All_planes.size(); j++)
				{
					if(plane_name == All_planes[j]->getPlaneName())
					{
						cout << "Duplicate plane name detected. Invalid input." << endl;
						valid_line = false;
					}
				}
			}

			if(debugging)
			{
				cout << "Checking plane_model..." << endl;
			}

			//Only plane models allowed are the ones listed below since loan costs must be found manually
			if(plane_model != "B600" && plane_model != "B800" && plane_model != "A100" && plane_model != "A300")
			{
				cout << "Plane model not found. Invalid input." << endl;
				valid_line = false;
			}

			if(debugging)
			{
				cout << "Checking max_passengers..." << endl;
			}
			//confirm max_passengers is more than 0
			if(max_passengers <= 0)
			{
				cout << "Max passengers cannot be below or equal to 0. Invalid input." << endl;
				valid_line = false;
			}

			if(debugging)
			{
				cout << "Checking max_fuel..." << endl;
			}
			//confirm max_fuel is more than 0
			if(max_fuel <= 0)
			{
				cout << "Max fuel cannot be below or equal to 0. Invalid input. " << endl;
				valid_line = false;
			}

			if(debugging)
			{
				cout << "Checking burn_rate..." << endl;
			}
			//confirm burn_rate is more than 0
			if(burn_rate <= 0)
			{
				cout << "Burn rate cannot be below or equal to 0. Invalid input. " << endl;
				valid_line = false;
			}			

			if(debugging)
			{
				cout << "Checking max_speed..." << endl;
			}
			//confirm max_speed is more than 0
			if(max_speed <= 0)
			{
				cout << "Max fuel cannot be below or equal to 0. Invalid input. " << endl;
				valid_line = false;
			}

			if(debugging)
			{
			cout << "Plane data: " << plane_model << ", " << plane_name << ", " << max_passengers << ", " << max_fuel << ", " << burn_rate << ", " << max_speed << endl;
			}
			//same process as in read_airports()
			//valid_line starts at true and becomes false is any above confirmations are invalid
			//if valid_line stays true, create as a new plane
			//otherwise, skip the line 
			if(valid_line)
			{
	    		Plane* plane = new Plane(i, plane_name, plane_model, max_fuel, burn_rate, max_speed, max_passengers);
	    		register_plane(plane);
			}
			else
			{
				cout << "Invalid input discovered. Skipping line number " << i+2 << endl;
			}
			//end early if end of file
			if(eof_flag)
			{
				cout << "End of file reached. Ending..." << endl;
				break;
			}
		}
	cout << "Plane processing complete." << endl;
}
void Input::register_plane(Plane* plane)
{
	if(debugging){
		cout << "Registering Plane" << endl;
	}
    //Same process but for planes
    All_planes.push_back(plane);
}
void Input::read_flights()
{
	//Mutex lock
	lock_guard<mutex> lock(input_lock);

    //Same process of reading in input except with flights. Not sure if this is the way to go since I was unfamiliar with the scheduleFlights function while writing this
    string default_file = "flightinput.csv";
    string user_input_file;
    int line_count;
	string line;
	bool eof_flag = false;

    ifstream infile(default_file);
	//variables for flight
    string flight_type_name;
	string flight_type_id;
    string origin;
    string dest;
    double distance;
    int takeoff_h, takeoff_m, takeoff_s;
	int takeoff_duration, in_air_duration, deboard_duration;
    int deboard_h, deboard_m, deboard_s;
    double ticket_price;
	string plane_type;

    if(infile.is_open())
    {
		//if default file opens, read that in
		cout << "Default file '" << default_file << "' found. Processing input." << endl;		
	}
    else
    {
		cout << "Default file '" << default_file << "' not found. Please insert the input file name for flight info." << endl;
		cin >> user_input_file;
		cout << user_input_file;

		infile.open(user_input_file);

		while(!infile.is_open())
		{
		    cout << "Input file '" << user_input_file << "' not found. Please insert another file name." << endl;
		    cin >> user_input_file;

		    infile.open(user_input_file);
		}
		cout << "Input file '" << user_input_file << "' found. Processing input." << endl; 
	}
		infile >> line_count;
		if(line_count < 1)
		{
			cout << "Line count less than 1. No flights will be found, and simulation will not produce results. Please restart the simulation." << endl;
		}

		//getline grabs the line_count line
		getline(infile, line);


		for(int i = 0; i < line_count; i++)
		{
			if(debugging){
			cout << "Index: " << i << endl;	
			}

			bool valid_line = true;
			if(infile.eof())
			{
				valid_line = false;
				eof_flag = true;
			}

			getline(infile, line);

			istringstream iss(line);

			char comma;

			iss >> ticket_price >> comma;
			getline(iss, flight_type_name, ',');
			getline(iss, flight_type_id, ',');
			getline(iss, origin, ',');
			getline(iss, dest, ',');
			
			iss >> takeoff_h >> comma >> takeoff_m >> comma >> takeoff_s >> comma >> takeoff_duration >> comma >> in_air_duration >> comma >> deboard_duration >> comma >> distance >> comma;

			getline(iss, plane_type, ',');

			iss >> deboard_h >> comma >> deboard_m >> comma >> deboard_s;

			//Trim the last variable on each line to remove white space
			/*size_t end = plane_type.length();
			while(end > 0 && isspace(plane_type[end-1]))
			{
				end--;
			}
			//Reassign plane_type once it has been trimmed
			plane_type = plane_type.substr(0, end);*/

			//PROOFING

			if(debugging)
			{
				cout << "Checking ticket_price..." << endl;
			}

			if(ticket_price < 0)
			{
				cout << "Ticket price below 0. Invalid input." << endl;
				valid_line = false;
			}

			if(All_flights.size() > 0)
			{
				if(debugging)
				{
					cout << "Searching flight_type_id..." << endl;
				}
				for(int j = 0; j < All_flights.size(); j++)
				{
					if(flight_type_id == All_flights[j]->getFlightTypeID())
					{
						cout << "Duplicate flight type ID. Invalid input." << endl;
						valid_line = false;
					}
				}
			}

			bool matching_origin = false;
			bool matching_dest = false;

			if(debugging)
			{
				cout << "Searching for matching origin and dest..." << endl;
			}

			for(int j = 0; j < All_airports.size(); j++)
			{
				if(origin == All_airports[j]->getAirportName())
				{
					matching_origin = true;
				}
				if(dest == All_airports[j]->getAirportName())
				{
					matching_dest = true;
				}
			}

			if(matching_origin == false)
			{
				cout << "Origin airport not found in airport list. Invalid input." << endl;
				valid_line = false;
			}

			if(matching_dest == false)
			{
				cout << "Destination airport not found in airport list. Invalid input." << endl;
				valid_line = false;
			}

			if(debugging)
			{
				cout << "Checking if origin and dest are the same..." << endl;
			}

			if(origin == dest)
			{
				cout << "Origin and destination airports cannot be the same. Invalid input." << endl;
				valid_line = false;
			}

			if(debugging)
			{
				cout << "Checking times and durations..." << endl;
			}

			if(takeoff_h < 0 || takeoff_h >= 24)
			{
				cout << "Takeoff hour time not between 0 and 23. Invalid input." << endl;
				valid_line = false;
			}

			if(takeoff_m < 0 || takeoff_m >= 60) 
			{
				cout << "Takeoff minute time not between 0 and 59. Invalid input." << endl;
				valid_line = false;
			}

			if(takeoff_s < 0 || takeoff_s >= 60)
			{
				cout << "Takeoff second time not between 0 and 59. Invalid input." << endl;
				valid_line = false;
			}

			if(takeoff_duration < 0)
			{
				cout << "Takeoff duration cannot be less than 0. Invalid input." << endl;
				valid_line = false;
			}

			if(in_air_duration < 0)
			{
				cout << "In air duration cannot be less than 0. Invalid input." << endl;
				valid_line = false;
			}

			if(deboard_duration < 0)
			{
				cout << "Deboard duration cannot be less than 0. Invalid input." << endl;
				valid_line = false;
			}

			if(deboard_h < 0 || deboard_h >= 24)
			{
				cout << "Deboard hour time not between 0 and 23. Invalid input." << endl;
				valid_line = false;
			}

			if(deboard_m < 0 || deboard_m >= 60) 
			{
				cout << "Deboard minute time not between 0 and 59. Invalid input." << endl;
				valid_line = false;
			}

			if(deboard_s < 0 || deboard_s >= 60)
			{
				cout << "Deboard second time not between 0 and 59. Invalid input." << endl;
				valid_line = false;
			}

			if(debugging)
			{
				cout << "Checking for plane model..." << endl;
			}

			bool plane_type_found = false;

			for(int j = 0; j < All_planes.size(); j++)
			{
				if(plane_type == All_planes[j]->getPlaneModel())
				{
					plane_type_found = true;
				}
			}

			if(!plane_type_found)
			{
				cout << "Plane model for flight not found. Invalid input." << endl;
				valid_line = false;
			}

			if(debugging){
				cout << "Flight data: " << ticket_price << ", " << origin << ", " << dest << ", " << flight_type_name << ", " << flight_type_id << ", " << origin << ", " <<  dest << ", " <<  takeoff_h << ", " << takeoff_m << ", " << takeoff_s << ", " << takeoff_duration << ", " << in_air_duration << ", " << deboard_duration << ", " << distance << ", " << plane_type << ", " << deboard_h << ", " << deboard_m << ", " << deboard_s << endl;
			}
			//cerr << "----------" << endl;
			//cerr << ticket_price << ", " << flight_type_name << ", " << plane_name << ", " << dest << ", " <<  origin << ", " <<  departure_h << ", " << departure_m << ", " << departure_s << ", " << arrival_h << ", " << arrival_m << ", " << arrival_s << ", " << distance << endl;
	
			if(valid_line)
			{
		    	Flight* flight = new Flight(i, ticket_price, flight_type_name, flight_type_id, plane_type, dest, origin, takeoff_h, takeoff_m, takeoff_s, deboard_h, deboard_m, deboard_s, distance, takeoff_duration, in_air_duration, deboard_duration);
		    	register_flight(flight);
			}
			else
			{
				cout << "Invalid input detected. Skipping line number " << i+2 << endl;
			}

			if(eof_flag)
			{
				cout << "End of file detected. Ending..." << endl;
				break;
			}
		}
	cout << "Flight processing complete." << endl;
}
void Input::register_flight(Flight* flight)
{
	if(debugging){
		cout << "Registering Flight" << endl;
	}
    //Don't create as an observer since this is something to be scheduled.
    All_flights.push_back(flight);
}
//Return vectors to airline (probably) so they can be registered as time observers
vector<Airport*>& Input::get_airport_vector()
{
	//Mutex lock
	lock_guard<mutex> lock(input_lock);
	return All_airports;
}
vector<Plane*>& Input::get_plane_vector()
{
	//Mutex lock
	lock_guard<mutex> lock(input_lock);
	return All_planes;
}
vector<Flight*>& Input::get_flight_vector()
{
	//Mutex lock
	lock_guard<mutex> lock(input_lock);
	return All_flights;
}
