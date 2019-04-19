all: target

debug: CXXFLAGS = -DDEBUG -g
debug: target

target: main.o functions.o xTea.o
	$(CXX) main.o functions.o xTea.o -o xTea.bin

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

functions.o: functions.cpp
	$(CXX) $(CXXFLAGS) -c functions.cpp

xTea.o: xTea.cpp
	$(CXX) $(CXXFLAGS) -c xTea.cpp

clean:
	rm -rf *.o 
