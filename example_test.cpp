#include "computational_graph.h"
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#define pf(x) Float::create(x)
using namespace computational_graph;
using std::cout;
using std::string;
using std::endl;
const string errp="ERROR: Placeholder missing\n",err0="ERROR: Division by zero\n",errlog="ERROR: LOG operator's input must be positive\n";
void test_eval(Session &s,const_pNode x,std::map<const_pNode,const_pData> p)
{
    try
    {
        cout<<*s.eval(x,p)<<endl;
    } catch(std::runtime_error e)
    {} catch(std::invalid_argument e)
    {}
}
void example1()
{
    cout<<"\nEx1:\n"
    "Expected output:\n"
    "1.0000\n3.0000\n-2.0000\n"+errp+err0+"---------------\n";
    pGraph g=Graph::create();
    Session s(g);
    auto x=Placeholder::create(g),
         y=Placeholder::create(g),
         z=Constant::create(g,Float::create(3));
    auto a=x+y,
         b=a-z,
         c=b*a,
         res=z/c;
    test_eval(s,x,{{x,pf(1)} });

    test_eval(s,a,{{x,pf(1)}, {y,pf(2)} });

    test_eval(s,b,{{x,pf(1)}, {y,pf(0)} });

    test_eval(s,c,{{y,pf(1)} });

    test_eval(s,res,{{x,pf(-1)}, {y,pf(1)} });
}

void example2()
{
    cout<<"\nEx2:\n"
    "Expected output:\n"
    "0.6126\n"+errlog+"2.7183\n0.9381\n---------------\n";
    pGraph g=Graph::create();
    Session s(g);
    auto x=Placeholder::create(g),
         y=Placeholder::create(g),
         z=Constant::create(g,Float::create(3));
    
    auto a=sin_node(z),
         b=log_node(y),
         c=exp_node(x),
         d=sigmoid_node(c),
         e=tanh_node(d),
         t=a+b,
         res=t*e;
    
    test_eval(s,res,{{x,pf(1)}, {y,pf(2)} });
    
    test_eval(s,res,{{x,pf(1)}, {y,pf(-1)} });
    
    test_eval(s,c,{{x,pf(1)}, {y,pf(2)} });
    
    test_eval(s,d,{{x,pf(1)}, {y,pf(1)} });
}

void example3()
{
    cout<<"\nEx3: calculate 1/sqrt(c) using Newton's method\n"
    "Expected output:\n"
    "0.7071\n1.4142\n0.0312\n0.0039\n--------------\n";
    pGraph g=Graph::create();
    Session s(g);
    auto x0=Constant::create(g,pf(0.000001)),
         c=Variable::create(g,pf(2)),
         half=Constant::create(g,pf(0.5)),
         one=Constant::create(g,pf(1));
    const_pNode x=x0;
    const int iteration_times=100;
    for(int i=0;i<iteration_times;++i) x=x+half*x*(one-c*x*x);
    const_pNode res=x;
    
    test_eval(s,res,{});
    
    s.set_variable(c,pf(0.5));
    test_eval(s,res,{});
    
    s.set_variable(c,pf(1024));
    test_eval(s,res,{});
    
    s.set_variable(c,pf(65535));
    test_eval(s,res,{});
}

int main()
{
    Message::set_log_level(1);
    std::ofstream logfile("log.txt");
    Message::set_log_stream(logfile);
    //Message::set_message_stream(cout);
    example1();
    example2();
    example3();
}
