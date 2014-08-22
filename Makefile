TARGET	= ./packer

SOURCES = main.cpp vfs.cpp 
HEADERS = common.h vfs.h

CXX = clang++

CXX_FLAGS  = -c -Wall -Wextra -pedantic -std=c++11 -O3  -Wno-variadic-macros 
LINK_FLAGS = -stdlib=libc++ 
OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(HEADERS) $(TARGET) Makefile
	rm -f $(OBJECTS)

$(TARGET): $(OBJECTS) $(HEADERS)  Makefile
	$(CXX) $(OBJECTS) $(LINK_FLAGS) -o $@
	
.cpp.o: $(SOURCES)  $(HEADERS) 
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
	
	