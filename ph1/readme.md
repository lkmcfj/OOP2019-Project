# OOP-project: 计算图

这是一个实现了简易计算图的库。

详细文档可以在```code_doc.md/pdf```中查看。

```example_test.cpp```给出了一组测试，并示范了这个库的推荐使用方式。你可以使用```make test```编译它。  

```oj_main.cpp```使用此库按照OJ要求的输入输出格式进行测试。你可以使用```make```编译它。

我们实现的额外功能在```bonus.md/pdf```中进行说明。

## 编译环境

使用支持C++14标准的编译器，并加入```-std=c++14```进行编译。

作为一个参考，我们的开发环境为:

MinGW-W64 gcc version 8.1.0

Microsoft Windows [版本 10.0.17134.706]

## 运行

要使用此库，需包含```computational_graph.h```，并与```graph.o,node.o,data.o,parser.o,message.o```连接