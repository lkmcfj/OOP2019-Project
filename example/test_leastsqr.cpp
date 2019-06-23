#include "../computational_graph.h"
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <string>
using namespace computational_graph;
using namespace std;

//Using gradient descend to optimize f(x) = /sum_{i=1}^n k_i*x_i + b
void test_eval(Session &s,const_pNode x,map<const_pNode,const_pData> p)
{
    try
    {
        cout<<*s.eval(x,p)<<endl;
    } catch(std::runtime_error e)
    {
        Message::error(e.what());
    } catch(std::invalid_argument e)
    {
        Message::error(e.what());
    }
}

int main()
{
    int epoches;
    cin>>epoches;
    double ETA; //learning rate
    cin>>ETA;
    pGraph g=Graph::create();
    Session s(g);
    int m,n;
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
    //optimization problem can be transformed into minimize ||Ak-y||
    const_pData Aval=Matrix::create(x_val, m, n+1),
                yval=Matrix::create(y_val, m, 1);
    const_pNode A = Constant::create(g, Aval),
				//k = Variable::create(g, Matrix::create(vector<double>(n+1,1) ,n+1,1) ),    			
                k = Variable::create(g, Matrix::create(vector<double>{1,-2.6,7.5},n+1,1) ),
                y = Constant::create(g, yval);
    const_pNode eta = Constant::create(g, Float::create(ETA));

    //Loss function : L = ||Ak-y||^2 , 
    const_pNode D = A*k-y,
                loss = Reduce::create(Reduce::create(D*D, "sum"), "sum"),
                grad = Grad::create(loss),
                delta = At::create(grad, k),
                opti  = Assign::create(k, k-eta*delta); 
    for (int i = 0; i < epoches; i++){
        cout<<'#'<<i+1<<':' << endl
        	<< *s.eval(opti,{}) <<endl ;
    }
    cout << endl;
    const_pTensor t = to_Tensor((s.eval(k,{})));
    vector<double> val = t->get_val();	
    cout << endl << "f(x) = ";
    for(int i = 0; i < n; i++){
    	cout << val[i] << " * x_" << i+1 <<  " + " ;
    }
    cout << val[n] << endl << endl;
    for(int i = 0 ; i < n; i++){
    	cout << "k[" << i+1 << "]" << " = " << val[i] << endl;
    }
    cout << "b = " << val[n] << endl;

    cout << endl << "The optimization of A*k-y is :" << endl
    	 << *s.eval(D, {}) << endl;

}