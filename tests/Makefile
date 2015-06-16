# Programs
CC = gcc
CXX = g++
RM = rm

# Compiler flags
CFLAGS = -O2 -g -Wall -Wextra -I.. -fmessage-length=0
CXXFLAGS = $(CFLAGS) -std=c++0x
LDFLAGS = -O2 -g -Wall -Wextra  -std=c++0x
LIBS = 

# more checks (Concept checks, ...)
CXXFLAGS += -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_DEBUG

# Boost tests
CXXFLAGS += -DBOOST_TEST_DYN_LINK
LIBS += -lboost_unit_test_framework

# Output file
PROGRAM = CodePatternsTests

# Input files
SRCFILES = $(wildcard *.cpp) $(wildcard Classes/*.cpp)

################################################################################

OBJECTS1 = $(SRCFILES)
OBJECTS2 = $(OBJECTS1:%.c=%.o)
OBJECTS3 = $(OBJECTS2:%.cc=%.o)
OBJECTS4 = $(OBJECTS3:%.cpp=%.o)
OBJECTS  = $(OBJECTS4:%.cxx=%.o)

all: $(PROGRAM)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -MT $@ -MF $@.d -c $< -o $@
	
%.o: %.cc
	$(CXX) $(CXXFLAGS) -MMD -MP -MT $@ -MF $@.d -c $< -o $@
	
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -MT $@ -MF $@.d -c $< -o $@
	
%.o: %.cxx
	$(CXX) $(CXXFLAGS) -MMD -MP -MT $@ -MF $@.d -c $< -o $@

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $(PROGRAM) $(LDFLAGS) $(OBJECTS) $(LIBS)

clean:
	$(RM) -rf *.o *.o.d $(PROGRAM)
	
-include $(wildcard *.o.d)
-include $(wildcard Classes/*.o.d)
