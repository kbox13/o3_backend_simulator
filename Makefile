CXXFLAGS := -g -Wall -std=c++0x -lm
#CXXFLAGS := -g -Wall -lm
CXX=g++
SRC=procsim.cpp procsim_driver.cpp processor.cpp functional_unit.cpp dispatch_unit.cpp register_file.cpp schedule_queue.cpp update_state.cpp rob.cpp
PROCSIM=./procsim
R=2
J=3
K=2
L=1
F=4
TRACE=mcf

build:
	$(CXX) $(CXXFLAGS) $(SRC) -o procsim

run:
	$(PROCSIM) -r$R -f$F -j$J -k$K -l$L < traces/$(TRACE).100k.trace > results/$(TRACE).output

run-debug:
	$(PROCSIM) -d -r$R -f$F -j$J -k$K -l$L < traces/$(TRACE).100k.trace > debug.out

run-log:
	$(PROCSIM) -v -r$R -f$F -j$J -k$K -l$L < traces/$(TRACE).100k.trace > log.out

run-output:
	$(PROCSIM) -o -r$R -f$F -j$J -k$K -l$L < traces/$(TRACE).100k.trace > results/$(TRACE).output

clean:
	rm -f procsim *.o
