POPULATION_SIZE=1000
AGE_THRESHOLD=0.15
ALPHA_THRESHOLD=0.6
REITER_THRESEHOLD=5

solver: main.cpp Solver.cpp FixedSet.cpp PermUtils.cpp
	clang++ -O2 -std=c++1y -o solver *.cpp

solver_debug: main.cpp Solver.cpp FixedSet.cpp PermUtils.cpp
	clang++ -g -std=c++1y -o solver_debug *.cpp

run40: solver
	cat wt40.txt | ./solver 40 125 $(POPULATION_SIZE) $(AGE_THRESHOLD) $(ALPHA_THRESHOLD) $(REITER_THRESEHOLD)

valgrind40: solver_debug 
	cat wt40.txt | valgrind ./solver_debug 40 125 $(POPULATION_SIZE) $(AGE_THRESHOLD) $(ALPHA_THRESHOLD) $(REITER_THRESEHOLD)
