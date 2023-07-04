#ifndef _H_HTTPD
#define _H_HTTPD
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

class CHttpd
{
	private:
		struct MHD_Daemon *daemon;

	public:
		ropamStatus *state;
		std::string weather;
		CHttpd(int port);		 
		void initHttpd();
		~CHttpd();
		unsigned int port;
		string mime;

		MHD_Response *statusResponse();
};
#endif
