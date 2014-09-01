all: hello

hello: main.o functions.o xTea.o
	g++ main.o functions.o xTea.o -o xTea.bin

main.o: main.cpp
	g++ -c main.cpp

functions.o: functions.cpp
	g++ -c functions.cpp

xTea.o: xTea.cpp
	g++ -c xTea.cpp

clean:
	rm -rf *.o 
