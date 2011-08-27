.PHONY: all

OBJ = cmdlib.o hash.o pr_comp.o pr_lex.o qcc.o

all: qccx

qccx: $(OBJ)
	$(CXX) $+ -O -o $@

clean:
	rm *.o qccx ../qccx

.cpp.o:
	$(CXX) $< -c -O -o $@
