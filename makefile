all: target

debug: CXXFLAGS = -DDEBUG -g
debug: target

target: main.o xTea.o
	$(CXX) main.o xTea.o -o xTea.bin

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

xTea.o: xTea.cpp
	$(CXX) $(CXXFLAGS) -c xTea.cpp

clean:
	rm -rf *.o 
