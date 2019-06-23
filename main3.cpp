#include "computational_graph.h"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <utility>
#define pf(x) Float::create(x)
using namespace computational_graph;
using namespace std;
/*
void test_eval(Session &s,const_pNode x,std::map<const_pNode,const_pData> p)
{
    try
    {
        cout<<*s.eval(x,p)<<endl;
    } catch(std::runtime_error e)
    {} catch(std::invalid_argument e)
    {}
}
*/
void Newton_iteration(int n, double *a, double x0)
{
	pGraph g=Graph::create();
    Session s(g);
    auto x=Variable::create(g,pf(x0));
    auto f=Constant::create(g,pf(a[n]));
    for(int i=n-1;i>=0;--i) 
        f=f*x+Constant::create(g,pf(a[i]));
    auto grad=Grad::create(f),fpx=At::create(grad,x);
    auto optimizer=Assign::create(x,x-f/fpx);
    const int epoches=5;
    for(int i=0;i<epoches;++i) cout << *s.eval(optimizer,{}) << ' ';
    cout << endl;
}

int main()
{
	int T;
	cin >> T;
	while(T--)
	{
		int n;
		double x0;
		cin >> n;
		double *a=new double[n+1];
		for(int i=n;i>=0;i--)
			cin >> a[i];
		cin >> x0; 
		Newton_iteration(n,a,x0);
	}
	return 0;
}


