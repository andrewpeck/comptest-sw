CXX = clang++

SRCDIR = src
OBJDIR = obj



SOURCES=src/adc.cpp src/comp_test.cpp src/dac.cpp src/ddd.cpp src/emulator.cpp src/lctcomp.cpp src/menu.cpp src/mux.cpp src/serial.cpp

#$(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

CXXFLAGS = -std=c++11 -g -Wall -O3 -I. -Wno-unused-const-variable
#CXXFLAGS = -std=c++11 -g -Wall -O3 -I. -Demulate -Wno-unused-const-variable
LDFLAGS  = -lftdi1

TARGET = comptest

all: $(TARGET)

test: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o adctest $@ $& $(LDFLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJECTS) : $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	$(RM) obj/*.o
	$(RM) comptest
