CC=g++
CFLAGS=-Wall -g

all: kosaraju

kosaraju: kosaraju.o 
	$(CC) $(CFLAGS) kosaraju.o -o kosaraju

kosaraju.o: kosaraju.cpp
	$(CC) -c $(CFLAGS) kosaraju.cpp	
	
clean:
	rm -f *.o kosaraju
