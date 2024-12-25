CC=g++
CFLAGS=-Wall -g

all: kosaraju

kosaraju: kosaraju.o proactor.o
	$(CC) $(CFLAGS) kosaraju.o proactor.o -o kosaraju

kosaraju.o: kosaraju.cpp
	$(CC) -c $(CFLAGS) kosaraju.cpp	

proactor.o: proactor.cpp
	$(CC) -c $(CFLAGS) proactor.cpp		
clean:
	rm -f *.o kosaraju
