cc = g++
deps = data.h dataop.h floatfunc.h graph.h message.h node.h parser.h computational_graph.h
obj = data.o dataop.o floatfunc.o graph.o message.o node.o parser.o

test2 : example_test_tensor.cpp $(obj) $(deps)
	$(cc) example_test_tensor.cpp $(obj) -o test2 -std=c++14

test1 : example_test.cpp $(obj) $(deps)
	$(cc) example_test.cpp $(obj) -o test -std=c++14

debug : example_test_tensor.cpp data.cpp dataop.cpp floatfunc.cpp graph.cpp message.cpp node.cpp parser.cpp $(deps)
	$(cc) example_test_tensor.cpp data.cpp dataop.cpp floatfunc.cpp graph.cpp message.cpp node.cpp parser.cpp -o test -std=c++14 -g

data.o : data.cpp $(deps)
	$(cc) data.cpp -o data.o -c -std=c++14

dataop.o : dataop.cpp $(deps)
	$(cc) dataop.cpp -o dataop.o -c -std=c++14

floatfunc.o : floatfunc.cpp $(deps)
	$(cc) floatfunc.cpp -o floatfunc.o -c -std=c++14

graph.o : graph.cpp $(deps)
	$(cc) graph.cpp -o graph.o -c -std=c++14

message.o : message.cpp $(deps)
	$(cc) message.cpp -o message.o -c -std=c++14

node.o : node.cpp $(deps)
	$(cc) node.cpp -o node.o -c -std=c++14

parser.o : parser.cpp $(deps)
	$(cc) parser.cpp -o parser.o -c -std=c++14

clean :
	rm -rf $(obj) test