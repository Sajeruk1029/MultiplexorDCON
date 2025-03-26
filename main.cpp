#include <cstring>
#include <string>
#include <iostream>
#include <ios>

#include <cstdint>

#include "Server.hpp"
#include "Config.hpp"

//вывод справки
void	help(const char*	prog)
{
	std::cout << "Usage: " << prog << " <keys> [args]" << std::endl;
	std::cout << "-f -- path to configuration file." << std::endl;
	std::cout << "-h -- show help." << std::endl;
}

//точка входа
int	main(int	argc, char*	argv[])
{
	char*	config = 0;

	std::string	ip = "";
	std::string	device = "";

	int32_t	port =	0;
	int32_t	stopBit	=	0;
	int32_t	dataBit	=	0;
	int32_t	baud =	0;
	int32_t	maxUsers =	0;
	int32_t	debug =	0;
	int32_t	timeMsWaitResponse	=	0;
	int32_t	maxSizeResponse	=	0;
	int32_t	maxSizeRequest	=	0;

	char	parity = 0;

	std::string	params[12] = { "Ip", "Device", "Port", "StopBit", "DataBit", "Baud", "MaxUsers", "Debug", "Parity", "TimeMsWaitResponse", "MaxSizeResponse", "MaxSizeRequest" };

	if((argc >= 3) && (strcmp(*(argv + 1), "-f") == 0)){ config = *(argv + 2); }
	else
	{
		help(*(argv + 0));
		return 0;
	}

	Config	conf(config);

	if(!conf.exist())
	{
		if(!conf.create())
		{
			std::cerr << "Create configuration file failed!" << std::endl;

			return -1;
		}

		if(!conf.open(std::ios_base::in | std::ios_base::out))
		{
			std::cerr << "Open configuration file failed!" << std::endl;

			return -2;
		}

		for(int counter = 0; counter < 12; ++counter){ conf.addValue(*(params + counter), ""); }

		conf.close();

		std::cerr << "Create configuration file success!" << std::endl;
		return 0;
	}
	else
	{
		if(!conf.readAll())
		{
			std::cerr << "Read configuration failed!" << std::endl;

			return -3;
		}

		if(!conf.getValue("Ip", ip))
		{
			std::cerr << "Not found parametr Ip!" << std::endl;

			return -4;
		}

		if(!conf.getValue("Device", device))
		{
			std::cerr << "Not found parametr Device!" << std::endl;

			return -4;
		}

		if(!conf.getValue("Port", port))
		{
			std::cerr << "Not found parametr Port!" << std::endl;

			return -5;
		}

		if(!conf.getValue("StopBit", stopBit))
		{
			std::cerr << "Not found parametr StopBit!" << std::endl;

			return -5;
		}

		if(!conf.getValue("DataBit", dataBit))
		{
			std::cerr << "Not found parametr DataBit!" << std::endl;

			return -5;
		}

		if(!conf.getValue("Baud", baud))
		{
			std::cerr << "Not found parametr baud!" << std::endl;

			return -5;
		}

		if(!conf.getValue("MaxUsers", maxUsers))
		{
			std::cerr << "Not found parametr MaxUsers!" << std::endl;

			return -5;
		}

		if(!conf.getValue("Debug", debug))
		{
			std::cerr << "Not found parametr Debug!" << std::endl;

			return -5;
		}

		if(!conf.getValue("Parity", parity))
		{
			std::cerr << "Not found parametr Parity!" << std::endl;

			return -6;
		}

		if(!conf.getValue("TimeMsWaitResponse", timeMsWaitResponse))
		{
			std::cerr << "Not found parametr TimeMsWaitResponse!" << std::endl;

			return -7;
		}

		if(!conf.getValue("MaxSizeResponse", maxSizeResponse))
		{
			std::cerr << "Not found parametr  MaxSizeResponse!" << std::endl;

			return -8;
		}

		if(!conf.getValue("MaxSizeRequest", maxSizeRequest))
		{
			std::cerr << "Not found parametr MaxSizeRequest!" << std::endl;

			return -9;
		}
	}

	Server	server(ip, device, port, stopBit, dataBit, baud, maxUsers, parity, timeMsWaitResponse, maxSizeResponse, maxSizeRequest, debug);

	if(server.settings()){ server.start(); }

	return 0;
}
