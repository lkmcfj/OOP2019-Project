cc = g++
prom = main
deps = data.h dataop.h floatfunc.h graph.h message.h node.h parser.h computational_graph.h
obj = data.o dataop.o floatfunc.o graph.o message.o node.o parser.o

$(prom) : oj_main.cpp $(obj)
	$(cc) oj_main.cpp $(obj) -o $(prom) -std=c++14

%.o : $.cpp $(deps)
	$(cc) -c $< -o $@ -std=c++14

clean :
	rm -rf $(obj) $(prom)