CC = g++  -fexceptions -g
OUTFILE = ropammanager
OBJS = main.o  config.o ropam.o tcp.o logger.o Httpd.o Weatherd.o
LIBS = -lptypes -lpthread -lcurl -lboost_system -lmicrohttpd -ljansson
INCS = -I/usr/include/ptypes
FLAGS = -O0 -fexceptions -g  -std=c++11 -g3
CXXFLAGS = -O0 -fexceptions -g -std=c++11 -Wno-deprecated-declarations -g3  -g 
#-g

all: $(OUTFILE)
config.o: config.cpp config.h
		$(CC) $(CXXFLAGS) -c $(INCS) config.cpp		
main.o: main.cpp 
		$(CC) $(CXXFLAGS) -c $(INCS) main.cpp	
tcp.o: tcp.cpp 
		$(CC) $(CXXFLAGS) -c $(INCS) tcp.cpp			
logger.o: logger.cpp 
		$(CC) $(CXXFLAGS) -c $(INCS) logger.cpp
ropam.o: ropam.cpp 
		$(CC) $(CXXFLAGS) -c $(INCS) ropam.cpp
Httpd.o: Httpd.cpp 
		$(CC) $(CXXFLAGS) -c $(INCS) Httpd.cpp
Weatherd.o: Weatherd.cpp 
		$(CC) $(CXXFLAGS) -c $(INCS) Weatherd.cpp
tcpclient.o: tcpclient.cpp 
		$(CC) $(CXXFLAGS) -c $(INCS) tcpclient.cpp


$(OUTFILE): $(OBJS)
	$(CC) $(CXXFLAGS) $(FLAGS) $(OBJS) $(LIBS) -o $(OUTFILE)
	
clean:
	rm *.o	
	rm *.gch
	rm $(OUTFILE)

cleanall: clean  
	  rm  $(OUTFILE)
