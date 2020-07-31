
#include <iostream>
#include <iomanip>
#include <fstream>

#include "MOOS/libMOOS/App/MOOSApp.h"



using namespace std;

// const char fileName[] = "/Users/jake/tmp/Riptide/logs/acomslog.csv"; 	// On Lap top
const char fileName[] = "/home/uuuv/Jake/logs/acomslog.csv"; 				// On Riptide

class ExampleApp : public CMOOSApp {

	bool OnStartUp(){

		std::ofstream myfile;
		myfile.open (fileName,std::ios_base::app);
		myfile << "Sys Time, GPS Date-Time, GPS_LATITUDE, GPS_LONGITUDE, Acoms \n";
		myfile.close();

		ResetVars();

		// Set rate for Iterate()
		double Hz = 1.0;   //[s]
		SetAppFreq(Hz,0.0);
//		SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL);

		return true;
	}


	bool OnConnectToServer(){

		Register("GPS_LATITUDE", 0.0);
		Register("GPS_LONGITUDE",0.0);
		Register("GPS_YEAR",  0.0);
		Register("GPS_MONTH", 0.0);
		Register("GPS_DAY",   0.0);
		Register("GPS_HOUR",  0.0);
		Register("GPS_MINUTE",0.0);
		Register("GPS_SECOND",0.0);

		Register("ACOMMS_RECV",0.0);

		cout << "\n\nAcoms Registered\n\n";

		time_ = MOOSLocalTime();

		cout << "T start: " << MOOSLocalTime() << endl;
		return true;
	}


	bool OnNewMail(MOOSMSG_LIST & Mail){

		cout << "\n\n_________________________________________\n";
		cout << " Checking Mail\n";

		time_ = MOOSLocalTime();

		MOOSMSG_LIST::iterator q;
		for(q=Mail.begin();q!=Mail.end();q++){

			// ____ Get GPS ____________________________
			if(q->GetKey()=="GPS_YEAR")      gpsYear_      = q->GetDouble();
			if(q->GetKey()=="GPS_MONTH")     gpsMonth_     = q->GetDouble();
			if(q->GetKey()=="GPS_DAY")       gpsDay_       = q->GetDouble();
			if(q->GetKey()=="GPS_HOUR")      gpsHour_      = q->GetDouble();
			if(q->GetKey()=="GPS_MINUTE")    gpsMinute_    = q->GetDouble();
			if(q->GetKey()=="GPS_SECOND")    gpsSecond_    = q->GetDouble();
			if(q->GetKey()=="GPS_LATITUDE")  gpsLatitude_  = q->GetDouble();
			if(q->GetKey()=="GPS_LONGITUDE"){gpsLongitude_ = q->GetDouble();
				cout << endl << gpsYear_ << "/" << gpsMonth_ << "/" << gpsDay_;
				cout << " " << gpsHour_ <<":" << gpsMinute_ <<":" << gpsSecond_;
				cout << ", " << gpsLatitude_ << ", " << gpsLongitude_ << endl;
				}

			// ____ Acoustic Communications ____________
			if(q->GetKey()=="ACOMMS_RECV")   { acoms_ = q->GetString();
												cout << q->GetKey() <<": " << q->GetString() <<endl;
											  Write2File();
											  acoms_ = "";
											  
											  cout <<"Send acoms\n";

												Notify("ACOMMS_XMIT", "Come on sucker, lick my battery", MOOSLocalTime());}
			}

		return true;
	}


	bool Iterate(){

/*	  cout <<"Send acoms\n";

	  Notify("ACOMMS_XMIT", "Talk to me baby", MOOSLocalTime());
*/
		return true;
	}



protected:

	void ResetVars(){

		time_ = 0.;

		gpsLongitude_ = 0.;
		gpsLatitude_  = 0.;
		gpsYear_      = 0;
		gpsMonth_     = 0;
		gpsDay_       = 0;
		gpsHour_      = -1;
		gpsMinute_ 	  = -1;
		gpsSecond_	  = -1;

		acoms_ = "";
		}


	void Write2File() {

		cout << "\n\n_________________________________________\n";
		cout << "Write to file:" << acoms_ << endl;

		ofstream myfile;
		myfile.open (fileName, ios_base::app);

		myfile << time_ <<","<< gpsYear_ << "/" << gpsMonth_ << "/" << gpsDay_ << " " << gpsHour_<< ":" << gpsMinute_ << ":" << gpsSecond_;
		myfile << fixed << setprecision(7) << "," << gpsLatitude_ << "," << gpsLongitude_;
		myfile << "," << acoms_ << endl;

		myfile.close();

		cout << "Time: " << time_ << endl;
		cout << "__________________________________________\n\n";

		}


	double time_;

	double gpsLongitude_;
	double gpsLatitude_;
	int gpsYear_;
	int gpsMonth_;
	int gpsDay_;
	int gpsHour_;
	int gpsMinute_;
	int gpsSecond_;

	string acoms_;

};



int main(int argc, char * argv[]){

	cout << fixed << setprecision(6);

	//here we do some command line parsing...
	MOOS::CommandLineParser P(argc,argv);

	//mission file could be first free parameter
	string mission_file = P.GetFreeParameter(0, "Mission.moos");

	//app name can be the  second free parameter
	string app_name = P.GetFreeParameter(1, "acoms");

	ExampleApp App;

	App.Run(app_name,mission_file,argc,argv);

	return 0;
}
