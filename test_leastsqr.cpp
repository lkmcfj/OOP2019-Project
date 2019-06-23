#include "computational_graph.h"
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <string>
using namespace computational_graph;
using namespace std;


//Using gradient descend to optimize f(x) = /sum_i=1^n k_i*x_i + b
int m,n; //n : dim of function. m: numbers of input points
int times = 100 ;
double ETA = 0.04; //learning rate
void test_eval(Session &s,const_pNode x,map<const_pNode,const_pData> p)
{
    try
    {
        cout<<*s.eval(x,p)<<endl;
    } catch(std::runtime_error e)
    {} catch(std::invalid_argument e)
    {}
}

int main(){
    pGraph g=Graph::create();
    Session s(g);
	cin >> m >> n;
	vector<double> x_val;
	vector<double> y_val;
	for (int i = 0; i < m; i++){
		double x, y;
		for (int j = 0; j < n; j++){
			cin >> x;
			x_val.push_back(x);
		}
		cin >> y;
		y_val.push_back(y);
		x_val.push_back(1);
	}
	//optimize question can be transformed into minimize ||Ax-b||
	auto A = Constant::create(g, Tensor::create(x_val, 	vector<int>{m, n+1}));
	auto x = Variable::create(g, Tensor::create(vector<double>(n+1, 1.0); vector<int>{n+1, 1}));
	auto b = Constant::create(g, Tensor::create(y_val, vector<int>{m, 1}));
	auto eta = Constant::create(g, Float::create(ETA));

	//Loss function : L = ||Ax-b||^2 , 
	auto D = A*x-b;
	auto loss = Reduce::create(D*D, "avg");
	auto grad = Grad::create(loss);
	auto delta = At::create(grad, x);
	auto opti  = Assign::create(x, x-eta*delta); 
	for (int i = 1; i <= times; i++){
		cout << endl << "Optimization by gradient descend of time " << i << " : " << endl ;
		test_eval(s, opti, {});
	}
}