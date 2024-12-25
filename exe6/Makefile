CC=g++
CFLAGS=-Wall -g

all: kosaraju

kosaraju: kosaraju.o reactor.o
	$(CC) $(CFLAGS) kosaraju.o reactor.o -o kosaraju

kosaraju.o: kosaraju.cpp
	$(CC) -c $(CFLAGS) kosaraju.cpp	

reactor.o: reactor.cpp
	$(CC) -c $(CFLAGS) reactor.cpp		
clean:
	rm -f *.o ttt mync kosaraju