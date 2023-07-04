#include "ropam.h"
#include "logger.h"
#include <cstdarg>
#include <stdexcept>
#include <string>
#include <chrono>
#include <iostream>
#undef _DEBUG

std::string Strings::format(const char *format...)
{
        va_list args1;
        va_start(args1, format);
        int size = vsnprintf(nullptr, 0, format, args1);
        va_end(args1);
        if (size < 0)
                throw std::runtime_error("string encoding error");
        char *buffer = new char[size + 1];
        va_list args2;
        va_start(args2, format);
        vsnprintf(buffer, size + 1, format, args2);
        va_end(args2);
        std::string result(buffer);
        delete[] buffer;
        return result;
}

ropam::ropam():headerSize(0x07),index(0x01)
{
	 getTcpVector();
}

ropamStatus::~ropamStatus()
{
	if (shortReport)
		json_decref(shortReport);
}

std::vector<unsigned char> ropam::connectVector()
{
	std::vector<unsigned char> v;
	v.push_back(0x22);
	v.push_back(0x34);
	v.push_back(0x44);
	v.push_back(0xa2);
	v.push_back(0xf4);
	v.push_back(0xcd);
	v.push_back(0x5b);
	v.push_back(0x55);
	v.push_back(0x21);
	v.push_back(0x34);
	v.push_back(0x43);
	v.push_back(0x12);
	v.push_back(0x98);
	v.push_back(0xad);
	v.push_back(0x24);
	v.push_back(0xec);
	return v;
}

void ropam::getTcpVector()
{
	if (tcpVector.empty())
	{
		tcpVector.push_back(0x04);
		tcpVector.push_back(0x5a);
		tcpVector.push_back(0x76);
		tcpVector.push_back(0x43);
		tcpVector.push_back(0x5a);
		tcpVector.push_back(0xea);
		tcpVector.push_back(0x2d);
		tcpVector.push_back(0x0b);
		tcpVector.push_back(0x7b);
		tcpVector.push_back(0x5a);
		tcpVector.push_back(0x57);
		tcpVector.push_back(0x2b);
		tcpVector.push_back(0x62);
		tcpVector.push_back(0x17);
		tcpVector.push_back(0x7b);
		tcpVector.push_back(0x59);
	}
};

void ropam::setTcpVector(std::vector<unsigned char> vec)
{
	tcpVector.clear();
	std::copy(&vec[0], &vec[vec.size()], std::back_inserter(tcpVector));
};

void ropam::setTcpKey(std::string key)
{
	tcpKey.clear();
	std::copy(&key[0], &key[key.length()], std::back_inserter(tcpKey));	
	Log(tcpKey, "tcpkey");
}

void ropam::Log(std::vector<unsigned char> response, std::string message, int level)
{

        std::string logstr = "";
        std::string ss;
	for (int i = 0; i < response.size(); i++)
	{
		logstr += Strings::format("%02x ", response[i]);
		ss += Strings::format("%c ", response[i]);		
	}
	switch (level)
	{
	case 0:
		LOG(INFO) <<  message.c_str() << " size: " <<  response.size() << "\t\t" << logstr;
		break;
	case 1:
		break;
	case 2:
		break;	
	}
}

unsigned char ropam::getCRC(std::vector<unsigned char> data, int start, int end)
{
	unsigned char b = 0;
            for (int i = start; i < end; i++)
            {
                unsigned char b2 = data[i];
                for (int j = 0; j < 8; j++)
                {
                    if (((b2 ^ b) & 1) == 1)
                    {
                        b = (unsigned char)((unsigned char)((b ^ 24) >> 1) | 128);
                    }
                    else
                    {
                        b = (unsigned char)(b >> 1);
                    }
                    b2 = (unsigned char)(b2 >> 1);
                }
            }
            return b;
}

std::vector<unsigned char> ropam::initialConnect(std::string serial, std::string cypher)
{
	std::vector<unsigned char> resp;
	std::vector<unsigned char> v;
	std::copy(&serial[0], &serial[serial.length()], std::back_inserter(v));
	v.push_back(0xcb);
	v.push_back(0x72);
	v.push_back(0x99);
	v.push_back(0xa7);
	v.push_back(0x25);
	v.push_back(0xfe);
	v.push_back(0xd2);
	v.push_back(0xd8);
	v.push_back(0x43);
	v.push_back(0xd1);
	v.push_back(0x4d);
	v.push_back(0xf7);
	v.push_back(0x65);
	v.push_back(0xb5);
	v.push_back(0xec);
	v.push_back(0x89);
	v.push_back(0x5b);
	v.push_back(0x38);
	v.push_back(0xe3);
	v.push_back(0xb0);
	v.push_back(0xa9);
	v.push_back(0x81);
	v.push_back(0xd0);
	v.push_back(0xd3);
	v.push_back(0x71);
	v.push_back(0xf1);
	v.push_back(0xbd);
	v.push_back(0xcf);
	v.push_back(0x46);
	v.push_back(0xc5);
	v.push_back(0x23);	
	v.push_back(0x89);
	seed = v;

	std::vector<unsigned char> mem = v;
	std::vector<unsigned char> vserial;
	std::copy(&serial[0], &serial[serial.length()], std::back_inserter(vserial));

	Log(v, "Initial request before encode");
	std::vector<unsigned char> request = encode(v, 0, 48, vserial, connectVector(), false);

	Log(request, "Initial request", 2);
	return request;
}


void ropam::setIp(std::vector<unsigned char> response)
{
	ip.clear();
	ip.push_back(response[9]);
	ip.push_back(response[8]);
	ip.push_back(response[7]);
	ip.push_back(response[6]);
}

std::vector<unsigned char> ropam::pwdIpDatagram(std::string pwd)
{
#ifdef _DEBUG
	LOG(INFO) << "******************  ropam::pwdIpDatagram ***********************";
#endif
	index +=2;
	tcpVector.clear();
	getTcpVector();
	std::vector<unsigned char> request;
	tcpVector[0] = index;
	tcpVector[1] = 0x00;
	
	request.push_back(0x31);
	request.push_back(0x31);
	request.push_back(0x31);
	request.push_back(0x31);
	for (int i = 0; i < ip.size(); i++)
		request.push_back(ip[3-i]);
	request.push_back(0x8a);	
	request.push_back(0xf5);
	request.push_back(0x47);
	request.push_back(0xf7);
	request.push_back(0x8e);
	request.push_back(0x94);
	request.push_back(0xe0);
	request.push_back(0xb3);
	request.push_back(0x2c);	
	request.push_back(0xc2);
	request.push_back(0x37);
	request.push_back(0xe0);
	request.push_back(0x4a);
	request.push_back(0x5b);
	request.push_back(0x19);
	request.push_back(0xfc);
	seed.clear();
	std::copy(&request[request.size()-16], &request[request.size()], std::back_inserter(seed));

#ifdef _DEBUG
	Log(seed, "tcpVector (seed)after modification");
	LOG(INFO) << "request filled";
	Log(request, "pwd/ip request before encryption");
#endif
	return toBytes(request);
}

bool finds(std::string ss, std::string substr)
{
	if (ss.find(substr) > 0 && ss.find(substr) < ss.length())
	{
		LOG(WARNING) << "FOUND SUBSTR: " << substr;
		return true;
	}
	return false;
}

void ropamZone::printout()
{
	std::string output = Strings::format("Zone %d: \n\t armed: %d\n\t nightarmed: %d\n\t ondeparture: %d\n\t onentrance: %d\n\t isalarm: %d\n\tistamper: %d\n\tisready: %d\n", id, isArmed, isNightArmed, onDeparture, onEntrance, isAlarm, isTamper, isReady);
	LOG(DEBUG)  << output;
}

json_t *ropamZone::getStatus()
{
	json_t *report = json_object();
        json_object_set_new(report, "Zone", json_integer(id));
	json_object_set_new(report, "Armed", json_integer(isArmed));
        json_object_set_new(report, "NightArmed", json_integer(isNightArmed));
        json_object_set_new(report, "OnDeparture", json_integer(onDeparture));
        json_object_set_new(report, "OnEntrance", json_integer(onEntrance));
        json_object_set_new(report, "IsALarm", json_integer(isAlarm));
        json_object_set_new(report, "IsTamper", json_integer(isTamper));
        json_object_set_new(report, "IsReady", json_integer(isReady));
        

        return report;
}

void ropamStatus::printout()
{
	return;
	LOG(DEBUG) << "Firmware: " << firmware;
	LOG(DEBUG) << "Language: " << language;
	LOG(DEBUG) << "TIme: " << currentTime;
	LOG(DEBUG) << "Inputs: " << ins;
	LOG(DEBUG) << "Outputs: " << outs;
	for (int i =0; i < zones.size(); i++)
	{
		zones[i].printout();
	}
	LOG(DEBUG) << "Temp: " << (float)(temp1/10.0);
	LOG(DEBUG) << "Temp: " << (float)(temp2/10.0);
	LOG(DEBUG) << "Voltage: " << (float)(voltage/10.0);

}

std::vector<std::string> ropamStatus::_split(const std::string& s, char seperator)
{
   std::vector<std::string> output;
   std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        output.push_back(substring);
        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

weatherContainer::weatherContainer():tZewn(0), tIn(0), tStrych(0), tGaraz(0), tDol(0), wZewn(0), wIn(0), wStrych(0),wGaraz(0),wDol(0),
wspd(0), wspdhi(0), wspdavg(0), rain(0), bar(0), tGBOX(0), wGBOX(0)
{

}


void ropamStatus::parseWeather()
{
	if(weather.bar < 1.0)
		return;
	if (weatherReport)	
		json_decref(weatherReport);
	weatherReport = json_object ();
	json_object_set_new(weatherReport, "tZewn", json_real(weather.tZewn/10.0));
	json_object_set_new(weatherReport, "tIn", json_real(weather.tIn/10.0));
	json_object_set_new(weatherReport, "tStrych", json_real(weather.tStrych/10.0));
	json_object_set_new(weatherReport, "tGaraz", json_real(weather.tGaraz/10.0));
	json_object_set_new(weatherReport, "tDol", json_real(weather.tDol/10.0));
	json_object_set_new(weatherReport, "tGBOX", json_real(weather.tGBOX/10.0));
	
	
	json_object_set_new(weatherReport, "wZewn", json_integer(weather.wZewn/1));
	json_object_set_new(weatherReport, "wIn", json_integer(weather.wIn/1));
	json_object_set_new(weatherReport, "wStrych", json_integer(weather.wStrych/1));
	json_object_set_new(weatherReport, "wGaraz", json_integer(weather.wGaraz/1));
	json_object_set_new(weatherReport, "wDol", json_integer(weather.wDol/1));
	json_object_set_new(weatherReport, "wGBOX", json_integer(weather.wGBOX/1));
	
	json_object_set_new(weatherReport, "wspd", json_real(weather.wspd/10.0));
	json_object_set_new(weatherReport, "wspdhi", json_real(weather.wspdhi/10.0));
	json_object_set_new(weatherReport, "wspdavg", json_real(weather.wspdavg/10.0));
	json_object_set_new(weatherReport, "rain", json_integer(weather.rain/1));
	json_object_set_new(weatherReport, "bar", json_real(weather.bar/10.0));

	if (!weather.battery.empty())
		json_object_set_new(weatherReport, "battery", json_string(weather.battery.c_str()));
	if (!weather.battery2.empty())
		json_object_set_new(weatherReport, "battery2", json_string(weather.battery2.c_str()));
	if (!weather.battery5.empty())
		json_object_set_new(weatherReport, "battery5", json_string(weather.battery5.c_str()));
	if (!weather.battery6.empty())
		json_object_set_new(weatherReport, "battery6", json_string(weather.battery6.c_str()));
	if (!weather.battery8.empty())
		json_object_set_new(weatherReport, "battery8", json_string(weather.battery8.c_str()));
	if (!weather.date.empty())
		json_object_set_new(weatherReport, "date", json_string(weather.date.c_str()));
	if (!weather.time.empty())
		json_object_set_new(weatherReport, "time", json_string(weather.time.c_str()));
}

std::string ropamStatus::getStatus()
{
	std::unique_lock<std::mutex> hrLock(mlock);
	char *src = 0x00;
	json_t *Report = json_object();
	json_object_set(Report, "ALARM", shortReport);
	json_object_set(Report, "WEATHER", weatherReport);
	src = json_dumps(Report, JSON_INDENT(4)| JSON_ENCODE_ANY);

	std::string ret = "{\"ERROR\":\"NO DATA\"}";
	if (src)
	{
		ret = src;
	}
	free(src);
	
/*	if (weatherReport)
	{
		src = json_dumps(weatherReport, JSON_INDENT(4)| JSON_ENCODE_ANY);

		if (src)
		{
			ret = ret + src;
		}
		free(src);
	}
	else printf("weather is NULL\n");*/
	json_decref(Report);
	return ret;	
}

void ropamStatus::prepareStatus()
{
	std::unique_lock<std::mutex> hrLock(mlock);
	if (shortReport)
		json_decref(shortReport);
	shortReport = json_object ();
	json_object_set_new(shortReport, "Time", json_string(currentTime.c_str()));
	json_object_set_new(shortReport, "Firmware", json_string(firmware.c_str()));	
	json_object_set_new(shortReport, "Language", json_string(language.c_str()));
	json_object_set_new(shortReport, "Inputs", json_string(ins.c_str()));
	json_object_set_new(shortReport, "Outputs", json_string(outs.c_str()));
	json_object_set_new(shortReport, "Armed", json_integer(zones[0].isArmed));
	if (zones[0].isAlarm > 0 || zones[1].isAlarm > 0)
		json_object_set_new(shortReport, "Siren", json_integer(1));	
	else
		json_object_set_new(shortReport, "Siren", json_integer(0));
	json_t *jzones = json_array();
	for (int i = 0; i<zones.size(); i++)
	{
		json_array_append_new(jzones, zones[i].getStatus());
	}
	json_object_set_new(shortReport, "Zones", json_deep_copy(jzones));
	json_object_set_new(shortReport, "Temperature_in", json_real((float)(temp1/10.0)));
	json_object_set_new(shortReport, "Temperature_out", json_real((float)(temp2/10.0)));
	json_object_set_new(shortReport, "Voltage", json_real((float)(voltage/10.0)));	
	json_decref(jzones);
	parseWeather();
}

void ropamZone::Load(unsigned char b4, int i)
{
	id = i;
	isArmed = ((b4 & 1) > 0);
	isNightArmed = ((b4 >> 1 & 1) != 0);
        onDeparture = ((b4 >> 2 & 1) != 0);
        onEntrance = ((b4 >> 3 & 1) != 0);
        isAlarm = ((b4 >> 4 & 1) != 0);
        isTamper = ((b4 >> 5 & 1) != 0);
        isReady = ((b4 >> 6 & 1) != 0);
        
}


void ropamStatus::Load(std::vector<unsigned char> datagram)
{
#ifdef _DEBUG
	LOG(INFO) << "SW Version is correct";
#endif
	short pos = 7;
	firmware = Strings::format("%c%c", datagram[pos], datagram[pos+1]);
	pos +=2;
	language = Strings::format("%c", datagram[pos++]);
	currentTime = Strings::format("%c%c-%c%c-%c%c %c%c:%c%c", datagram[pos], 
	datagram[pos+1], 
	datagram[pos+2],
	datagram[pos+3],	
	datagram[pos+4],	
	datagram[pos+5],
	datagram[pos+6],
	datagram[pos+7],
	datagram[pos+8],
	datagram[pos+9]);
	pos += 10;

	ins.clear();
	outs.clear();
	for (int i = 0; i < 32; i++)
	{
		ins += Strings::format("%c", datagram[pos]);
		inputs.push_back(datagram[pos++]);
	}

	for (int i = 0; i < 32; i++)
	{
		outs += Strings::format("%c", datagram[pos]);
		outputs.push_back(datagram[pos++]);
	}

	zones.clear();
	for (int i = 0; i < 2; i++)
	{
		ropamZone z;
		std::string hexts = Strings::format("%c%c", datagram[pos], datagram[pos+1]);
                std::stringstream ss;
                ss << std::hex << hexts;
                int value1 = 0;
                ss >> value1;
		z.Load(value1, i+1);
		zones.push_back(z);
		pos += 2;
	}

	bool isAlarm = false;
	for (int i = 0; i < zones.size(); i++)
		if (zones[i].isAlarm == 1)
			isAlarm = true;
	
	
	if (!isAlarm && alarmSent)
		alarmSent = false;
	
	pos+=8;
	

	for (int i = 0; i < 2; i++)
	{
		std::string hexts = Strings::format("%c%c%c%c", datagram[pos], datagram[pos+1], datagram[pos+2], datagram[pos+3]);
		std::stringstream ss;
		ss << std::hex << hexts;
		i < 1 ? ss >> temp1 : ss >> temp2;
		pos += 4;
	}
	pos++;
	std::string hexts = Strings::format("%c%c", datagram[pos], datagram[pos+1]);
#ifdef _DEBUG
	LOG(INFO) << hexts;
#endif
	std::stringstream ss;
	ss << std::hex << hexts;
	ss >> voltage;
	pos += 2;	
	
	prepareStatus();
}


int ropam::switchType(std::vector<unsigned char> datagram)
{
#ifdef _DEBUG
	LOG(INFO) << "switchType";
#endif
	if (datagram.size() < 5)
	{
		LOG(ERROR) << "Invalid frame size!";
		return -1;
	}

#ifdef _DEBUG
	Log(datagram, "datagram in swtichType");
	LOG(INFO) << "datagram[4] " << (unsigned int)datagram[4];
#endif
	switch(datagram[4])
	{
	case 0:
		if (datagram[5] == 0x00)
		{
			LOG(DEBUG) << "User: " << (unsigned int)datagram[6];
			unsigned char b = datagram[7];
			LOG(DEBUG) << "Zone1: " << (unsigned int)((b & 1) > 0);
			LOG(DEBUG) << "Zone2: " << (unsigned int)((b >> 1 & 1) != 0);
			LOG(DEBUG) << "Remote: " << (unsigned int)((b >> 4 & 1) != 0); 
			LOG(DEBUG) << "CanLockInputs: " << (unsigned int)((b >> 5 & 1) != 0);
		
			tcpVector.clear();
			seed.clear();
			for (int i = 0; i < 16; i++)
				seed.push_back(datagram[8+i]);
#ifdef _DEBUG
			Log(seed, "seed");
#endif			
			std::string s1, s2;
			for (int i = 24; i < 24+16; i++)
				s1 += Strings::format("%c", datagram[i]);

			for (int i = 40; i < 40+16; i++)
				s2 += Strings::format("%c", datagram[i]);
#ifdef _DEBUG
			LOG(WARNING) << "S1: " << s1.c_str();
			LOG(WARNING) << "S2: " << s2.c_str();
#endif
		}
		else
			LOG(ERROR) << "Incorrect value at pos 5 in type 0" << datagram[5];
		break;
	case 0x10:
#ifdef _DEBUG
		LOG(DEBUG) << "STATUS MESSAGE!";
#endif
		if (datagram[5] == 0x23 && datagram[6] == 0x37)
		{
			state.Load(datagram);
			state.printout();			
		}		
		else
			LOG(ERROR) << "INCORRECT SW VERSION!!";
		
		break;	
	
	};
	return 1;
}

bool ropam::parseDatagram(std::vector<unsigned char> source, bool brute_force)
{
#ifdef _DEBUG
	LOG(INFO) << "*************************************************************** parseDatagram ******************************************************************";
#endif
	std::vector<unsigned char> datagram;
	std::string crcdump;
	index +=2;

	std::copy(&source[0], &source[source.size()], std::back_inserter(datagram));
	unsigned char crc_from_datagram = datagram[datagram.size() - 1];
	unsigned char crc_calculated = getCRC(datagram, 0, datagram.size()-1);
	
#ifdef _DEBUG
	crcdump = Strings::format("-----------> CRC: oryginal crc: %02x, calculated crc: %02x\n", crc_from_datagram, crc_calculated);
	LOG(DEBUG) << crcdump;
#endif
	if (crc_from_datagram != crc_calculated)
	{
		LOG(ERROR) << " response crc does not match!\n";
//		std::chrono::system_clock::duration(std::chrono::milliseconds(30000));
//		exit(0);
		return false;
	}
	tcpVector[0] = datagram[0];
	tcpVector[1] = datagram[1];

#ifdef _DEBUG
	Log(tcpKey, "decoding datagram with tcpkey:");
	Log(tcpVector, "decoding datagram with vector: ");  
	LOG(WARNING) << "index: " << index;
	
	std::string crcs = Strings::format( "CRC at: %02x, i: %d\n", datagram[headerSize+source.size()-1], headerSize+source.size()-1);
	LOG(WARNING) << crcs << " length to decode: " << datagram.size()-1;
	Log(datagram, "bytes to decode");
	LOG(INFO) << "datagram.size(): "<< datagram.size();

#endif	
	std::vector<unsigned char> request = encode(datagram, 4, datagram.size()-1, tcpKey, tcpVector);
#ifdef _DEBUG
        Log(request, "Datagram decoded:");
#endif
        crc_from_datagram = request[request.size()-2];
        std::vector<unsigned char> results;
        crc_calculated = getCRC(request, 4, request.size()-2);
        unsigned char crc_calculated2 = getCRC(request, 4, request.size()-2); 

#ifdef _DEBUG
        crcdump = Strings::format("oryginal crc: %02x, calculated crc: %02x, %02x  \n", crc_from_datagram, crc_calculated, crc_calculated2);
        LOG(DEBUG) << crcdump;
        std::string ss;
        for (int i = 0; i < request.size(); i++)
        	ss += Strings::format("%c", request[i]);
//	LOG(INFO)<<ss;
#endif	
	if (crc_from_datagram != crc_calculated)
	{
		LOG(ERROR) << "response crc does not match!\n";
		return false;
	}
	else
		switchType(request);
	
	return true;
}

//VMPC.Encode(array, 4, (int)((ushort)Frame.HeaderSize + this.length - 1), this.tcpKey, this.tcpVector, 16);
std::vector<unsigned char> ropam::encode(std::vector<unsigned char> bytes, int startIdx, int endIdx, std::vector<unsigned char> key, std::vector<unsigned char> _tcpVector, bool verbose)
{
#ifdef _DEBUG
	if (verbose)
	{
		std::string encodeinfo = Strings::format("encode:start: %d, end:%d, len: %d\n", startIdx, endIdx, bytes.size());
		LOG(INFO) << "*******> ENCODE HEADER:START";
		LOG(INFO) << "\t" << encodeinfo;
	}
#endif

	if (endIdx > bytes.size())
	{
		LOG(ERROR) << "Incorrect endIdx in encode - fixing!!!";
		endIdx = bytes.size()-1;
	}
#ifdef _DEBUG
	if (verbose)
	{
		Log(key, "\tkey:");
		Log(_tcpVector, "\ttcpVector:");
		LOG(INFO) << "*******> ENCODE HEADER:END";	
	}
#endif
	std::vector<unsigned char> array;
	
	int _vectorLen = _tcpVector.size() > 16 ? 16 : _tcpVector.size();
	array.resize(256);
	if (endIdx - startIdx > 0)
	{
		byte b = 0;
		for (unsigned int i = 0; i < 256; i++)
		{
			array[i] = (unsigned short)i;
		}
		unsigned short b2;

		for (unsigned int i = 0; i <= 767; i++)
		{
			b2 = (unsigned short)(i % 256);
			b = array[(unsigned short)(b + array[(unsigned short)b2] + key[i % key.size()]) % 256];
			byte b3 = array[(unsigned short)b2];
			array[(unsigned short)b2] = array[(unsigned short)b];
			array[(unsigned short)b] = b3;
		}

		for (unsigned int i = 0; i <= 767; i++)
		{
			b2 = (unsigned char)(i % 256);
			b = array[(unsigned int)(b + array[(unsigned int)b2] + _tcpVector[i % _vectorLen]) % 256];
			byte b3 = array[(unsigned int)b2];
			array[(unsigned int)b2] = array[(unsigned int)b];
			array[(unsigned int)b] = b3;
		}
		b2 = 0;
		for (unsigned int i = startIdx; i < endIdx; i++)
		{
			b = array[(unsigned int)(b + array[(unsigned int)b2]) % 256];
			bytes[i] ^= array[(unsigned int)(array[(int)array[(unsigned int)b]] + 1) % 256];
			byte b3 = array[(unsigned int)b2];
			array[(unsigned int)b2] = array[(unsigned int)b];
			array[(unsigned int)b] = b3;
			b2 += 1;
		}
	}
	return bytes;
}


std::vector<unsigned char> ropam::toBytes(std::vector<unsigned char> data, int type)
{
#ifdef _DEBUG
	LOG(WARNING) << "*************************************************************** toBytes ******************************************************************";
#endif
	int length = data.size();
	std::vector<unsigned char> datagram;
	tcpVector.clear();
	getTcpVector();
	datagram.push_back(index);
	datagram.push_back(0x00);	//TODO: shifts
	datagram.push_back(length);
	datagram.push_back(0x00);	//TODO: shifts
	datagram.push_back(0x00);
	std::copy(&data[0], &data[data.size()], std::back_inserter(datagram));
	datagram.push_back(0x00);
#ifdef _DEBUG
	Log(datagram, "datagram before crc");	
#endif
	datagram[datagram.size()-1] = getCRC(datagram, 4, datagram.size()-1);
#ifdef _DEGUG	
	Log(datagram, "datagram with crc");
	LOG(INFO) << "dlen: " << datagram.size() << ", headersize: " << headerSize << ", lenght: " << length;
#endif
	tcpVector[0] = datagram[0];
	tcpVector[1] = datagram[1];
	std::vector<unsigned char> request = encode(datagram, 4, headerSize + length - 1, tcpKey, tcpVector/*tvec*/);
	request.push_back(0x00);
#ifdef _DEBUG
	Log(request, "toBytes returns:");
#endif
	request[request.size()-1] = getCRC(request, 0, request.size()-1);
#ifdef _DEBUG
	Log(request, "toBytes with CRC: ");
#endif
	return request;
}
