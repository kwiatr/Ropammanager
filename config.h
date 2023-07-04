
#ifndef _C_CONFIG_H_
#define _C_CONFIG_H_

#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>

typedef std::map<std::string, std::string> in_level;
typedef std::map<std::string, in_level> mapa_;

class Cconfig
{
private:
	int loaded;
	std::string path;
public:
	mapa_ ustawienia;
	
	Cconfig(std::string path_ = "");
	~Cconfig(void);

	int Load();
	int Refresh();
};


#endif
