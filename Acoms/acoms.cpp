
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
		myfile << "Sys Time, Mission, GPS fix, GPS date-time";
		for(int i = 7; i< (moosMsgs.size()-1); i++) myfile << ", " << moosMsgs[i];
		myfile << ", ACOMMS_XMIT";
		myfile << ", " << moosMsgs[ moosMsgs.size()-1 ];
		myfile << endl;
		myfile.close();


		// Make Sure the varialbe are set to void values
		ResetVars();
		

		// Set rate for Iterate()
		double Hz = 1.0;   //[s]
		SetAppFreq(Hz,0.0);
//		SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL);

		cout << "\n\n______________________________________\n"
		     << moosMsgs[ moosMsgs.size()-1 ]
		     << "\n_______________________________________\n\n";

		return true;
	}


	bool OnConnectToServer(){

		for(int i = 0; i<moosMsgs.size(); i++) Register( moosMsgs[i], 0.0); 

		Register("PLOGGER_CMD",0.0);
	
		Notify("RT_SET_PAYLOAD_POWER", 1, MOOSLocalTime());

		return true;
	}


	bool OnNewMail(MOOSMSG_LIST & Mail){
		
		MOOSMSG_LIST::iterator q;
		for(q=Mail.begin();q!=Mail.end();q++){ 
			
			// Check for recived Acomms
			if(q->GetKey() == moosMsgs[ moosMsgs.size()-1 ]){	
				acoms_rx = q -> GetString();
				cout <<"\nACOMMS_RECV: " << q->GetString() <<endl << endl;		
					
				SendAcomms("R");
					
				Write2File(acoms_rx, "\0");
				continue;
					}
			
			// Check for PLOGGER_CMD --> what mission is being run
			if (q->GetKey() == "PLOGGER_CMD"){ 
				plog_cmd = q->GetString(); 
				continue;} 
				
			// Keep Track of the rest of the maill
			for(int i = 0; i < moosMsgs.size(); i++){
						
				if(q->GetKey()==moosMsgs[i]) {
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

		if(values[15] ==1. && elapsed_seconds.count() > 60) SendAcomms("I");
		
		cout << "Modem ID: " << values[15] << "\tElapsed Time: " << elapsed_seconds.count() << "\r";
		
		return true;
	}



protected:

	void SendAcomms( string const &data) {
	
		int64_t microseconds = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count();
		string trx = to_string(microseconds) + ";" + data;		
		Notify("ACOMMS_XMIT", trx,  MOOSLocalTime());
		Write2File("\0", trx);
		cout <<"\nSend acomms: " << trx << endl;

//		Notify("ACOMMS_XMIT", data, MOOSLocalTime());
//		cout <<"\nSend acomms: " << data << endl;
//		Write2File("\0", data);
		
		last_rx = chrono::system_clock::now();
		
	    }
	
	
	string AcomsMsg(){
		char buffer[100];
		
		// GPS Latitude, GPS Longitude
		sprintf(buffer, "%2.8f,%3.7f", values[7], values[8]);
		
		// GPS Latitude, GPS Longitude, NAV Heading, NAV Roll, NAV Pitch, NAV Yaw
//		sprintf(buffer, "%2.8f,%3.7f,%3.1f,%2.1f,%2.1f,%3.1f", values[7], values[8], values[11], values[12], values[13], values[12]);
		
		string msg = buffer;
		
		return msg;
	}
						   			

	void Write2File(string const & AcommsRecive, string const &AcommsTransmit) { 	// Write data to file
		
		cout << "\nWrite to file -- ";

		// Get current system time in microseconds
		gettimeofday(&tv, NULL);                                // Get current time
        usec = tv.tv_usec;                                      // Get uSeconds
        tm_info = localtime(&tv.tv_sec);                        // Get current time in local format

        strftime(date_buffer, 50, "%F %T", tm_info);            // Format data and time --> YY/mm/DD HH:MM:SS
        sprintf(usec_buffer, "%s.%06ld", date_buffer, usec);    // Add useconds to time --> YY/mm/DD HH:MM:SS.zzzzzz
			 
		
		// Write data to file
		myfile.open (fileName, ios_base::app);
		myfile << usec_buffer
			   << "," << plog_cmd;
		
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
		
		myfile << "," << AcommsTransmit 
		       << "," << AcommsRecive;
		
		// End
		myfile << endl;
		myfile.close();

		cout << usec_buffer << endl;
				
		}


	void ResetVars(){ 	// Initialize / Resed data values
		for(int i = 0; i< sizeof(values)/sizeof(values[0]); i++) values[i] = std::nan("1");
		last_rx = std::chrono::system_clock::now();
		acoms_rx = "\0";
		plog_cmd = "\0";
		}


	// ___ Values _____
	ofstream myfile; 			// File object to read and write from file
	
	char fileName [80]; 		// Path to Data file 
	char date_buffer[50];      	// Buffer to format date and time
	char usec_buffer[50];      	// Buffer to add microseconds to date & time
	
	double values[16]; 			// Data Values

	long usec;                  // variable to hold useconds
	
	struct tm* tm_info;         // Structures for time values
	struct timeval tv;
			
	string acoms_rx, plog_cmd; 	// String for recived acomms message and plogger cmd
	
	vector<string> moosMsgs = {	// MOOS messages
		"GPS_FIX", 			//  1
		"GPS_YEAR", 		//  2
		"GPS_MONTH", 		//  3
		"GPS_DAY", 			//  4
		"GPS_HOUR", 		//  5
		"GPS_MINUTE", 		//  6
		"GPS_SECOND",  //_______7
		"GPS_LATITUDE",  	//  8
		"GPS_LONGITUDE",	//  9
		"NAV_LAT", 			// 10
		"NAV_LONG",  //________11
		"NAV_HEADING", 		// 12
		"NAV_ROLL", 		// 13
		"NAV_PITCH",		// 14
		"NAV_YAW",			// 15
		"MODEM_ID", 		// 16
		"ACOMMS_RECV_CSV",	// 17

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
