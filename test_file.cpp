#include "fileop.h"
#include <iostream>
using namespace std;
using namespace computational_graph;
void f1()
{
    FileWriter out("test.txt");
    int x=2;
    double y=4.3;
    char c='t';
    out.write(x);
    out.write(y);
    out.write(c);
}
void f2()
{
    FileReader in("test.txt");
    int x=in.read<int>();
    double y=in.read<double>();
    char c=in.read<char>();
    cout<<x<<endl<<y<<endl<<c;
}
int main()
{
    f1();
    getchar();
    f2();
}
