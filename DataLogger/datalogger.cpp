
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <ctime>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#include "MOOS/libMOOS/App/MOOSApp.h"



using namespace std;



//string fileName = "/home/uuuv/Jake/logs/" + ctime(&now) + "_sensorLog.csv";

class DataloggerApp : public CMOOSApp {

	bool OnStartUp(){

		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );

		strftime (fileName,80,"/home/uuuv/Jake/logs/%Y%m%d%H%M%S_DataLog.csv",now);
		
		cout << "\n\n\nWriting Log File to:  " << fileName << "\n\n\n";
		
		// Write file header
		ofstream myfile;
		myfile.open (fileName,ios_base::app);
		myfile << "Sys Time, GPS fix, GPS date-time";
		for(int i = 7; i<moosMsgs.size(); i++) myfile << ", " << moosMsgs[i];
		myfile << endl;
		myfile.close();

		// Make Sure the varialbe are set to void values
		ResetVars();
		
		return true;
	}


	bool OnConnectToServer(){
		
		for(int i = 0; i<moosMsgs.size(); i++) Register( moosMsgs[i], 0.0); 
				
		cout << "\n\n---- Datalogger Registered ----\n\n";	
		cout << "Start Time: " << MOOSLocalTime() << endl;
		
		Notify("RT_SET_ALT_TRIGGER", "auto", MOOSLocalTime());
		Notify("RT_SET_ALT_PING_RATE", 1, MOOSLocalTime());
		
		return true;
	}


	bool OnNewMail(MOOSMSG_LIST & Mail){

		MOOSMSG_LIST::iterator q;
		for(q=Mail.begin();q!=Mail.end();q++){ 
			
			for(int i = 0; i < moosMsgs.size(); i++){
			
				if(q->GetKey()==moosMsgs[i]) { 
					values[i] = q->GetDouble(); 
					if(q->GetKey()=="ALT_ALTITUDE") Write2File();
					break;
					}
				}
			}
		return true;
	}


	bool Iterate(){return true; }



protected:


	void Write2File() {
		
		cout << "\nWrite to file -- ";

		gettimeofday(&tv, NULL);                                // Get current time
        usec = tv.tv_usec;                                      // Get uSeconds
        tm_info = localtime(&tv.tv_sec);                        // Get current time in local format

        strftime(date_buffer, 50, "%F %T", tm_info);            // Format data and time --> YY/mm/DD HH:MM:SS
        sprintf(usec_buffer, "%s.%06ld", date_buffer, usec);    // Add useconds to time --> YY/mm/DD HH:MM:SS.zzzzzz
	
			 
		// Write data to file
		myfile.open (fileName, ios_base::app);
		myfile << usec_buffer;
		
		// Write GPS meta-data
		myfile << fixed << setprecision(0) 										// No Decimal Point
		       << "," << values[0] 												// GPS fix
		       << "," << values[1] << '/' << values[2] << '/' << values[3]  	// GPS Date
		       << " " << values[4] << ':' << values[5] << ':' << values[6]; 	// GPS Time
		
		// Write lat lon
		myfile << fixed << setprecision(8);
		for(int i = 7; i< 11; i++) myfile << "," << values[i];
		
		// Write Other data
		myfile << fixed << setprecision(3);
		for(int i = 11; i< sizeof(values)/sizeof(values[0]); i++) myfile << "," << values[i];
		
		// End
		myfile << endl;
		myfile.close();

		cout << usec_buffer << endl;
				
			}


	void ResetVars(){ 	
		for(int i = 0; i< sizeof(values)/sizeof(values[0]); i++) values[i] = std::nan("1");
		}


	// ___ Variables ________
	ofstream myfile; 			// File object to read and write from files
	
	char fileName [80]; 		// Path to Data file 
	char date_buffer[50];      	// Buffer to format date and time
	char usec_buffer[50];      	// Buffer to add microseconds to date & time
	
	double values[26]; 			// Data Values

	long usec;                  // variable to hold useconds
	
	struct tm* tm_info;         // Structures for time values
	struct timeval tv;
			
	vector<string> moosMsgs = {"GPS_FIX",		//  1
					   "GPS_YEAR",				//  2
					   "GPS_MONTH", 			//  3
					   "GPS_DAY", 				//  4
					   "GPS_HOUR", 				//  5
					   "GPS_MINUTE", 			//  6
					   "GPS_SECOND", //_____________7
					   "GPS_LATITUDE",  		//  8
					   "GPS_LONGITUDE", 		//  9
					   "NAV_LAT", 				// 10
					   "NAV_LONG",	//_____________11
					   "NAV_HEADING", 			// 12
					   "NAV_ROLL", 				// 13
					   "NAV_PITCH", 			// 14
					   "NAV_YAW", 				// 15
					   "PWR_NOSE_VOLTAGE",		// 16
					   "PWR_TAIL_VOLTAGE",		// 17
					   "PWR_PAYLOAD_VOLTAGE",	// 18
					   "PWR_NOSE_FAULT",		// 19
					   "PWR_TAIL_FAULT",		// 20
					   "PWR_PAYLOAD_FAULT", 	// 21
					   "RT_THRUST_SPEED", 		// 22
					   "ALT_TRIGGER",			// 23
					   "ALT_PING_RATE",			// 24
					   "ALT_SOUND_SPEED", 		// 25
					   "ALT_ALTITUDE" 			// 26
					   };


};


int main(int argc, char * argv[]){

//	cout << fixed << setprecision(6);
    
	//here we do some command line parsing...
	MOOS::CommandLineParser P(argc,argv);

	//mission file could be first free parameter
	string mission_file = P.GetFreeParameter(0, "Mission.moos");

	//app name can be the  second free parameter
	string app_name = P.GetFreeParameter(1, "datalogger");

	DataloggerApp App;

	App.Run(app_name,mission_file,argc,argv);

	return 0;
}
