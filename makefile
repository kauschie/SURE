all: main

clean:
	rm -f *.o *.exe *.out main

main: main.o Temperature.h Test.h Stats.h
	g++ -o main main.o

main.o: main.cpp
	g++ -c main.cpp

