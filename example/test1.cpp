#include "../computational_graph.h"
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
    auto x=Variable::create(g,pf(0.000001)),
         c=Variable::create(g,pf(2)),
         half=Constant::create(g,pf(0.5)),
         one=Constant::create(g,pf(1)),
         newx=x+half*x*(one-c*x*x),
         optimizer=Assign::create(x,newx);
    const int epoches=50;
    for(int i=0;i<epoches;++i) s.eval(optimizer,{});
	test_eval(s,x,{});
	
    s.set_variable(c,pf(0.5));
    s.set_variable(x,pf(0.000001));
    for(int i=0;i<epoches;++i) s.eval(optimizer,{});
    test_eval(s,x,{});
    
    s.set_variable(c,pf(1024));
    s.set_variable(x,pf(0.000001));
    for(int i=0;i<epoches;++i) s.eval(optimizer,{});
    test_eval(s,x,{});
    
    s.set_variable(c,pf(65535));
    s.set_variable(x,pf(0.000001));
    for(int i=0;i<epoches;++i) s.eval(optimizer,{});
    test_eval(s,x,{});
}

void example4()
{
    cout<<
    "Ex4: Newton's method for polynomial\n"
    "f(x)=5x^9+2x^8+5x^6+2x^5-4x^4+9x^3+x^2+5x+3\n";
    constexpr int n=9;
    double a[n+1]={3,5,1,9,-4,2,5,0,2,5};
    pGraph g=Graph::create();
    Session s(g);
    auto x=Variable::create(g,pf(1));
    auto f=Constant::create(g,pf(a[n]));
    for(int i=n-1;i>=0;--i) 
        f=f*x+Constant::create(g,pf(a[i]));
    auto grad=Grad::create(f),fpx=At::create(grad,x);
    auto optimizer=Assign::create(x,x-f/fpx);
    const int epoches=50;
    for(int i=0;i<epoches;++i) s.eval(optimizer,{});
    test_eval(s,x,{});
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
    example4();
}
