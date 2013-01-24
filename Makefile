CXX:=clang++
LD:=$(CXX)
CC:=clang

# Build Flags
CXX_BUILD_FLAGS := -g -O3 -Wall -Wextra -std=c++11 -stdlib=libc++
LINKER_FLAGS := -stdlib=libc++


sources := main.cpp parser.cpp
objects := $(subst .cpp,.o,$(sources))


all: $(objects)
	$(LD) -o main $(objects) $(LINKER_FLAGS)

%.o: %.cpp 
	$(CXX) $(CXX_BUILD_FLAGS) $(include_flags) -c -o $@ $<

clean:
	$(RM) $(objects)
