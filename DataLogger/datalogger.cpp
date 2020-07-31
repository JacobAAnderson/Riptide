
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <ctime>
#include <chrono>

#include "MOOS/libMOOS/App/MOOSApp.h"



using namespace std;



//string fileName = "/home/uuuv/Jake/logs/" + ctime(&now) + "_sensorLog.csv";

class ExampleApp : public CMOOSApp {

	bool OnStartUp(){

		time_t t = time(0);   // get time now
		struct tm * now = localtime( & t );

		strftime (fileName,80,"/home/uuuv/Jake/logs/%Y%m%d%H%M%S_sensorLog.csv",now);

//		chrono::duration<double> time_ = chrono::system_clock::now();

		cout << "\n\n\n";
//		cout << time_
		cout << "\tWriting Log File to:  " << fileName << "\n\n\n";
		
		// Write file header
		ofstream myfile;
		myfile.open (fileName,ios_base::app);
//		myfile << "Sys Time";
		for(int i = 0; i<moosMsgs.size(); i++) myfile << ", " << moosMsgs[i];
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
		
		return true;
	}


	bool OnNewMail(MOOSMSG_LIST & Mail){
		
		cout << "\n\n_________________________________________\n";
		cout << " Checking Mail\n";

		MOOSMSG_LIST::iterator q;
		for(q=Mail.begin();q!=Mail.end();q++){ cout << q->GetKey();
			
			for(int i = 0; i < moosMsgs.size(); i++){
			
			// ____ Get GPS ____________________________	
			if(q->GetKey()==moosMsgs[i]) { values[i] = q->GetDouble(); 
				                           cout <<": " << q->GetDouble() <<endl;
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
		
		cout << "\n\n_________________________________________\n";
		cout << "Write to file\n";
		
//		chrono::duration<double> time_ = chrono::system_clock::now();
		
		double time_ = 0.0;
			 
		ofstream myfile;
		myfile.open (fileName, ios_base::app);
		myfile << fixed << setprecision(8);
		myfile << time_;
		for(int i = 0; i< sizeof(values)/sizeof(values[0]); i++) myfile << "," << values[i];
		myfile << endl;
		myfile.close();

//		cout << "Time: " << std::chrono::system_clock::now() << endl;
		cout << "__________________________________________\n\n";
				
			}


	void ResetVars(){ 	
		for(int i = 0; i< sizeof(values)/sizeof(values[0]); i++) values[i] = -999.9999;
		}


	// ___ Variables ________
	char fileName [80];
	double values[24];
	vector<string> moosMsgs = {"GPS_FIX",
					   "GPS_YEAR",
					   "GPS_MONTH",
					   "GPS_DAY",
					   "GPS_HOUR",
					   "GPS_MINUTE",
					   "GPS_SECOND",
					   "GPS_LATITUDE", 
					   "GPS_LONGITUDE",
					   "NAV_HEADING",
					   "NAV_ROLL",
					   "NAV_PITCH",
					   "NAV_YAW",
					   "PWR_NOSE_VOLTAGE",
					   "PWR_TAIL_VOLTAGE",
					   "PWR_PAYLOAD_VOLTAGE",
					   "PWR_NOSE_FAULT",
					   "PWR_TAIL_FAULT",
					   "PWR_PAYLOAD_FAULT",
					   "RT_THRUST_SPEED",
					   "ALT_TRIGGER",
					   "ALT_PING_RATE",
					   "ALT_SOUND_SPEED",
					   "ALT_ALTITUDE"};
};


int main(int argc, char * argv[]){

//	cout << fixed << setprecision(6);
    
	//here we do some command line parsing...
	MOOS::CommandLineParser P(argc,argv);

	//mission file could be first free parameter
	string mission_file = P.GetFreeParameter(0, "Mission.moos");

	//app name can be the  second free parameter
	string app_name = P.GetFreeParameter(1, "datalogger");

	ExampleApp App;

	App.Run(app_name,mission_file,argc,argv);

	return 0;
}
