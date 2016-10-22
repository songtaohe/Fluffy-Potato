CPPC=g++
CPPFLAGS= -o2 -pthread

Server : main.o server.o
	$(CPPC) $(CPPFLAGS) main.o server.o -o server

main.o : main.cpp
	$(CPPC) $(CPPFLAGS) -c main.cpp -o main.o

server.o : server.cpp
	$(CPPC) $(CPPFLAGS) -c server.cpp -o server.o

clean :
	rm *.o

