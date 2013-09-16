LEPT=/usr/include/leptonica
BOOST_DIR=/usr/include/boost

CC=g++
CFLAGS=-c -Wall
LDFLAGS=
OBJECTS=$(SOURCES:.cpp=.o)
LIBS=-llept -lboost_program_options


THRIFT_SRC=src/main/thrift/service.thrift
SOURCES=src/main/cpp/server.cpp


EXE=image-server

OUT=build

all: init thrift $(SOURCES) $(EXE) clear-tmps

init:
	mkdir -p $(OUT)
   
$(EXE): $(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUT)/$(EXE)
		
cpp.o:
		$(CC) $(CFLAGS) $< -o $@
		
clear-tmps:
		rm -fr *.o
		rm -fr *~		
				
clean: 
		rm -fr build

thrift:
		thrift -o $(OUT) --gen js:node $(THRIFT_SRC)
		thrift -o $(OUT) --gen cpp $(THRIFT_SRC)

    		
