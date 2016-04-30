CC 	= g++-4.9 -std=c++14
CXXFLAG	= -O0 -Wall -Wextra -W -ggdb3 -lrestclient-cpp -lcurl

Server: Server.o
	$(CC) $^ -o $@ $(CXXFLAG)

Server.o: Server.cpp
	$(CC) $< -c $(CXXFLAG)
	
clean:
	rm -f *.o Server

rebuild:	clean Server

.PHONY:	clean rebuild
