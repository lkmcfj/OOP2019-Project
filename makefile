main : data.o graph.o message.o node.o parser.o oj_main.cpp computational_graph.h
	g++ oj_main.cpp data.o graph.o message.o node.o parser.o -o main -std=c++14
    
data.o : data.cpp data.h
	g++ -c data.cpp -o data.o -std=c++14
    
graph.o : graph.cpp graph.h node.h message.h data.h
	g++ -c graph.cpp -o graph.o -std=c++14
    
message.o : message.cpp message.h
	g++ -c message.cpp -o message.o -std=c++14

node.o : node.cpp node.h graph.h message.h data.h
	g++ -c node.cpp -o node.o -std=c++14
	
parser.o : parser.cpp parser.h graph.h node.h data.h message.h
	g++ -c parser.cpp -o parser.o -std=c++14
    
debug : data.cpp data.h graph.cpp graph.h message.cpp message.h node.cpp node.h test_main.cpp
	g++ test_main.cpp data.cpp graph.cpp message.cpp node.cpp -o main -g -std=c++14

test : data.o graph.o message.o node.o test.cpp
	g++ test.cpp data.o graph.o message.o node.o -o test -std=c++14