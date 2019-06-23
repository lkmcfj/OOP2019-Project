#include "../computational_graph.h"
#include <iostream>
using namespace computational_graph;
using namespace std;
int main()
{
	FileReader in("mnist_test.cg");
    const int n=50000,len=784,K=10;
    for(int i=0;i<n;++i)
    {
		const_pData t=load_data(in);
		auto shape=to_Tensor(t)->get_shape();
		for(int i:shape) cout<<i<<' ';
		cout<<endl;
		t=load_data(in);
		shape=to_Tensor(t)->get_shape();
		for(int i:shape) cout<<i<<' ';
		cout<<endl;
	}
}