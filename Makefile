CC = gcc
CCFLAGS = -Wno-unused-result
LDFLAG = -lstdc++
INCLUDE = -I .
OPT = -O2

all: solver checker

.PHONY: all clean

clean:
	-rm *.o
	-rm solver
	-rm checker

solver.o: main.cpp
	$(CC) $(CCFLAGS) $(OPT) -c $^ -o $@ $(INCLUDE)

solver: solver.o
	$(CC) $(CCFLAGS) $(OPT) -o $@ $^ $(INCLUDE) $(LDFLAG)

checker.o: checker.cpp
	$(CC) $(CCFLAGS) $(OPT) -c $^ -o $@ $(INCLUDE)

checker: checker.o
	$(CC) $(CCFLAGS) $(OPT) -o $@ $^ $(INCLUDE) $(LDFLAG)

