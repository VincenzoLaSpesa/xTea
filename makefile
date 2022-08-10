all: target

debug: CXXFLAGS = -DDEBUG -g
debug: target

target: xTea_console.o xTea.o
	$(CXX) xTea_console.o xTea.o -o xTea.bin

xTea_console.o: xTea_console.cpp
	$(CXX) $(CXXFLAGS) -c xTea_console.cpp

xTea.o: ./src/xTea.cpp
	$(CXX) $(CXXFLAGS) -c ./src/xTea.cpp

clean:
	rm -rf *.o 
