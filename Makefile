# Specify Target Application
TARGET = test_governor

# Specify Desired Compilers
CXX := g++
LINK := g++

# Includes
INCLUDES = 

# Common flags
COMMONFLAGS += -std=c++11 $(INCLUDES)
CXXFLAGS += -pipe $(COMMONFLAGS)
LIBS := -lboost_program_options -lboost_system -lboost_thread -lpthread -lboost_filesystem
CPP_FILES = $(wildcard ./*.cpp)
OBJS =  $(patsubst %.cpp,./%.cpp.o,$(notdir $(CPP_FILES)))

# Linker
LINKLINE = $(LINK) -o $(TARGET) $(OBJS) $(LIBS)

# Declare file types
.SUFFIXES: .cpp .o

# Setup our target
all: release

# Prevent Make from rebuilding Makefile
Makefile: ;

# Setup Debug Compile
debug: CXXFLAGS	    +=  -DDEBUG -g -Wall
debug: $(TARGET) 
	@echo Debug Done

# Setup Release Compile
release: CXXFLAGS	+=  -O3
release: $(TARGET) 
	@echo Runtime Done

# Build Application
$(TARGET): $(OBJS) Makefile
	$(LINKLINE)

# Compile CPP Files
%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean Up!
clean:
	rm -f rm -rf $(TARGET) $(OBJS)