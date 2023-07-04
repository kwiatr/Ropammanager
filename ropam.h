#ifndef _ROPAM_H_
#define _ROPAM_H_
#include <string>
#include <vector>
typedef unsigned char byte;
#include <string>
#include <jansson.h>
#include <functional>
#include <thread>
#include <condition_variable>
#define _DEBUG

namespace Strings
{

        std::string format(const char *format...);
};

//class representing a single zone in alarm system
class ropamZone
{
	public:
		int id;
		int isArmed;
		int isNightArmed;
		int onDeparture;
		int onEntrance;
		int isAlarm;
		int isTamper;
		int isReady;
		
		void Load(unsigned char value, int id);
		void printout();
		json_t *getStatus();
};

class weatherContainer
{
	public:
		int tZewn;
		int tIn;
		int tStrych;
		int tGaraz;
		int tDol;
		int tGBOX;
		
		int wZewn;
		int wIn;
		int wStrych;
		int wGaraz;
		int wDol;
		int wGBOX;
		
		int wspd;
		int wspdhi;
		int wspdavg;
		int rain;
		int bar;

		std::string battery;
		std::string battery2;
		std::string battery5;
		std::string battery6;
		std::string battery8;
		std::string date;
		std::string time;
		
		weatherContainer();
};

//class representing full status of the whole alarm system
class ropamStatus
{
	private:
		int isArmed;
		int mainAlarm;
		std::string firmware;
		std::string language;
		std::string currentTime;
		bool alarmSent;	//flag indicating if alarm was already pushed to mobile device
		std::vector<unsigned char> inputs;
		std::vector<unsigned char> outputs;
		std::string ins;
	        std::string outs;
		
		std::vector<ropamZone> zones;
		std::vector<std::string> _split(const std::string& s, char seperator);
		
		int temp1;
		int temp2;
		int voltage;
		std::mutex mlock;
		
		json_t *shortReport;
		json_t *weatherReport;
	public:
		ropamStatus():shortReport(0x00),weatherReport(0x00),alarmSent(false){};
		~ropamStatus();
		std::string apiKey;
		std::string deviceToken;
		weatherContainer weather;
		
		void Load(std::vector<unsigned char> datagram);
		void printout();
		
		std::string getStatus();
		void prepareStatus();
		void parseWeather();

};


class ropam
{
	private:
		std::vector<unsigned char> tcpVector;

//		std::vector<unsigned char> seed2;
		unsigned int headerSize;
		std::vector<unsigned char> tcpKey;
		std::vector<unsigned char> ip;
		unsigned char getCRC(std::vector<unsigned char> data, int start, int end);
		std::vector<unsigned char> encode(std::vector<unsigned char> bytes, int startIdx, int endIdx, std::vector<unsigned char> key, std::vector<unsigned char>, bool verbose = true);	
		bool recurrent(std::vector<unsigned char> try_datagram, int k, int j);

	public:
		std::vector<unsigned char> seed;
		ropamStatus state;
		int index;
		
		ropam();

		std::vector<unsigned char> connectVector();
		void getTcpVector();
		void setTcpVector(std::vector<unsigned char> vec);
		void setIp(std::vector<unsigned char> response);
		void setTcpKey(std::string key);

		void Log(std::vector<unsigned char>, std::string message, int level=0);
		bool parseDatagram(std::vector<unsigned char> datagram, bool brute_force=false);

		int switchType(std::vector<unsigned char> datagram);


		//************* CONNECTION ************
		std::vector<unsigned char> initialConnect(std::string serial, std::string cypher);
		std::vector<unsigned char> pwdIpDatagram(std::string pwd);
		std::vector<unsigned char> toBytes(std::vector<unsigned char> data, int type=0);
};

#endif