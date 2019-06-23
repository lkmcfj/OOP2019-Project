cc = g++
deps = data.h dataop.h floatfunc.h graph.h message.h node.h parser.h fileop.h computational_graph.h
obj = data.o dataop.o floatfunc.o graph.o message.o node.o parser.o fileop.o

OJ : main1 main2 main3

main1 : oj_main.cpp $(obj) $(deps)
	$(cc) oj_main.cpp $(obj) -o main1 -std=c++14 -O2

main2 : oj_main.cpp $(obj) $(deps)
	$(cc) oj_main.cpp $(obj) -o main2 -std=c++14 -O2

main3 : main3.cpp $(obj) $(deps)
	$(cc) main3.cpp $(obj) -o main3 -std=c++14 -O2

data.o : data.cpp $(deps)
	$(cc) data.cpp -o data.o -c -std=c++14 -O2

dataop.o : dataop.cpp $(deps)
	$(cc) dataop.cpp -o dataop.o -c -std=c++14 -O2

floatfunc.o : floatfunc.cpp $(deps)
	$(cc) floatfunc.cpp -o floatfunc.o -c -std=c++14 -O2

graph.o : graph.cpp $(deps)
	$(cc) graph.cpp -o graph.o -c -std=c++14 -O2

message.o : message.cpp $(deps)
	$(cc) message.cpp -o message.o -c -std=c++14 -O2

node.o : node.cpp $(deps)
	$(cc) node.cpp -o node.o -c -std=c++14 -O2

parser.o : parser.cpp $(deps)
	$(cc) parser.cpp -o parser.o -c -std=c++14 -O2
	
fileop.o : fileop.cpp $(deps)
	$(cc) fileop.cpp -o fileop.o -c -std=c++14 -O2

clean :
	rm -rf $(obj) test main1 main2 main3

test_sess : example/test_sess.cpp $(obj) $(deps)
	$(cc) example/test_sess.cpp $(obj) -o example/test -std=c++14 -O2

test_tensor : example/test_tensor.cpp $(obj) $(deps)
	$(cc) example/test_tensor.cpp $(obj) -o example/test -std=c++14 -O2

leastsqr : example/test_leastsqr.cpp $(obj) $(deps)
	$(cc) example/test_leastsqr.cpp $(obj) -o example/test -std=c++14 -O2