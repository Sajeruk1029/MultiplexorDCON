#ifndef CONF_HPP
#define CONF_HPP

	#include <cstdint>
	#include <cctype>

	#include <fstream>
	#include <string_view>
	#include <string>

	class Config
	{
		public:
						explicit	Config(std::string_view	name);
						//конструктор
						virtual	~Config(void);
						//деструктор

						bool	create(void)	const;
						//создает конфигуарционный файл
						bool	exist(void)	const;
						//проверяет налаичие конфигурационного файла
						bool	isOpened(void)	const;
						//проверяет открыт ли конфигурационный файл

						bool	addValue(std::string_view	key, std::string_view	value)	const;
						//добавляет текстовое значение по ключу
						bool	addValue(std::string_view	key, int32_t	value)	const;
						//добавляет целочисленное значение по ключу
						bool	addValue(std::string_view	key, char	value)	const;
						//добавляет символьное значение по ключу

						bool	getValue(std::string_view	key, std::string&	value)	const;
						//получает текстовое значение по ключу
						bool	getValue(std::string_view	key, int32_t&	value)	const;
						//получает целочисленное значение по ключу
						bool	getValue(std::string_view	key, char&	value)	const;
						//получает символьное значение по ключу

						bool	open(std::ios_base::openmode	mode);
						//открывает конфигурационный файл
						bool	close(void);
						//закрывает конфигурационный файл
						bool	readAll(void);
						//читает полностью конфигурационный файл

						std::string_view	getName(void)	const;
						//получает имя конфигурационного файла
						std::string	getContent(void)	const;
						//получает содержимое конфигурационного файла
						bool	setName(std::string_view	name);
						//задает имя конфигуарционного файла

		private:
						bool	isNumber(std::string	str)	const;


						std::string_view	_name;
						
						std::fstream*	_file;

						std::string	_content;
	};

#endif
