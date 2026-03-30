CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# (a) Build the module object file
rope.o: rope.h rope.cpp
	$(CXX) $(CXXFLAGS) -c rope.cpp -o rope.o

# (b) Build the demo executable
demo.o: rope.o demo.cpp
	$(CXX) $(CXXFLAGS) demo.cpp rope.o -o demo.o

# (c) Run the demo
demo: demo.o
	./demo.o

# (d) Build the test executable
test.o: rope.o test.cpp
	$(CXX) $(CXXFLAGS) test.cpp rope.o -o test.o

# (e) Run the tests (writes to log.txt)
test: test.o
	./test.o

# (f) Clean all generated artefacts
clean:
	rm -f *.o log.txt

# (g) Rebuild everything from scratch
rebuild: clean demo.o test.o