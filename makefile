all: main infinity

clean:
	rm -f *.o *.exe *.out main infinity

main: main.o Temperature.h Test.h Stats.h Menu.h
	g++ -o main main.o -lwiringPi

main.o: main.cpp
	g++ -c main.cpp

infinity: infinity.cpp
	g++ -o infinity infinity.cpp


