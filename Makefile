LEPT   := /usr/include/leptonica
BOOST  := /usr/include/boost
THRIFT := /usr/local/include/thrift  

CC=g++
CFLAGS=-O -c -Wall 
LDFLAGS=-O

SRC=src/main/cpp
GEN_THRIFT=build/gen-cpp
INC=-I$(BOOST) -I$(THRIFT)
INC_SRCS=-I$(SRC) -I$(GEN_THRIFT)
LIBS=-lc -lz -llept -lboost_thread -lboost_program_options -lthrift -lthriftnb -levent



EXE=image-server
OUT=build

THRIFT_SRC=src/main/thrift/service.thrift
#SOURCES=src/main/cpp/server.cpp src/main/cpp/ImageServiceImpl.cpp 
#SOURCES=src/main/cpp/server.cpp
OBJECTS:= service_types.o service_constants.o ImageService.o ImageServiceImpl.o server.o 

#$(patsubst %,$(ODIR)/%,$(_OBJS))
OBJS := $(patsubst %,$(OUT)/%,$(OBJECTS))


all: init thrift link clear-tmps

init:
	mkdir -p $(OUT)
   
link: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(OUT)/$(EXE) $(INC) $(INC_SRCS) $(LIBS)

#compile sources
#$(CC) -c $(INC) -o $@ $< $(CFLAGS)
$(OUT)/%.o: $(GEN_THRIFT)/%.cpp
	$(CC) $(INC) $(INC_SRCS) -o $@ $< $(CFLAGS)
	

$(OUT)/%.o: $(SRC)/%.cpp			
	$(CC) $(INC) $(INC_SRCS) -o $@ $< $(CFLAGS)

#build/ImageServiceImpl.o: 		
#	$(CC) $(INC) $(INC_SRCS) -o $@ -Lsrc/main/cpp ImageServiceImpl.cpp $(CFLAGS)

clear-tmps:
	#rm -fr *.o
	#rm -fr *~		
				
clean: 
	rm -fr build

thrift:
	thrift -o $(OUT) --gen js:node $(THRIFT_SRC)
	thrift -o $(OUT) --gen cpp $(THRIFT_SRC)
	thrift -o $(OUT) --gen py $(THRIFT_SRC)
	
.PHONY:    		
	echo "phony"