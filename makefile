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



test2 : test2.cpp $(obj) $(deps)
	$(cc) test2.cpp $(obj) -o test -std=c++14

test1 : test1.cpp $(obj) $(deps)
	$(cc) test1.cpp $(obj) -o test -std=c++14

#debug : example_test_tensor.cpp data.cpp dataop.cpp floatfunc.cpp graph.cpp message.cpp node.cpp parser.cpp $(deps)
#	 $(cc) example_test_tensor.cpp data.cpp dataop.cpp floatfunc.cpp graph.cpp message.cpp node.cpp parser.cpp fileop.cpp -o test -std=c++14 -g

test_yj : test_yj.cpp $(obj) $(deps)
	$(cc) test_yj.cpp $(obj) -o test -std=c++14 -O2

leastsqr : leastsqr.cpp $(obj) $(deps)
	$(cc) leastsqr.cpp $(obj) -o test -std=c++14 -O2