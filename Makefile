#PATH_TO_CONFIG="\"/var/nfs/Fluffy-Potato/Cluster.cfg\""
#PATH_TO_NODEID="\"/usr/local/hstnode\""
SRC=src/
INC=include
PATH_TO_CONFIG="\"/home/songtao/Mapmaking/Fluffy-Potato/Cluster.cfg\""
PATH_TO_NODEID="\"/home/songtao/Mapmaking/Fluffy-Potato/nodeid\""
MACRO=-DPATH_TO_CONFIG=$(PATH_TO_CONFIG) -DPATH_TO_NODEID=$(PATH_TO_NODEID)
CPPC=g++
CPPFLAGS= -o2 -pthread -std=c++11 -fPIC $(MACRO) -I$(INC)
OBJ=server.o hashTable.o index.o index_rtree.o cluster.o client.o 
#W_OBJ = client.o

All : Client Server Wrapper

Wrapper : $(OBJ) pythonWrapper.o
	$(CPPC) $(CPPFLAGS) pythonWrapper.o $(OBJ) -shared -o pythonWrapper.so

Client : $(OBJ) testClient.o
	$(CPPC) $(CPPFLAGS) testClient.o $(OBJ) -o client

Server : $(OBJ) main.o
	$(CPPC) $(CPPFLAGS) main.o $(OBJ) -o server

pythonWrapper.o: $(SRC)pythonWrapper.cpp
	$(CPPC) $(CPPFLAGS) -c $(SRC)pythonWrapper.cpp -o pythonWrapper.o 

testClient.o : $(SRC)testClient.cpp
	$(CPPC) $(CPPFLAGS) -c $(SRC)testClient.cpp -o testClient.o

client.o : $(SRC)client.cpp
	$(CPPC) $(CPPFLAGS) -c $(SRC)client.cpp -o client.o

main.o : $(SRC)main.cpp
	$(CPPC) $(CPPFLAGS) -c $(SRC)main.cpp -o main.o

server.o : $(SRC)server.cpp
	$(CPPC) $(CPPFLAGS) -c $(SRC)server.cpp -o server.o

hashTable.o : $(SRC)hashTable.cpp
	$(CPPC) $(CPPFLAGS) -c $(SRC)hashTable.cpp -o hashTable.o

index.o : $(SRC)index.cpp
	$(CPPC) $(CPPFLAGS) -c $(SRC)index.cpp -o index.o

index_rtree.o : $(SRC)index_rtree.cpp
	$(CPPC) $(CPPFLAGS) -c $(SRC)index_rtree.cpp -o index_rtree.o

cluster.o : $(SRC)cluster.cpp
	$(CPPC) $(CPPFLAGS) -c $(SRC)cluster.cpp -o cluster.o

clean :
	rm *.o

