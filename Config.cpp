#include "Config.hpp"

Config::Config(std::string_view	name)	:	_name(name),  _file(new std::fstream(_name.data())), _content(""){}

Config::~Config()
{
	if(_file)
	{
		if(_file->is_open()){ _file->close(); } 

		delete _file;

		_file = nullptr;
	}
}

bool	Config::create()	const
{
	FILE*	file = nullptr;
	
	file = fopen(_name.data(), "w+");

	if(!file){ return false; }
	else
	{
		fclose(file);

		return true;
	}
}

bool	Config::exist()	const{ return _file->is_open(); }

bool	Config::isOpened()	const
{
	if(!_file){ return false; }

	return _file->is_open();
}

bool	Config::open(std::ios_base::openmode	mode)
{
	if(!_file){ return false; }

	if(_file->is_open()){ return true; }

	_file->open(_name.data(), mode);
	
	return !_file->fail();
}

bool	Config::close()
{
	if(!_file){ return false; }

	if(!_file->is_open()){ return true; }

	_file->close();

	return true;
}

std::string_view	Config::getName()	const{ return _name; }

bool	Config::setName(std::string_view	name)
{
	if(!_file){ return false; }

	if(_file->is_open()){ return false; }

	delete _file;

	_name = name;

	_file = new std::fstream(_name.data());

	return true;
}

bool	Config::addValue(std::string_view	key, std::string_view	value)	const
{
	if(!_file){ return false; }
	if(!_file->is_open()){ return false; }

	_file->write(key.data(), key.size());
	_file->write("=", 1);
	_file->write(value.data(), value.size());
	_file->write("\n", 1);

	return true;
}

bool	Config::addValue(std::string_view	key, int32_t	value)	const
{
	if(!_file){ return false; }
	if(!_file->is_open()){ return false; }

	_file->write(key.data(), key.size());
	_file->write("=", 1);
	_file->write(std::to_string(value).data(), std::to_string(value).size());
	_file->write("\n", 1);

	return true;
}

bool	Config::addValue(std::string_view	key, char	value)	const
{
	if(!_file){ return false; }
	if(!_file->is_open()){ return false; }

	_file->write(key.data(), key.size());
	_file->write("=", 1);
	_file->write(std::string(1, value).data(), 1);
	_file->write("\n", 1);

	return true;
}

bool	Config::getValue(std::string_view	key, std::string&	value)	const
{
	int32_t	pos = _content.find(key.data());
	int32_t	pos2 = 0;
	int32_t	pos3 = 0;

	std::string	str = "";

	if(pos == -1){ return false; }

	pos2 = _content.find("\n", pos);

	str = _content.substr(pos, pos2 - pos);

	pos3 = str.find("=");

	if(pos3 == -1){ return false; }

	value = str.substr(pos3 + 1, pos2 - pos3 - 1);

	return true;
}

bool	Config::getValue(std::string_view	key, char&	value)	const
{
	int32_t	pos = _content.find(key.data());
	int32_t	pos2 = 0;
	int32_t	pos3 = 0;

	std::string	str = "";

	if(pos == -1){ return false; }

	pos2 = _content.find("\n", pos);

	str = _content.substr(pos, pos2 - pos);

	pos3 = str.find("=");

	if(pos3 == -1){ return false; }

	value = *(str.substr(pos3 + 1, pos2 - pos3 - 2).c_str() + 0);

	return true;
}

bool	Config::getValue(std::string_view	key, int&	value)	const
{
	int32_t	pos = _content.find(key.data());
	int32_t	pos2 = 0;
	int32_t	pos3 = 0;

	std::string	str = "";

	if(pos == -1){ return false; }

	pos2 = _content.find("\n", pos);

	str = _content.substr(pos, pos2 - pos);

	pos3 = str.find("=");

	if(pos3 == -1){ return false; }

	if(!isNumber(str.substr(pos3 + 1, pos2 - pos3 - 2))){ return	false; }

	value = std::stoi(str.substr(pos3 + 1, pos2 - pos3 - 2));

	return true;
}

bool	Config::readAll()
{
	char	symbol = 0;

	if(!_file){ return false; }
	if(!_file->is_open()){ return false; }

	_content = "";

	while(_file->get(symbol))
	{
		_content += symbol;

		if(_file->eof()){ break; }
	}

	_file->seekp(std::ios_base::beg);

	return true;
}

std::string	Config::getContent()	const { return _content; }

bool	Config::isNumber(std::string	str)	const
{
	bool	isNumber	=	true;

	for(uint32_t	counter	=	0; counter < str.size(); ++counter)
	{
		if(!isdigit(str.at(counter)))
		{
			isNumber	=	false;

			break;
		}
	}

	return	isNumber;
}
