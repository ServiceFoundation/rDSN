<?php
require_once($argv[1]); // type.php
require_once($argv[2]); // program.php
$file_prefix = $argv[3];
?>

TARGET = <?=$_PROG->name?> 
INC_EXTRA = 
LIB_EXTRA = 
TYPE = EXE

ROOT = .

SYS_INC = /usr/local/include /usr/include
SYS_LIBS = \
	-lpthread \
	-lstdc++ \
	-lboost_chrono \
	-lboost_date_time \
	-lboost_filesystem \
	-lboost_regex \
    -lboost_system \
    -ldsn.tools.simulator \
    -ldsn.tools.common \
    -ldsn.dev \
    -ldsn.core  

BIN_DIR = $(ROOT)
SOURCES = $(wildcard *.cpp)
OBJS = $(SOURCES:%.cpp=%.o)

CPP = g++
CPPFLAGS = -std=c++11 -I$(ROOT)/inc $(SYS_INC:%= -I%) -I./ $(INC_EXTRA:%= -I%) -O0 -g 

######
ifeq (/Users/$(LOGNAME), $(HOME))  # mac

	SYS_LIB = /usr/local/lib
	SYS_LIB_DEPS = \
		$(SYS_LIBS)				 \
		-lboost_thread-mt					 
										 
	LD = g++							     
	LDFLAGS = -L$(BIN_DIR) $(SYS_LIB:%= -L%) $(SYS_LIB_DEPS) 
										 
else # linux									 
										 
	SYS_LIB = /usr/lib/x86_64-linux-gnu /usr/local/lib
	SYS_LIB_DEPS = \
		$(SYS_LIBS)	\
		-lboost_thread \
		-lrt								  
										 
	LIB_NONLINKED_PATH = /usr/lib/x86_64-linux-gnu
	LIB_NONLINKED_WR = \
				   $(LIB_NONLINKED_PATH)/libboost_thread.a		\
				   $(LIB_NONLINKED_PATH)/libboost_filesystem.a	\
				   $(LIB_NONLINKED_PATH)/librt.a
										 
	LD = g++ -pthread
	LDFLAGS = -L$(BIN_DIR) $(SYS_LIB:%= -L%) $(SYS_LIB_DEPS)  $(LIB_NONLINKED_WR) 

endif
##########

AR = ar
ARFLAGS = rv

all: $(TYPE)

$(BIN_DIR):
	-mkdir $(BIN_DIR)

clean:
	-rm *.o
	-rm $(BIN_DIR)/$(TARGET)

%.o : %.cpp
	$(CPP) -c $(CPPFLAGS) $< -o $@

LIB: $(BIN_DIR) $(OBJS)
	echo building library $(BIN_DIR)/$(TARGET) ...
	$(AR) $(ARFLAGS) $(BIN_DIR)/$(TARGET) $(OBJS)

EXE: $(BIN_DIR) $(OBJS) $(LIB_EXTRA_TARGETS)
	echo building executable $(BIN_DIR)/$(TARGET) ...
	$(LD) -o $(BIN_DIR)/$(TARGET) $(OBJS) $(LDFLAGS) 

