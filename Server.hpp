#ifndef SERVER_HPP
#define SERVER_HPP
	
	#include <iostream>

	#include <string_view>

	#include <cerrno>
	#include <cstring>
	#include <cstdlib>
	#include <cstdint>

	#include <signal.h>

	#include <unistd.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <fcntl.h>
	#include <termios.h>

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/file.h>
	#include <sys/ioctl.h>
	#include <sys/poll.h>

	class Server
	{
			public:
							explicit	Server(std::string_view	ip, std::string_view	device, int32_t	port, int32_t	stopBit, int32_t	dataBit, int32_t	baud, int32_t	maxUsers, char	parity, int32_t	timeMsWaitResponse, int32_t	maxSizeResponse, int32_t	maxSizeRequest, int32_t	debug);
							//конструктор
							explicit	Server(void);
							//конструктор

							virtual	~Server(void);
							//деструктор

							static	int32_t	g_connections;


							std::string_view	getIp(void)	const;
							//получение адреса сервера
							std::string_view	getDevice(void)	const;
							//получение названия устройства

							int32_t	getPort(void)	const;
							//получение названия порта
							int32_t	getStopBit(void)	const;
							//получение стопового бита для определния конца сообщения
							int32_t	getDataBit(void)	const;
							//получение количества битов передоваемых за раз
							int32_t	getBaud(void)	const;
							//получение скорости данных в битах в секунду
							int32_t	getMaxUsers(void)	const;
							//получение максимального количества пользователей
							int32_t	getTimeMsWaitResponse(void)	const;
							//получение времени ожидания ответа в миллисекундах
							int32_t	getMaxSizeResponse(void)	const;
							//получение максимального размера ответной посылки
							int32_t	getMaxSizeRequest(void)	const;
							//получение максимального размера отправной посылки
							int32_t	getDebug(void)	const;
							//получение флага отладки

							char	getParity(void)	const;
							//получение бита четности


							void	setIp(std::string_view	ip);
							//запись адреса сервера
							void	setDevice(std::string_view	device);
							//запись названия устройства

							void	setPort(int32_t	port);
							//запись названия порта
							void	setStopBit(int32_t	stopBit);
							//запись стопового бита для определния конца сообщения
							void	setDataBit(int32_t	dataBit);
							//запись количества битов передаваемых за раз
							void	setBaud(int32_t	baud);
							//запись скорости данных в битах в секунду
							void	setMaxUsers(int32_t	maxUsers);
							//запись максимального количества пользователей
							void	setTimeMsWaitResponse(int32_t	timeMsWaitResponse);
							//запись времени ожидания ответа в миллисекундах
							void	setMaxSizeResponse(int32_t	maxSizeResponse);
							//запись максимального размера ответной посылки
							void	setMaxSizeRequest(int32_t	maxSizeRequest);
							//запись максимального размера отправной посылки
							void	setDebug(int8_t	debug);
							//получение флага отладки

							void	start(void);
							//запуск сервера

							void	setParity(char	parity);
							//установка бита четности

							bool	settings(void);
							//настройка сети
			private:
							int8_t	chooseStopBit(int32_t&	stopBit);
							int8_t	chooseDataBit(int32_t&	dataBit);
							int8_t	chooseSpeed(int32_t&	speed);

							int8_t	chooseParity(char&	parity);


							bool	settingsPort(void);
							//настройка com-порта


							static	void	handlerChild(int32_t	sig, siginfo_t*	info, void*	context);
							//обработчик завершения дочерних процессов


							std::string_view	_ip;
							std::string_view	_device;

							int32_t	_port;
							int32_t	_stopBit;
							int32_t	_dataBit;
							int32_t	_baud;
							int32_t	_maxUsers;
							int32_t	_debug;
							int32_t	_timeMsWaitResponse;
							int32_t	_maxSizeResponse;
							int32_t	_maxSizeRequest;

							int32_t	_sockServer;
							int32_t	_hardwarePort;

							char	_parity;

							struct termios	_tty;
							struct termios	_ttyRes;

							struct sockaddr_in	_addr;

							struct sigaction	_act;

							struct pollfd	_eventer;
	};

#endif
