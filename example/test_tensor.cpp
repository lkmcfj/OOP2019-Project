#include "../computational_graph.h"
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

//testing: broadcast
void example1()
{
	cout << "\nEx001: Testing broadcast of Tensor:\n"
	"\nEx01: Testing broadcast +-*/ of Tensor:\n";
    vector<int> s1={1,3}, s2={2,1};
    vector<double> v1={1,2,-1}, v2={1,-2};
    auto t1=tf(v1,s1)+tf(v2,s2),
    	 t2=tf(v1,s1)-tf(v2,s2),
    	 t3=tf(v1,s1)*tf(v2,s2),
    	 t4=tf(v1,s1)/tf(v2,s2);
	cout << *t1 << endl;
    cout << *t2 << endl;
    cout << *t3 << endl;
    cout << *t4 << endl;
    
    cout << "\nEx02: Testing multiplication of Matrix:\n";
    vector<int> s3={1,1,2}, s4={1,2,2};
	vector<double> v3={1,-1}, v4={1,2,-2,-1};
    auto t5=tf(v3,s3)*tf(v4,s4);
    cout << *t5 << endl;
	/*
	vector<int> s5={2,5}, s6={3,4};
    vector<double> v5={1,2,3,4,5,6,7,8,9,10},
    			   v6={1,2,3,4,5,6,7,8,9,10,11,12};
    auto t6=tf(v5,s5)*tf(v6,s6);
    */
	//不匹配，不能broadcast，报错 
}

//testing: reshape and concat
void example2()
{
	cout << "\nEx002: Testing reshape and concat of a tensor:\n"
	"\nEx01: Testing reshape:\n";
	vector<double> init_v = {3,4,5,2,5,3,-2,3,5,10,2,5};
	vector<int>  init_s = {2,6},
			     new_s1 = {3,2,2},
			     new_s2 = {2,4,2}; //fail to reshape
	auto x = tf(init_v, init_s);
	x->reshape(new_s1);
	cout << *x << endl;
	x->reshape(new_s2); //fail to reshape
	
	cout << "\nEx02: Testing concat:\n";
	vector<int> s1 = {1,3}, s2 = {1,3};
	vector<double> v1 = {1,2,3}, v2 = {4,5,6};
	cout << *concat(tf(v1,s1), tf(v2,s2), 0) << endl;
	cout << *concat(tf(v1,s1), tf(v2,s2), 1) << endl;
}

//testing single_op
void example3()
{
	cout << "\nEx003:Testing single_op derivation of a Tensor:\n";
	vector<int> sh[3] = {{1,3},{1,2},{2,2}};
	vector<double> v[3] = {{1,2,3},{1,2},{1,2,3,4}};
	pGraph g = Graph::create();
	Session s(g);
	auto x = Placeholder::create(g);
	auto a1 = Single_op::create(x, "sin"),
		 a2 = Single_op::create(x, "exp"),
		 a3 = Single_op::create(x, "log");
	cout << *s.eval(a1,{{x,tf(v[0],sh[0])}} ) << endl;
	cout << *s.eval(a2,{{x,tf(v[1],sh[1])}} ) << endl;
	cout << *s.eval(a3,{{x,tf(v[2],sh[2])}} ) << endl;
		 
	auto b1=Grad::create(a1),
		 b2=Grad::create(a2),
		 b3=Grad::create(a3);
	cout << *s.eval(b1,{{x,tf(v[0],sh[0])}} ) << endl;
	cout << *s.eval(b2,{{x,tf(v[1],sh[1])}} ) << endl;
	cout << *s.eval(b3,{{x,tf(v[2],sh[2])}} ) << endl;
}

//testing: diff of a tensor
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
    cout<<"\nEx004: testing diff of Tensor\n";
    pGraph g=Graph::create();
    Session s(g);
    auto x=Placeholder::create(g),
         y=Placeholder::create(g);
    auto a=x+y,
         b1=Grad::create(a),
         b2=At::create(b1,x),
         c=b2*a;
    test_eval(s,b1,{{x,tf(v[4], sh[4])}, {y,tf(v[5], sh[5])}});
    test_eval(s,c,{{x,tf(v[6], sh[6])}, {y,tf(v[7], sh[7])}});
}

int main()
{
    //Message::set_log_level(1);
    //Message::set_message_stream(cout);
    example1();
    example2();
    example3();
    example4();
}
