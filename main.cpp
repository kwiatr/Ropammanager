#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "config.h"
#include "tcp.h"
#include <string>

//***********
#include "logger.h"
#include <iostream>
#include "ropam.h"
#include "Httpd.h"
#include "Weatherd.h"

INITIALIZE_EASYLOGGINGPP
#undef _DEBUG
#define _MAX_PACKET_IDENTIFIER	65535

Cconfig *cfg_;

int loop(std::string serial, CHttpd *httpd, CWeatherd *weather)
{
	TcpClient c;
	retry:	if (!c.ropamConnect(cfg_->ustawienia["ROPAM"]["host"].c_str(),  atoi(cfg_->ustawienia["ROPAM"]["port"].c_str()) ))
	{
		chrono::system_clock::duration(chrono::milliseconds(30000));
		goto retry;
	}

	LOG(WARNING) << "connected to the operator!";
	ropam r;
	r.setTcpKey((cfg_->ustawienia["ROPAM"]["cypher"]));
	httpd->state = &r.state;
	weather->state = &r.state;

	std::vector<unsigned char> request = r.initialConnect(serial, cfg_->ustawienia["ROPAM"]["cypher"]);
	
	if (c.send_binary(request))
	{	
		LOG(INFO) << "Initial connect sent.";
		std::vector<unsigned char> response = c.receive_binary(request.size());
		r.Log(response, "Initial login response");
		//00 00 07 00 02
		if (response.size() < 5 ||  response[0] != 0x00 || response[1] != 0x00 || response[2] != 0x07 || response[3] != 0x00)
		{
			printf("ERROR - incorrect response to initial connection!\n");
			return 0;
		}
		LOG(WARNING) << "------------------------------ PWD/IP---------------------------\n";
		r.setIp(response);
		request.clear();

		bool frame_ok = false;
		{
			request.clear();
			request = r.pwdIpDatagram(cfg_->ustawienia["ROPAM"]["pwd"].c_str());
			if (c.send_binary(request))
			{			
				std::vector<unsigned char> response; 
				std::vector<unsigned char> ret = c.receive_binary(0x04);  //56
				#ifdef _DEBUG
				r.Log(request, "sent. ", 2);
				#endif
				response.clear();
				response.push_back(ret[0]);
				response.push_back(ret[1]);
				response.push_back(ret[2]);
				response.push_back(ret[3]);
				std::vector<unsigned char> suffix_response = c.receive_binary(response[2]+3);
				#ifdef _DEBUG
				#endif
				for (int i = 0; i < suffix_response.size(); i++)
					response.push_back(suffix_response[i]);

				#ifdef _DEBUG			 
				#endif
				std::vector<unsigned char> subres;
				for (int i = 0; i < 16; i++)
					subres.push_back(response[i+8]);
				#ifdef _DEBUG
				r.Log(subres, "subresponse");
				#endif
				r.setTcpVector(r.seed);

				if (r.parseDatagram(response, false))
				{
									
					while(r.index < _MAX_PACKET_IDENTIFIER)
					{
						ret = c.receive_binary(0x04);
						r.setTcpVector(r.seed);
						response.clear();
						response.push_back(ret[0]);
						response.push_back(ret[1]);
						response.push_back(ret[2]);
						response.push_back(ret[3]);					
						std::vector<unsigned char> suffix_response = c.receive_binary(response[2]+3);
						#ifdef _DEBUG
						r.Log(suffix_response, "RECEIVED BINARY: ", 1);
						#endif
						for (int i = 0; i < suffix_response.size(); i++)
							response.push_back(suffix_response[i]);
						#ifdef _DEBUG
						#endif
						if (! r.parseDatagram(response,false))
						{
							LOG(ERROR) << "UNABLE TO UNDERSTAND in parseDatagram!";
							c.disconnect();
							return 1;
							break;
						}
						chrono::system_clock::duration(chrono::milliseconds(2000));
					}
					LOG(ERROR) << "reconnecting";
				}	
				else
				{
					printf("Frame OK!");
					LOG(WARNING) << "Frame OK??";
					c.disconnect();
					return 1;
//					break;
					
				}
				request.clear();
				chrono::system_clock::duration(chrono::milliseconds(1000));
				LOG(WARNING) << "loop";
			}
			else
			{
				LOG(ERROR) << "Unable to send data!";
				c.disconnect();
				return 1;				
			}
		}

	}
	else
	{
		LOG(ERROR) << "Unable to make Initiall Connection call";
	}
	c.disconnect();
	LOG(WARNING) << "returning";
	return 0;
}


int main(int argc, char* argv[])
{
	el::Configurations conf("logger.ini");
        el::Loggers::reconfigureAllLoggers(conf);
        el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
        LOG(INFO) << "ROPAM Manager v. "<< 1.0 << " Starting";

	cfg_ = new Cconfig("config.ini");
	cfg_->Load();
	CHttpd *httpd = new CHttpd(atoi(cfg_->ustawienia["SERVER"]["port"].c_str()));
	httpd->initHttpd();
	
	CWeatherd *weather = new CWeatherd(atoi(cfg_->ustawienia["SERVER"]["weatherport"].c_str()));
	weather->initWeatherd();

	while(1)
	{
		if (loop(cfg_->ustawienia["ROPAM"]["serial"].c_str(), httpd, weather))
			break;
		LOG(ERROR) << "waiting..";
		chrono::system_clock::duration(chrono::milliseconds(60000));
	}
	delete httpd;
	delete weather;
        
	return 0;
}
