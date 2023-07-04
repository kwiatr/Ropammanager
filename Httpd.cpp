#include "Httpd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <turbojpeg.h>

CHttpd *httpd;

//------------------------------------------------------------------------------------------------
static int answer_to_connection (void *cls, struct MHD_Connection *connection,
const char *url, const char *method,
const char *version, const char *upload_data,
size_t *upload_data_size, void **con_cls)
{
	CHttpd*  srv = httpd;
	struct MHD_Response *response;
	int fd;
	int ret;
	std::string sip = "";

	struct sockaddr *ip;
	ip = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS)->client_addr;

	char str[20];

	if (ip->sa_family == AF_INET)
	{
		struct sockaddr_in *v4 = (struct sockaddr_in *)ip;
		sip = inet_ntop(AF_INET, &(v4->sin_addr), str, 20);
	}

	if (0 != strcmp (method, "GET"))
		return MHD_NO;
	string mimeType  = srv->mime.c_str();

	if(strstr(url,"status"))
	{
		response    = srv->statusResponse();
		mimeType    = "application/json";
	}
	else
		return MHD_NO;

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
CHttpd::CHttpd(int port_):
port(port_),
mime("application/json")
{
	printf("bound to port: %d\n", port_);
}

CHttpd::~CHttpd()
{
	MHD_stop_daemon(this->daemon);
}

//------------------------------------------------------------------------------------------------
void CHttpd::initHttpd()
{
	this->daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, this->port, NULL, NULL,
		&answer_to_connection,(void**)this, MHD_OPTION_END);
	if (NULL == this->daemon)
	{
		fprintf(stderr,"Nie mozna uruchomic serwera HTTPD");
		exit(1);
		return;
	}
	httpd = this;
	printf("daemon started\n");

}


//------------------------------------------------------------------------------------------------
MHD_Response *CHttpd::statusResponse()
{
	//    char statusResponse[255];
	char *statusResponse = 0x00;
	std::string status = "no data";
	if (state)
	{
		std::string status = state->getStatus();
		if (!status.empty())
		{
			printf("STATUS: %s\n", status.c_str());
/*			if (!weather.empty())
			{
				printf("WEATHER : %s\n", weather.c_str());
				statusResponse = (char*)malloc(sizeof(char) * (status.length()+weather.length()+1));
				sprintf(statusResponse, "%s\n%s", status.c_str(), weather.c_str());
			}
			else*/
			{
				statusResponse = (char*)malloc(sizeof(char) * (status.length()+1));
				sprintf(statusResponse, "%s", status.c_str());			
			}
			//       sprintf(statusResponse, status.c_str());

		}
		else
		{
			statusResponse = (char*)malloc(sizeof(char)*6);
			sprintf(statusResponse, "%s", "error");
		}
	}
	//    return MHD_create_response_from_data(strlen(statusResponse),(void*)statusResponse, 0, 1);
	return MHD_create_response_from_data(strlen(statusResponse),(void*)(const char*)statusResponse, 1, 0);
}
