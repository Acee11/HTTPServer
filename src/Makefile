Flags = -O2 -Wall -W -Wreturn-type -pedantic -pedantic-errors -Wundef -std=c++14
CPP = g++
BIN = main.out

main.out: main.o HttpMsg.o ClientHandler.o Server.o utils.o
	$(CPP) $(Flags) main.o HttpMsg.o ClientHandler.o Server.o utils.o -o main.out -lpthread

main.o: main.cpp
	$(CPP) $(Flags) -c main.cpp -o main.o -lpthread

Server.o: Server.h Server.cpp 
	$(CPP) $(Flags) -c Server.cpp -o Server.o -lpthread

HttpMsg.o: HttpMsg.h HttpMsg.cpp 
	$(CPP) $(Flags) -c HttpMsg.cpp -o HttpMsg.o -lpthread

ClientHandler.o: ClientHandler.h ClientHandler.cpp
	$(CPP) $(Flags) -c ClientHandler.cpp -o ClientHandler.o -lpthread

utils.o: utils.h utils.cpp
	$(CPP) $(Flags) -c utils.cpp -o utils.o



clean:
	rm -rf *.o
distclean:
	rm -rf $(BIN)

