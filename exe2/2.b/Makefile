CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pg
EXEC1 = q2WithAdjMatrix
EXEC2 = q2WithList
SOURCES1 = q2WithAdjMatrix.cpp
SOURCES2 = q2WithList.cpp
OBJECTS1 = $(SOURCES1:.cpp=.o)
OBJECTS2 = $(SOURCES2:.cpp=.o)

all: $(EXEC1) $(EXEC2)

$(EXEC1): $(OBJECTS1)
	$(CXX) $(OBJECTS1) $(CXXFLAGS) -o $(EXEC1)  

$(EXEC2): $(OBJECTS2)
	$(CXX) $(OBJECTS2) $(CXXFLAGS) -o $(EXEC2)  
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC1) $(EXEC2) $(OBJECTS1) $(OBJECTS2) gmon.out
