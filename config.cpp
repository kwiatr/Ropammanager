#include "config.h"

//--------------------------------------------------------------------------------------------
Cconfig::Cconfig(std::string path_):path(path_)
{
	if (path.empty())
		path = "config.ini";
	loaded = 0;
}

//--------------------------------------------------------------------------------------------
Cconfig::~Cconfig(void)
{}

//--------------------------------------------------------------------------------------------
int Cconfig::Refresh()
{
	loaded = 0;
	return Load();
}

//--------------------------------------------------------------------------------------------
int Cconfig::Load()
{
	if (loaded)
		return 1;
	
	if (path.empty())
		return -1;

	FILE * f = fopen(path.c_str(), "r");
	if (!f)
		return -1;

	char character_;
	std::string ch_buf;
	std::string v2;
	std::string v1;
	std::string wpis = "global";
	bool polecsv = false;
	while(!feof(f)) 
	{
		character_ = fgetc(f);
		if ((polecsv && character_ != '\"') || 
			(character_ != '[' && character_ != ']' && character_ != ' ' && character_ != '\n' && character_ != '\r' && character_ != '=' && character_ != ';' && character_ != '\"'))
			ch_buf += character_;
		if (character_ == ';' && !polecsv)
		{
			while (!feof(f) && fgetc(f) != '\n'){};
			continue;
		}
		if (character_ == '\"' && polecsv)
			polecsv = false;
		else
			if (character_ == '\"')
				polecsv = true;			
		if (character_ == ']' && !polecsv)
		{
			wpis = ch_buf;
			ch_buf = "";
			continue;
		}
		if (character_ == '=' && !polecsv)
		{
			v2 = ch_buf;
			ch_buf = "";
		}
		if (character_ == '\n' && !polecsv)
		{
			v1 = ch_buf;
			ch_buf = "";
			if (!v2.empty())
			{
				ustawienia[wpis.c_str()][v2.c_str()] = v1.c_str();
				v2 = "";
				v1 = "";
			}
		}
		if (character_ == EOF) 
			break;
    }
	fclose(f);
	loaded = true;
	return 0;
}

//--------------------------------------------------------------------------------------------
