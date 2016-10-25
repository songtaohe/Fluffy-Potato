CPPC=g++
CPPFLAGS= -o2 -pthread -std=c++11 -fPIC
OBJ=server.o hashTable.o index.o cluster.o client.o
#W_OBJ = client.o

All : Client Server Wrapper

Wrapper : $(OBJ) pythonWrapper.o
	$(CPPC) $(CPPFLAGS) pythonWrapper.o $(OBJ) -shared -o pythonWrapper.so

Client : $(OBJ) testClient.o
	$(CPPC) $(CPPFLAGS) testClient.o $(OBJ) -o client

Server : $(OBJ) main.o
	$(CPPC) $(CPPFLAGS) main.o $(OBJ) -o server

pythonWrapper.o: pythonWrapper.cpp
	$(CPPC) $(CPPFLAGS) -c pythonWrapper.cpp -o pythonWrapper.o 

testClient.o : testClient.cpp
	$(CPPC) $(CPPFLAGS) -c testClient.cpp -o testClient.o

client.o : client.cpp
	$(CPPC) $(CPPFLAGS) -c client.cpp -o client.o

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

