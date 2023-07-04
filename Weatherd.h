#ifndef _H_WEATHERD
#define _H_WEATHERD
#include <string>
#include <map>
#include <vector>
#include <microhttpd.h>
#include <ptypes/pasync.h>
#include <curl/curl.h>
#include <ptypes.h>

#include <ptime.h>
#include <ptypes/pasync.h>
#include "ropam.h"
using namespace std;

class CWeatherd
{
	private:
		struct MHD_Daemon *daemon;

	public:
		ropamStatus *state;
		CWeatherd(int port);		 
		void initWeatherd();
		~CWeatherd();
		unsigned int port;
		string mime;

		MHD_Response *statusResponse();
};
#endif
