
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <chrono>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>


#include "MOOS/libMOOS/App/MOOSApp.h"



using namespace std;

// const char fileName[] = "/Users/jake/tmp/Riptide/logs/acomslog.csv"; 	// On Lap top
const char fileName[] = "/home/uuuv/Jake/logs/acomslog.csv"; 				// On Riptide



class AcommsApp : public CMOOSApp {

	bool OnStartUp(){

		// Write file header
		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );

		strftime (fileName,80,"/home/uuuv/Jake/logs/%Y%m%d%H%M%S_acommsLog.csv",now);

		ofstream myfile;
		myfile.open (fileName,ios_base::app);
		myfile << "Sys Time, GPS fix, GPS date-time";
		for(int i = 7; i<moosMsgs.size(); i++) myfile << ", " << moosMsgs[i];
		myfile << endl;
		myfile.close();


		// Make Sure the varialbe are set to void values
		ResetVars();
		

		// Set rate for Iterate()
		double Hz = 1.0;   //[s]
		SetAppFreq(Hz,0.0);
//		SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL);

		return true;
	}


	bool OnConnectToServer(){

		for(int i = 0; i<moosMsgs.size(); i++) Register( moosMsgs[i], 0.0); 

		cout << "\n\nAcoms Registered\n\n";

		return true;
	}


	bool OnNewMail(MOOSMSG_LIST & Mail){
		
		MOOSMSG_LIST::iterator q;
		for(q=Mail.begin();q!=Mail.end();q++){ 
			
			for(int i = 0; i < moosMsgs.size(); i++){
			
				if(q->GetKey()=="ACOMMS_RECV"){
					acoms_ = q -> GetString();
					cout <<"\nACOMMS_RECV: " << q->GetString() <<endl << endl;		
					
					string msg = "Hey Ya";
					SendAcomms(msg);
					
					Write2File();
					break;
					}
							
				else if(q->GetKey()==moosMsgs[i]) {
					values[i] = q->GetDouble(); 
				    break;
				    }
			}
		}
		return true;
	}



	bool Iterate(){

		now = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = now - last_rx;
//		if( elapsed_seconds.count() > 60) SendAcomms("Talk to me"); 
		
		cout << "Elapsed Time: " << elapsed_seconds.count() << "\r";
		
		return true;
	}



protected:

	void SendAcomms( string const &data) {
		
		cout <<"\nSend acomms: ";
					
		int64_t microseconds = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
    
		string trx = to_string(microseconds) + ":" + data;
		
		Notify("ACOMMS_XMIT", trx, MOOSLocalTime());
	
		last_rx = chrono::system_clock::now();
		cout << microseconds << "\t[mico-s]\n";
	    }
				

	void Write2File() { 	// Write data to file
		
		cout << "\nWrite to file -- ";

		// Get current system time in microseconds
		gettimeofday(&tv, NULL);                                // Get current time
        usec = tv.tv_usec;                                      // Get uSeconds
        tm_info = localtime(&tv.tv_sec);                        // Get current time in local format
		strftime(date_buffer, 50, "%F %T", tm_info);           	// Format data and time --> YY/mm/DD HH:MM:SS
		sprintf(usec_buffer, "%s.%06ld", date_buffer, usec);  	// Add useconds to time --> YY/mm/DD HH:MM:SS.zzzzzz
			 
		
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
//		myfile << fixed << setprecision(3);
//		for(int i = 11; i< sizeof(values)/sizeof(values[0]); i++) myfile << "," << values[i];
		
		// End
		myfile << endl;
		myfile.close();

		cout << usec_buffer << endl;
				
		}


	void ResetVars(){ 	// Initialize / Resed data values
		for(int i = 0; i< sizeof(values)/sizeof(values[0]); i++) values[i] = std::nan("1");
		last_rx = std::chrono::system_clock::now();
		acoms_ = "\0";
		}


	// ___ Values _____
	ofstream myfile; 			// File object to read and write from file
	
	char fileName [80]; 		// Path to Data file 
	char date_buffer[50];      	// Buffer to format date and time
	char usec_buffer[50];      	// Buffer to add microseconds to date & time
	
	double values[26]; 			// Data Values

	long usec;                  // variable to hold useconds
	
	struct tm* tm_info;         // Structures for time values
	struct timeval tv;
			
	string acoms_; 				// String for recived acomms message
	
	vector<string> moosMsgs = {"GPS_FIX", 		//  1 		MOOS messages
					   "GPS_YEAR", 				//  2
					   "GPS_MONTH", 			//  3
					   "GPS_DAY", 				//  4
					   "GPS_HOUR", 				//  5
					   "GPS_MINUTE", 			//  6
					   "GPS_SECOND",  //____________7
					   "GPS_LATITUDE",  		//  8
					   "GPS_LONGITUDE",			//  9
					   "NAV_LAT", 				// 10
					   "NAV_LONG",  //_____________11
					   "ACOMMS_RECV" 			// 12
					   };
					  
	std::chrono::time_point<std::chrono::system_clock> last_rx, now; 	// Timepoints

};



int main(int argc, char * argv[]){

	cout << fixed << setprecision(6);

	//here we do some command line parsing...
	MOOS::CommandLineParser P(argc,argv);

	//mission file could be first free parameter
	string mission_file = P.GetFreeParameter(0, "Mission.moos");

	//app name can be the  second free parameter
	string app_name = P.GetFreeParameter(1, "acoms");

	AcommsApp App;

	App.Run(app_name,mission_file,argc,argv);

	return 0;
}
