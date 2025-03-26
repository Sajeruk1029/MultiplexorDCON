CXX	=	g++

SOURCES	=	main.cpp Server.cpp Config.cpp
OBJECTS	=	main.o Server.o Config.o

WFLAGS	=	-Wall -Wpedantic -Wextra
#DFLAGS	=	-g

STD	=	-std=c++17

PROGNAME	=	DCONServer


all	:	$(PROGNAME)


$(PROGNAME)	:	$(OBJECTS)
	$(CXX) $(WFLAGS) $(OBJECTS) -o $(PROGNAME) $(STD) $(DFLAGS)


main.o	:	main.cpp
	$(CXX) $(WFLAGS) main.cpp -c $(STD) $(DFLAGS)

Server.o	:	Server.cpp
	$(CXX) $(WFLAGS) Server.cpp -c $(STD) $(DFLAGS)

Config.o	:	Config.cpp
	$(CXX) $(WFLAGS) Config.cpp -c $(STD) $(DFLAGS)

clean	:
	rm -rf $(OBJECTS) $(PROGNAME)

install	:
	cp $(PROGNAME) /bin/

uninstall	:
	rm -rf /bin/$(PROGNAME)


