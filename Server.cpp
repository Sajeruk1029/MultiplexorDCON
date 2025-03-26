#include "Server.hpp"

Server::Server(std::string_view	ip, std::string_view	device, int32_t	port, int32_t	stopBit, int32_t	dataBit, int32_t	baud, int32_t	maxUsers, char	parity, int32_t	timeMsWaitResponse, int32_t	maxSizeResponse, int32_t	maxSizeRequest, int32_t	debug)	:	_ip(ip), _device(device), _port(port), _stopBit(stopBit), _dataBit(dataBit), _baud(baud), _maxUsers(maxUsers), _debug(debug), _timeMsWaitResponse(timeMsWaitResponse), _maxSizeResponse(maxSizeResponse), _maxSizeRequest(maxSizeRequest), _sockServer(-1), _hardwarePort(-1), _parity(parity)
{
	memset(&_tty, 0, sizeof(struct termios));
	memset(&_ttyRes, 0, sizeof(struct termios));

	memset(&_addr, 0, sizeof(struct sockaddr_in));

	memset(&_act, 0, sizeof(struct sigaction));

	memset(&_eventer, 0, sizeof(struct pollfd));
}

Server::Server(void)	:	_sockServer(-1), _hardwarePort(-1)
{
	memset(&_tty, 0, sizeof(struct termios));
	memset(&_ttyRes, 0, sizeof(struct termios));

	memset(&_addr, 0, sizeof(struct sockaddr_in));

	memset(&_act, 0, sizeof(struct sigaction));

	memset(&_eventer, 0, sizeof(struct pollfd));
}

Server::~Server()
{
	if(ioctl(_hardwarePort, TCSETS, &_ttyRes) == -1){ if(_debug){ std::cerr << strerror(errno) << std::endl; } }

	if(_hardwarePort != -1){ close(_hardwarePort); }
	if(_sockServer != -1){ close(_sockServer); }
}


std::string_view	Server::getIp()	const{ return _ip; }
std::string_view	Server::getDevice()	const{ return _device; }

int32_t	Server::getPort()	const{ return _port; }
int32_t	Server::getStopBit()	const{ return _stopBit; }
int32_t	Server::getDataBit()	const{ return _dataBit; }
int32_t	Server::getBaud()	const{ return _baud; }
int32_t	Server::getMaxUsers()	const{ return _maxUsers; }
int32_t	Server::getTimeMsWaitResponse()	const{ return _timeMsWaitResponse; }
int32_t	Server::getMaxSizeResponse()	const{ return _maxSizeResponse; }
int32_t	Server::getMaxSizeRequest()	const{ return _maxSizeRequest; }
int32_t	Server::getDebug()	const{ return _debug; }


void	Server::setIp(std::string_view	ip){ _ip = ip; }
void	Server::setDevice(std::string_view	device){ _device = device; }

void	Server::setPort(int32_t	port){ _port = port; }
void	Server::setStopBit(int32_t	stopBit){ _stopBit = stopBit; }
void	Server::setDataBit(int32_t	dataBit){ _dataBit = dataBit; }
void	Server::setBaud(int32_t	baud){ _baud = baud; }
void	Server::setMaxUsers(int32_t	maxUsers){ _maxUsers = maxUsers; }
void	Server::setTimeMsWaitResponse(int32_t	timeMsWaitResponse){ _timeMsWaitResponse = timeMsWaitResponse; }
void	Server::setMaxSizeResponse(int32_t	maxSizeResponse){ _maxSizeResponse = maxSizeResponse; }
void	Server::setMaxSizeRequest(int32_t	maxSizeRequest){ _maxSizeRequest = maxSizeRequest; }
void	Server::setDebug(int8_t	debug){ _debug = debug; }

void	Server::setParity(char	parity){ _parity = parity; }

void	Server::start()
{
	struct sockaddr_in	caddr;

	int32_t	len = sizeof(struct sockaddr_in);
	int32_t	sock = 0;

	pid_t	child = 0;

	if(_debug){ std::cout << "Server has been started!" << std::endl; }

	while(true)
	{
		if((sock = accept(_sockServer, (struct sockaddr*)&caddr, (socklen_t*)&len)) != -1)
		{
			if(_maxUsers == g_connections){ continue; }

			++Server::g_connections;

			child = fork();

			if(child == -1){ if(_debug){ std::cerr << strerror(errno) << std::endl; } }
			else if(child == 0)
			{
				char*	request	=	0x00;
				char*	response	=	0x00;

				int32_t	resultR = 0;
				int32_t	resultR2 = 0;
				int32_t	resultW = 0;
				int32_t	resultW2 = 0;

				request	=	(char*)calloc(_maxSizeRequest, sizeof(char));

				if(!request)
				{
					if(_debug){ std::cerr << strerror(errno) << std::endl; }

					close(sock);

					exit(-1);
				}

				response	=	(char*)calloc(_maxSizeResponse, sizeof(char));

				if(!response)
				{
					if(_debug){ std::cerr << strerror(errno) << std::endl; }

					free(request);

					close(sock);

					exit(-2);
				}

				memset(request, 0, _maxSizeRequest);
				memset(response, 0, _maxSizeResponse);

				if(!settingsPort())
				{
					close(sock);

					free(request);
					free(response);

					request	=	0x00;
					response	=	0x00;

					exit(-3);
				}

				close(_sockServer);

				while(1)
				{
					resultR = recv(sock, request, _maxSizeRequest, 0);

					if(resultR <= 0){ break; }

					if(flock(_hardwarePort, LOCK_EX) == -1)
					{
						if(_debug){ std::cerr << strerror(errno) << std::endl; }

						close(sock);

						free(request);
						free(response);

						request	=	0x00;
						response	=	0x00;

						exit(-4);
					}

					if(_debug){ std::cout << "Receive from socket: " << resultR << " " << request << std::endl; }

					resultW = write(_hardwarePort, request, resultR);

					if(resultW == -1)
					{
						if(_debug){ std::cerr << strerror(errno) << std::endl; }

						close(sock);

						free(request);
						free(response);

						request	=	0x00;
						response	=	0x00;

						exit(-5);
					}

					if(_debug){ std::cout << "Write in port: " << resultW << std::endl; }

					if(poll(&_eventer, 1, _timeMsWaitResponse))
					{
						resultR2 = read(_hardwarePort, response, _maxSizeResponse);

						if(resultR2 == -1)
						{
							if(_debug){ std::cerr << strerror(errno) << std::endl; }

							free(request);
							free(response);

							request	=	0x00;
							response	=	0x00;

							exit(-6);
						}

						if(_debug){ std::cout << "Receive from port: " << resultR2 << std::endl; }
					}
					else{ if(_debug){ std::cerr << "Nothing was read from port" << std::endl; } }

					printf("%i\n", resultR2);

					if(flock(_hardwarePort, LOCK_UN) == -1)
					{
						if(_debug){ std::cerr << strerror(errno) << std::endl; }

						close(sock);

						free(request);
						free(response);

						request	=	0x00;
						response	=	0x00;

						exit(-7);
					}

					resultW2 = write(sock, response, resultR2);

					if(resultW2 == -1)
					{
						if(_debug){ std::cerr << strerror(errno) << std::endl; }

						close(sock);

						free(request);
						free(response);

						request	=	0x00;
						response	=	0x00;

						exit(-8);
					}

					
					if(_debug){ std::cout << "Write in socket: " << resultW2 << " " << response << std::endl; }

					memset(request, 0, _maxSizeRequest);
					memset(response, 0, _maxSizeResponse);	
				}

				close(sock);

				free(request);
				free(response);

				request	=	0x00;
				response	=	0x00;

				exit(0);
			}
			else{ close(sock); }
		}
		else{ if(_debug){ std::cerr << strerror(errno) << std::endl; } }
	}
}

bool	Server::settings()
{
	if(!settingsPort()){ return false; }

	if((_sockServer = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		if(_debug){ std::cerr << strerror(errno) << std::endl; }

		close(_hardwarePort);

		_hardwarePort = -1;

		return false;
	}

	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = inet_addr(_ip.data());
	_addr.sin_port = htons(_port);

	if(bind(_sockServer, (struct sockaddr*)&_addr, sizeof(struct sockaddr_in)) == -1)
	{
		if(_debug){ std::cerr << strerror(errno) << std::endl; }

		close(_sockServer);
		close(_hardwarePort);

		_sockServer = -1;
		_hardwarePort = -1;

		return false;
	}

	if(listen(_sockServer, _maxUsers) == -1)
	{
		if(_debug){ std::cerr << strerror(errno) << std::endl; }

		close(_sockServer);
		close(_hardwarePort);

		_sockServer = -1;
		_hardwarePort = -1;

		return false;
	}

	_act.sa_flags = SA_NOCLDWAIT | SA_RESTART;
	_act.sa_sigaction	= handlerChild;

	sigaction(SIGCHLD, &_act, 0);	

	close(_hardwarePort);
	_hardwarePort = -1;

	return true;
}

bool	Server::settingsPort()
{
	_hardwarePort = open(_device.data(), (O_RDWR | O_NOCTTY | O_SYNC));

	if(_hardwarePort == -1)
	{
		if(_debug){ std::cerr << strerror(errno) << std::endl; }

		return false;
	}

	if(!isatty(_hardwarePort))
	{
		if(_debug){ std::cerr << strerror(errno) << std::endl; }

		return false;
	}

	if(tcsetattr(_hardwarePort, TCSANOW, &_ttyRes) == -1)
	{
		if(_debug){ std::cerr << strerror(errno) << std::endl; }

		close(_hardwarePort);

		_hardwarePort = -1;

		return false;
	}

	if(tcgetattr(_hardwarePort, &_tty) == -1)
	{
		if(_debug){ std::cerr << strerror(errno) << std::endl; }

		close(_hardwarePort);

		_hardwarePort = -1;

		return false;
	}

	if(chooseStopBit(_stopBit) == -1)
	{
		if(_debug){ std::cerr << strerror(errno) << std::endl; }

		close(_hardwarePort);

		return false;
	}

	if(chooseDataBit(_dataBit) == -1)
	{
		if(_debug){ std::cerr << strerror(errno) << std::endl; }

		close(_hardwarePort);

		_hardwarePort = -1;

		return false;
	}

	if(chooseParity(_parity) == -1)
	{
		if(_debug){ std::cerr << strerror(errno) << std::endl; }

		close(_hardwarePort);

		_hardwarePort = -1;

		return false;
	}

	if(chooseSpeed(_baud) == -1)
	{
		if(_debug){ std::cerr << strerror(errno) << std::endl; }

		close(_hardwarePort);

		_hardwarePort = -1;

		return false;
	}

	_tty.c_cflag |= (CREAD | CLOCAL);

	_tty.c_lflag = 0;

	_tty.c_iflag = 0;
	_tty.c_iflag &= ~(INLCR | IGNCR | ICRNL);

	_tty.c_oflag &=	~OPOST;

	_tty.c_cc[VMIN] = 1;
	_tty.c_cc[VTIME] = 0;

	if(ioctl(_hardwarePort, TCSETS, &_tty) == -1)
	{
		if(_debug){ std::cerr << strerror(errno) << std::endl; }

		close(_hardwarePort);

		_hardwarePort = -1;

		return false;
	}

	_eventer.fd	=	_hardwarePort;
	_eventer.events	=	POLLIN;

	return true;
}

int8_t	Server::chooseStopBit(int32_t&	stopBit)
{
	_tty.c_cflag &= ~CSTOPB;

	switch(stopBit)
	{
		case 1:
		{
			_tty.c_cflag &= CSTOPB;

			break;
		}

		case 2:
		{
			_tty.c_cflag |= CSTOPB;

			break;
		}

		default:{ return -1; }
	}

	return 0;
}

int8_t	Server::chooseDataBit(int32_t&	dataBit)
{
	_tty.c_cflag &= ~CSIZE;

	switch(dataBit)
	{
		case 7:
		{
			_tty.c_cflag |= CS7;

			break;
		}

		case 8:
		{
			_tty.c_cflag |= CS8;

			break;
		}

		default:{ return -1; }
	}

	return 0;
}

int8_t Server::chooseParity(char&	parity)
{
	_tty.c_cflag &= ~PARENB;

	switch(parity)
	{
		case 'N':
		{
			_tty.c_cflag |= IGNPAR;

			break;
		}

		case 'O':
		{
			_tty.c_cflag |= PARODD;

			break;
		}

		case 'E':
		{
			_tty.c_cflag |= PARENB;

			break;
		}

		default:{ return -1; }
	}

	return 0;
}

int8_t	Server::chooseSpeed(int32_t&	speed)
{
	switch(speed)
	{
		case 0:
		{
			if(cfsetispeed(&_tty, B0) == -1){ return -1; }
			if(cfsetospeed(&_tty, B0) == -1){ return -1; }

			break;
		}
		case 50:
		{
			if(cfsetispeed(&_tty, B50) == -1){ return -1; }
			if(cfsetospeed(&_tty, B50) == -1){ return -1; }

			break;
		}
		case 75:
		{
			if(cfsetispeed(&_tty, B75) == -1){ return -1; }
			if(cfsetospeed(&_tty, B75) == -1){ return -1; }

			break;
		}
		case 110:
		{
			if(cfsetispeed(&_tty, B110) == -1){ return -1; }
			if(cfsetospeed(&_tty, B110) == -1){ return -1; }

			break;
		}
		case 134:
		{
			if(cfsetispeed(&_tty, B134) == -1){ return -1; }
			if(cfsetospeed(&_tty, B134) == -1){ return -1; }

			break;
		}
		case 150:
		{
			if(cfsetispeed(&_tty, B150) == -1){ return -1; }
			if(cfsetospeed(&_tty, B150) == -1){ return -1; }

			break;
		}
		case 200:
		{
			if(cfsetispeed(&_tty, B200) == -1){ return -1; }
			if(cfsetospeed(&_tty, B200) == -1){ return -1; }

			break;
		}
		case 300:
		{
			if(cfsetispeed(&_tty, B300) == -1){ return -1; }
			if(cfsetospeed(&_tty, B300) == -1){ return -1; }

			break;
		}
		case 600:
		{
			if(cfsetispeed(&_tty, B600) == -1){ return -1; }
			if(cfsetospeed(&_tty, B600) == -1){ return -1; }

			break;
		}
		case 1200:
		{
			if(cfsetispeed(&_tty, B1200) == -1){ return -1; }
			if(cfsetospeed(&_tty, B1200) == -1){ return -1; }

			break;
		}
		case 1800:
		{
			if(cfsetispeed(&_tty, B1800) == -1){ return -1; }
			if(cfsetospeed(&_tty, B1800) == -1){ return -1; }

			break;
		}
		case 2400:
		{
			if(cfsetispeed(&_tty, B2400) == -1){ return -1; }
			if(cfsetospeed(&_tty, B2400) == -1){ return -1; }

			break;
		}
		case 4800:
		{
			if(cfsetispeed(&_tty, B4800) == -1){ return -1; }
			if(cfsetospeed(&_tty, B4800) == -1){ return -1; }

			break;
		}
		case 9600:
		{
			if(cfsetispeed(&_tty, B9600) == -1){ return -1; }
			if(cfsetospeed(&_tty, B9600) == -1){ return -1; }

			break;
		}
		case 19200:
		{
			if(cfsetispeed(&_tty, B19200) == -1){ return -1; }
			if(cfsetospeed(&_tty, B19200) == -1){ return -1; }

			break;
		}
		case 38400:
		{
			if(cfsetispeed(&_tty, B38400) == -1){ return -1; }
			if(cfsetospeed(&_tty, B38400) == -1){ return -1; }

			break;
		}
		case 57600:
		{
			if(cfsetispeed(&_tty, B57600) == -1){ return -1; }
			if(cfsetospeed(&_tty, B57600) == -1){ return -1; }

			break;
		}
		case 115200:
		{
			if(cfsetispeed(&_tty, B115200) == -1){ return -1; }
			if(cfsetospeed(&_tty, B115200) == -1){ return -1; }

			break;
		}
		case 230400:
		{
			if(cfsetispeed(&_tty, B230400) == -1){ return -1; }
			if(cfsetospeed(&_tty, B230400) == -1){ return -1; }

			break;
		}
		default:{ return -1; }
	}

	return 0;
}

void	Server::handlerChild(int32_t	sig, siginfo_t*	info, void*	context){ --Server::g_connections; }

int32_t	Server::g_connections = 0;
