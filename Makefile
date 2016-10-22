CPPC=g++
CPPFLAGS= -o2 -pthread -std=c++11
OBJ=main.o server.o hashTable.o index.o cluster.o

Server : $(OBJ)
	$(CPPC) $(CPPFLAGS) $(OBJ) -o server

main.o : main.cpp
	$(CPPC) $(CPPFLAGS) -c main.cpp -o main.o

server.o : server.cpp
	$(CPPC) $(CPPFLAGS) -c server.cpp -o server.o

hashTable.o : hashTable.cpp
	$(CPPC) $(CPPFLAGS) -c hashTable.cpp -o hashTable.o

index.o : index.cpp
	$(CPPC) $(CPPFLAGS) -c index.cpp -o index.o

cluster.o : cluster.cpp
	$(CPPC) $(CPPFLAGS) -c cluster.cpp -o cluster.o

clean :
	rm *.o

