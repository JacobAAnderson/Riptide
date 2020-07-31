
#include <iostream>
#include <fstream>

#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;

class ExampleApp : public CMOOSApp{
	
	bool OnStartUp(){ iter_ =0; return true;}

	bool OnConnectToServer(){
//		Register("GPS_X",0.0);
//		Register("GPS_Y",0.0);
//		Register("Alt",0.0);
//		cout << "\n\nGPS Registered\n\n";

		return true;
	}

	bool OnNewMail(MOOSMSG_LIST & Mail){
		cout << " Checking Mail\n";

		MOOSMSG_LIST::iterator q;
		for(q=Mail.begin();q!=Mail.end();q++){ cout << q->GetKey() << endl; }

		return true;
	}

	bool Iterate(){
		cout <<"Send GPS\n";

		Notify("GPS_X", rand() % 30 + 1985.0, MOOSLocalTime());
		Notify("GPS_Y", rand() % 40 + 2000.0, MOOSLocalTime());

		iter_ += 1;
		if (iter_ == 3){ iter_ = 0;
										 cout << "Send Altimeter\n";
										 Notify("Alt", rand() % 100 + 1, MOOSLocalTime());
									 }

		return true;
	}

protected:
	int iter_;

};

int main(int argc, char * argv[])
{
	//here we do some command line parsing...
	MOOS::CommandLineParser P(argc,argv);

	//mission file could be first free parameter
	string mission_file = P.GetFreeParameter(0, "Mission.moos");

	//app name can be the  second free parameter
	string app_name = P.GetFreeParameter(1, "sensors");

	ExampleApp App;

	App.Run(app_name,mission_file,argc,argv);

	return 0;
}
