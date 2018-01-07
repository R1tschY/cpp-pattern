# Output file
PROGRAM = CodePatternsTests

# Input files
SRCFILES = $(wildcard *.cpp) $(wildcard Classes/*.cpp)

# Programs
CC = gcc
CXX = g++
RM = rm

# Compiler flags
CFLAGS = -Og -g -Wall -Wextra -I.. -fmessage-length=0
CXXFLAGS = $(CFLAGS) -std=c++0x
LDFLAGS = -Og -g -Wall -Wextra  -std=c++0x
LIBS = 

# more checks (Concept checks, ...)
CXXFLAGS += -D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_DEBUG

# Boost tests
CXXFLAGS += -DBOOST_TEST_DYN_LINK
LIBS += -lboost_unit_test_framework

# Test coverage
CFLAGS += --coverage
LDFLAGS += --coverage
COVERAGE_REPORT_DIR = test_coverage_report
LCOV_FILE = $(PROGRAM).info
GENHTML_ARGS = -t "$(PROGRAM) test coverage" --num-spaces 2 --demangle-cpp --branch-coverage

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
	
check: $(PROGRAM)
	./$(PROGRAM)
	
check-coverage: $(PROGRAM)
	lcov --base-directory . --directory . --zerocounters -q
	./$(PROGRAM)
	lcov -q --base-directory . --directory . -c -o $(PROGRAM).info
	lcov -q --remove $(PROGRAM).info "/usr*" -o $(PROGRAM).info
	rm -rf $(COVERAGE_REPORT_DIR)
	genhtml -q -o $(COVERAGE_REPORT_DIR) $(GENHTML_ARGS) $(PROGRAM).info
	find $(COVERAGE_REPORT_DIR) -type f -name '*.html' -execdir sed -i 's/std::basic_string\&lt;char, std::char_traits\&lt;char\&gt;, std::allocator\&lt;char\&gt; \&gt;/std::string/g' {} +
	find $(COVERAGE_REPORT_DIR) -type f -name '*.html' -execdir sed -i 's/std::unique_ptr\&lt;\(.*\), std::default_delete\&lt;\(.*\)\&gt; \&gt;/std::unique_ptr\&lt;\1\&gt;/' {} +
	x-www-browser file://`pwd`/$(COVERAGE_REPORT_DIR)/index.html

clean:
	$(RM) -rf *.o *.o.d $(PROGRAM) $(COVERAGE_REPORT_DIR)
	
-include $(wildcard *.o.d)
-include $(wildcard Classes/*.o.d)
