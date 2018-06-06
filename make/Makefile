CXX       := g++
CXXFLAGS  := -Wall -std=c++0x -v

TARGET    := block_identifier
PROJ_ROOT := $(realpath ..)
VPATH     := $(PROJ_ROOT)/block_identifier
INCLUDES  := `pkg-config opencv --cflags` \
	     -I $(PROJ_ROOT)/block_identifier
SRCS      := identify.cpp \
			 main.cpp \
			 option.cpp \
			 sender.cpp \
			 trigger.cpp
LIBS      := -lpthread \
	     -ldl \
	     `pkg-config opencv --libs` \
	     -lboost_serialization \
	     -lboost_system \
		 -lboost_program_options \
		 -lbcm2835 \
	     -lm

UNAME := ${shell uname}
ifeq ($(UNAME), Linux)
CXXFLAGS := $(CXXFLAGS) -DENABLE_RASPBERRY_PI_CAMERA
LIBS := $(LIBS) -lraspicamcv
endif

OBJS      := $(SRCS:.cpp=.o)

### Rules ######################################################################

.PHONY: all
all: $(TARGET)
.PHONY: make
make: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS)

.PHONY: clean
clean:
	$(RM) -f $(TARGET) *.o

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $<
