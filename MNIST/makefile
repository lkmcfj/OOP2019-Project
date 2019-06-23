cc = g++
obj = ../data.o ../dataop.o ../floatfunc.o ../graph.o ../message.o ../node.o ../parser.o ../fileop.o

wrapper : wrapper.cpp
	$(cc) wrapper.cpp $(obj) -o wrapper -std=c++14 -O2

loader : loader.cpp
	$(cc) loader.cpp $(obj) -o loader -std=c++14 -O2

train : train.cpp
	$(cc) train.cpp $(obj) -o train -std=c++14 -O2

test : test.cpp
	$(cc) test.cpp $(obj) -o test -std=c++14 -O2