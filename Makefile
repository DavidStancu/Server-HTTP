CXX      := g++
CXXFLAGS := -std=c++11 -Isingle_include -Wall

SRCS     := client.cpp helper.cpp requests.cpp
OBJS     := $(SRCS:.cpp=.o)
TARGET   := client

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
