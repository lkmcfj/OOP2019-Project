#include "computational_graph.h"
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <string>
#define tf(v,s) Tensor::create((v),(s))
using namespace computational_graph;
using namespace std;

int dim, size;
vector<double> init_v, init_s;

void test_eval(Session &s,const_pNode x,map<const_pNode,const_pData> p)
{
    try
    {
        cout<<*s.eval(x,p)<<endl;
    } catch(std::runtime_error e)
    {} catch(std::invalid_argument e)
    {}
}

/*
//input_tensor, if needed.
void init_dim()
{
	size = 1;
	cout << "Entor Dim of tensor: ";  
    cin >> dim; 
    init_s.resize(dim);
}
void input_shape()
{
  	init_s.resize(dim);
    cout << "\n Enter shape of tensor: " ;
    for (int i = 0; i < dim; i++){
    	cin >> init_s[i];
    	size *= init_s[i];
    }	
    init_v.resize(size);
}
void init_value()
{
    cout << "\n Enter value by index: ";
    for (int i = 0 ; i < size; i++)
    	cin >> init_v[i];
}
void input_tensor()
{
	input_dim();
	input_shape();
	input_value();
}
*/
//testing diff of a tensor
void example4()
{
	vector<double> v[8] = { 
							 {1,2,3,4,5,6},
							 {1,2,3,4,5,6},
							 {2,3,4,5,6,7},
							 {1,2,3,4,5,6},
							 {1,2},
							 {3,2},
							 {1,2,3,4,5,6,7,8},
							 {2,3,4,5},
					};
	vector<int> sh[8] ={ 
							 {2,3},
							 {2,3},
							 {2,3},
							 {2,3},
							 {1,2},
							 {2,1},
							 {2,2,2},
							 {2,2},
					};
    cout<<"\nEx1: testing +-*/ (with broadcast) of Tensor\n";
    pGraph g=Graph::create();
    Session s(g);
    auto x=Placeholder::create(g),
         y=Placeholder::create(g);
    auto a=x+y,
         b1=Grad::create(a),
         b2=At::create(b1,x),
         c=b2*a;
    
    test_eval(s,x,{{x,tf(v[1], sh[1])} });

    test_eval(s,a,{{x,tf(v[2], sh[2])}, {y,tf(v[3], sh[3])}});

    test_eval(s,b1,{{x,tf(v[4], sh[4])}, {y,tf(v[5], sh[5])}});

    test_eval(s,c,{{x,tf(v[6], sh[6])}, {y,tf(v[7], sh[7])}});

}

//testing: resize
void example3()
{
	cout << "\nEx3: Testing reshape of a tensor:\n";
	vector<double> init_v = {3,4,5,2,5,3,-2,3,5,10,2,5};
	vector<int>  init_s = {2,6},
			     new_s1 = {3,2,2},
			     new_s2 = {2,4,2}; //fail to reshape
	auto x = tf(init_v, init_s);
	x->reshape(new_s1);
	x->reshape(new_s2);
}

/*
//testing: sin, log, exp, sigmoid, tanh
void example2()
{
	vector<double> v[9] = { 
							 {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {}
					},
					s[9] ={ 
							 {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {}
					};
    cout<<"\n Ex2:testing sin,log,exp,sigmoid,tanh of tensor \n"
    Graph g;
    Session s(g);
    auto x=Placeholder::create(&g),
         y=Placeholder::create(&g),
         z=Constant::create(&g,tf(v[0], s[0]));
    
    auto a=sin_node(z),
         b=log_node(y),
         c=exp_node(x),
         d=sigmoid_node(c),
         e=tanh_node(d),
         t=a+b,
         res=t*e;
    
    test_eval(s,res,{{x,tf(v[1], s[1])}, {y,tf(v[2], s[2])}});
    
    test_eval(s,res,{{x,tf(v[3], s[3])}, {y,tf(v[4], s[4])}});
    
    test_eval(s,c,{{x,tf(v[5], s[5])}, {y,tf(v[6], s[6])}});
    
    test_eval(s,d,{{x,tf(v[7], s[7])}, {y,tf(v[8], s[8])}});
}

//testing : arithmetic operators of tensor
void example1()
{
	vector<double> v[10] = { {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {}
					},
					s[10] ={ {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {},
							 {}

					};
    cout<<"\nEx1: testing arithmetic operators (with broadcast) of Tensor\n"
    Graph g;
    Session s(g);
    auto x=Placeholder::create(&g),
         y=Placeholder::create(&g),
         z=Constant::create(&g,tf(v[0], s[0]));      
    auto a=x+y,
         b=a-z,
         c=b*a,
         res=z/c;
    
    test_eval(s,x,{{x,tf(v[1], s[1])} });

    test_eval(s,a,{{x,tf(v[2], s[2])}, {y,tf(v[3], s[3])}});

    test_eval(s,b,{{x,tf(v[4], s[4])}, {y,tf(v[5], s[5])}});

    test_eval(s,c,{{y,tf(v[6], s[6])}, {y,tf(v[7], s[7])}});

    test_eval(s,res,{{tf(v[8], s[8])}, {y,tf(v[9], s[9])}});
}
*/


int main()
{
    //Message::set_log_level(1);
    //Message::set_message_stream(cout);
    //example1();
    //example2();
    example3();
    example4();
}
