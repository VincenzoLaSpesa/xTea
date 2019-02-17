debug: copyFiles
debug: all
debug: CFLAGS +=-DDEBUG -ggdb

all: xteaBin

xteaBin: main.o functions.o xTea.o
	$(CXX) $(CFLAGS) main.o functions.o xTea.o -o xTea.bin

main.o: main.cpp
	$(CXX) $(CFLAGS) -c main.cpp

functions.o: functions.cpp
	$(CXX) $(CFLAGS) -c functions.cpp

xTea.o: xTea.cpp
	$(CXX) $(CFLAGS) -c xTea.cpp

clean:
	rm -rf *.o 

copyFiles:
	cp README.md in.txt

test: debug
	@md5sum in.txt
	@md5sum replain.txt
	@md5sum replain_cbc.txt