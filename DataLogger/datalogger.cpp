
#include <iostream>
#include <iomanip>
#include <fstream>

#include "MOOS/libMOOS/App/MOOSApp.h"



using namespace std;

const char fileName[] = "/home/uuuv/Jake/logs/sensorlog.csv";

class ExampleApp : public CMOOSApp {

	bool OnStartUp(){

		std::ofstream myfile;
		myfile.open (fileName,std::ios_base::app);
		myfile << "Sys Time, GPS Fix, GPS Date-Time [yy/MM/dd hh/mm/ss], GPS_LATITUDE [deg], GPS_LONGITUDE [deg], Roll [rad], Pitch [rad], Yaw [rad], Alt Speed of sound [m/s], Altitude [m], Alt-Triger, Atl-Rate, Battery [V] \n";
		myfile.close();

		ResetVars();
		
		return true;
	}


	bool OnConnectToServer(){
		
		Register("GPS_FIX",0.0);
		Register("GPS_LATITUDE",0.0);
		Register("GPS_LONGITUDE",0.0);
		Register("GPS_YEAR",0.0);
		Register("GPS_MONTH",0.0);
		Register("GPS_DAY",0.0);
		Register("GPS_HOUR",0.0);
		Register("GPS_MINUTE",0.0);
		Register("GPS_SECOND",0.0);
		
		Register("NAV_HEADING",0.0);
		Register("NAV_ROLL",0.0);
		Register("NAV_PITCH",0.0);
		Register("NAV_YAW",0.0);
		
		Register("PWR_NOSE_VOLTAGE",0.0);
		Register("PWR_TAIL_VOLTAGE",0.0);
		Register("PWR_NOSE_FAULT",0.0);
		Register("PWR_TAIL_FAULT",0.0);
		
		Register("RT_THRUST_SPEED", 0.0);
		
		Register("ALT_TRIGGER", 0.0);
		Register("ALT_PING_RATE", 0.0);
		Register("ALT_SOUND_SPEED",0.0);
		Register("ALT_ALTITUDE",0.0);
		
		cout << "\n\n---- Datalogger Registered ----\n\n";

		time_ = MOOSLocalTime();
	
		cout << "Start Time: " << MOOSLocalTime() << endl;
		
		return true;
	}


	bool OnNewMail(MOOSMSG_LIST & Mail){
		
		cout << "\n\n_________________________________________\n";
		cout << " Checking Mail\n";

		time_ = MOOSLocalTime();
		
		MOOSMSG_LIST::iterator q;
		for(q=Mail.begin();q!=Mail.end();q++){ cout << q->GetKey();
			
			// ____ Get GPS ____________________________	
			if(q->GetKey()=="GPS_FIX")      { gpsFix_       = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			if(q->GetKey()=="GPS_YEAR")     { gpsYear_      = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			if(q->GetKey()=="GPS_MONTH")    { gpsMonth_     = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			if(q->GetKey()=="GPS_DAY")      { gpsDay_       = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			if(q->GetKey()=="GPS_HOUR")     { gpsHour_      = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			if(q->GetKey()=="GPS_MINUTE")   { gpsMinute_    = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			if(q->GetKey()=="GPS_SECOND")   { gpsSecond_    = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			
			if(q->GetKey()=="GPS_LATITUDE") { gpsLatitude_  = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			if(q->GetKey()=="GPS_LONGITUDE"){ gpsLongitude_ = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
						
			
			// ____ Record Atitude of the Vehicle _____
			if(q->GetKey()=="NAV_HEADING"){ heading_  = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			if(q->GetKey()=="NAV_ROLL")   { role_     = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			if(q->GetKey()=="NAV_PITCH")  { pitch_    = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			if(q->GetKey()=="NAV_YAW")    { yaw_      = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			
			
			// ____ Record Battery Voltage ____________
			if(q->GetKey()=="BATTERY") { battery_ = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}


/*			// ____ Check if Altimeter is running _____
			if(q->GetKey()=="ALT_TRIGGER")    { Alt_triger_ = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;
												if(q->GetDouble() != 2.){ Notify("ALT_TRIGGER", 2., MOOSLocalTime());
																		   cout << "--> Resetting Alt-Trigger \n\n";
																	     }
												}
												
			if(q->GetKey()=="ALT_PING_RATE")  { Alt_rate_ = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;
												if(q->GetDouble() != 1.){ Notify("ALT_PING_RATE", 1., MOOSLocalTime());
																		  cout << "--> Resetting Atl-Ping Rate \n\n";
																		 }
												}
*/		

			// ____ Record Altimeter _______________________
			if(q->GetKey()=="ALT_SOUND_SPEED"){ Alt_soundSpeed_ = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;}
			if(q->GetKey()=="ALT_ALTITUDE")   { Altitude_       = q->GetDouble(); cout <<": " << q->GetDouble() <<endl;
												Write2File();
												Altitude_ = -1.;	
												}
			}

		return true;
	}


	bool Iterate(){return true; }



protected:

	void ResetVars(){
		
		time_ = 0.0;
		
		gpsLongitude_ = 0.;
		gpsLatitude_  = 0.;
		gpsYear_      = 0;
		gpsMonth_     = 0;
		gpsDay_       = 0;
		gpsHour_      = -1;
		gpsMinute_ 	  = -1;
		gpsSecond_	  = -1;
		gpsFix_       = -1;
	
		role_  = 999.;
		pitch_ = 999.;
		yaw_   = 999.;
		
		Altitude_       = -1.;
		Alt_soundSpeed_ =  0.;
		Alt_triger_     = -1;
	    Alt_rate_       = -1;
	    
	    battery_ = 0.;
	    
		}


	void Write2File() {
		
		cout << "\n\n_________________________________________\n";
		cout << "Write to file\n";
			 
		ofstream myfile;
		myfile.open (fileName, ios_base::app);
		
		myfile << time_ <<","<< gpsFix_ << "," << gpsYear_ << "/" << gpsMonth_ << "/" << gpsDay_ << " " << gpsHour_<< ":" << gpsMinute_ << ":" << gpsSecond_;
		myfile << fixed << setprecision(7) << "," << gpsLatitude_ << "," << gpsLongitude_;
		myfile << fixed << setprecision(3) << "," << role_ << "," << pitch_ << "," << yaw_;
		myfile << fixed << setprecision(2) << "," << Alt_soundSpeed_ << "," << Altitude_ << "," << Alt_triger_ << "," << Alt_rate_;
		myfile << fixed << setprecision(1) << "," << battery_ << endl;
		
		myfile.close();

		cout << "Time: " << time_ << endl;
		cout << "__________________________________________\n\n";
				
			}


	// ___ Variables ________
	double time_;
	
	double gpsLongitude_;
	double gpsLatitude_;
	int gpsYear_;
	int gpsMonth_;
	int gpsDay_;
	int gpsHour_;
	int gpsMinute_;
	int gpsSecond_;
	int gpsFix_;
	
	double role_;
	double pitch_;
	double yaw_;
	double heading_;
	
	double Altitude_;
	double Alt_soundSpeed_;
	int Alt_triger_;
	int Alt_rate_;
	
	double battery_;
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
