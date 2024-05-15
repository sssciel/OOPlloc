CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

TARGET = main
SRCS = main.cpp allocator/ooplloc.cpp allocator/mutexLock.cpp allocator/atomicStructs.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
