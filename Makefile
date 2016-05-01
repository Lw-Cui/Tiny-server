CC 	= g++-4.9 -std=c++14
CXXFLAG	= -O0 -Wall -Wextra -W -ggdb3 -lrestclient-cpp -lcurl

all: Server Algorithm

Server: Server.o
	$(CC) $^ -o $@ $(CXXFLAG)

Algorithm: Algorithm.o
	$(CC) $^ -o $@ $(CXXFLAG)

%.o: %.cpp
	$(CC) $< -c $(CXXFLAG)

clean:
	rm -f *.o Server

rebuild:	clean Server

.PHONY:	clean rebuild all