#include "Weatherd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <turbojpeg.h>

CWeatherd *weatherd;


//------------------------------------------------------------------------------------------------
static int answer_to_connection (void *cls, struct MHD_Connection *connection,
const char *url, const char *method,
const char *version, const char *upload_data,
size_t *upload_data_size, void **con_cls)
{
	CWeatherd*  srv = weatherd;
	struct MHD_Response *response;
	int fd;
	int ret;
	std::string sip = "";

	
        const char *val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "temp");
        if (val && atoi(val) > -500 && atoi(val) < 500) {
        	srv->state->weather.tZewn = atoi(val);
        }
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "tempin");
        if (val && atoi(val) > -500 && atoi(val) < 500) {
        	srv->state->weather.tIn = atoi(val);
        }
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "temp2");
        if (val && atoi(val) > -50 && atoi(val) < 500) {
        	srv->state->weather.tStrych = atoi(val);
        }
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "temp6");
        if (val && atoi(val) > -50 && atoi(val) < 500) {
        	srv->state->weather.tGBOX = atoi(val);
        }        
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "temp5");
        if (val && atoi(val) > -50 && atoi(val) < 500) {
        	srv->state->weather.tGaraz = atoi(val);
        }
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "temp8");
        if (val && atoi(val) > -50 && atoi(val) < 500) {
        	srv->state->weather.tDol = atoi(val);
        }        
        
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "hum");
        if (val) {
        	srv->state->weather.wZewn = atoi(val);
        }
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "humin");
        if (val) {
        	srv->state->weather.wIn = atoi(val);
	}
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "hum2");
        if (val) {
        	srv->state->weather.wStrych = atoi(val);
	}
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "hum6");
        if (val && atoi(val) > 0) {
        	srv->state->weather.wGBOX = atoi(val);
	}	
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "hum5");
        if (val && atoi(val) > 0) {
        	srv->state->weather.wGaraz = atoi(val);
	}
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "hum8");
        if (val && atoi(val) > 0) {
        	srv->state->weather.wDol = atoi(val);
	}	
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "battery");
        if (val) {
        	srv->state->weather.battery = val;
	}
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "battery2");
        if (val) {
        	srv->state->weather.battery2 = val;
	}
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "battery5");
        if (val) {
        	srv->state->weather.battery5 = val;
	}
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "battery6");
        if (val) {
        	srv->state->weather.battery6 = val;
	}	
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "time");
        if (val) {
        	srv->state->weather.time = val;
	}
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "date");
        if (val) {
        	srv->state->weather.date = val;
	}
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "wspd");
        if (val) {
        	srv->state->weather.wspd = atoi(val);
	}
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "wspdhi");
        if (val) {
        	srv->state->weather.wspdhi = atoi(val);
	}
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "wspdavg");
        if (val) {
        	srv->state->weather.wspdavg = atoi(val);
	}
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "rainrate");
        if (val) {
        	srv->state->weather.rain = atoi(val);
	}	
        val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "bar");
        if (val) {
        	srv->state->weather.bar = atoi(val);
	}
	string mimeType  = srv->mime.c_str();

//	if(strstr(url,"status"))
	{
		response    = srv->statusResponse();
		mimeType    = "application/json";
	}
//	else
//		return MHD_NO;

	if(!response)
	{
		printf("invalid response\n");
		return MHD_NO;
	}
	MHD_add_response_header (response, "Content-Type",mimeType.c_str());
	ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
	MHD_destroy_response (response);

	return ret;
}


//------------------------------------------------------------------------------------------------
CWeatherd::CWeatherd(int port_):
port(port_),
mime("application/json")
{
}

CWeatherd::~CWeatherd()
{
	MHD_stop_daemon(this->daemon);
}

//------------------------------------------------------------------------------------------------
void CWeatherd::initWeatherd()
{
	this->daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, this->port, NULL, NULL,
		&answer_to_connection,(void**)this, MHD_OPTION_END);
	if (NULL == this->daemon)
	{
		fprintf(stderr,"Nie mozna uruchomic serwera HTTPD");
		exit(1);
		return;
	}
	weatherd = this;

}


//------------------------------------------------------------------------------------------------
MHD_Response *CWeatherd::statusResponse()
{
	//    char statusResponse[255];
	char *statusResponse = 0x00;
	std::string status = "no data";
	statusResponse = (char*)malloc(sizeof(char)*6);
	sprintf(statusResponse, "%s", "OK");
	//    return MHD_create_response_from_data(strlen(statusResponse),(void*)statusResponse, 0, 1);
	return MHD_create_response_from_data(strlen(statusResponse),(void*)(const char*)statusResponse, 1, 0);
}
