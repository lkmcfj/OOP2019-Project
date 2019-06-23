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
void test_eval(Session &s,const_pNode x,std::map<const_pNode,const_pData> p)
{
    try
    {
        cout<<*s.eval(x,p)<<endl;
    } catch(std::runtime_error e)
    {} catch(std::invalid_argument e)
    {}
}
int main()
{
    pGraph g=Graph::create();
    Session s(g);
    vector<double> x_list{1,2.3,3,5}, y_list{2,4.5,6.2,9.95};
    vector<int> shape{4};
    auto X=Constant::create(g,Tensor::create(x_list,shape)),
         Y=Constant::create(g,Tensor::create(y_list,shape));
    auto k=Variable::create(g,pf(0)),
         b=Variable::create(g,pf(0));
    auto D=Y-(k*X+b),
         loss=Reduce::create(D*D,"avg");
    auto grad=Grad::create(loss),
         pk=At::create(grad,k),
         pb=At::create(grad,b);
    auto eta=Constant::create(g,pf(0.004));
    auto optimizer=Bind::create(Assign::create(k,k-eta*pk),Assign::create(b,b-eta*pb));
    const int epoches=4000;
    for(int i=0;i<epoches;++i) s.eval(optimizer,{});
    test_eval(s,k,{});
    test_eval(s,b,{});
    test_eval(s,Reduce::create(D*X),{});
    test_eval(s,Reduce::create(D),{});
}
