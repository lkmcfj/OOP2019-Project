#include "../computational_graph.h"
#include <fstream>
using namespace computational_graph;
using namespace std;
int main()
{
    ifstream in("mnist_test.in");
    FileWriter out("mnist_test.cg");
    const int n=10000,len=784,K=10;
    vector<double> v;
    v.reserve(len);
    for(int i=0;i<n;++i)
    {
        v.clear();
        for(int j=0;j<len;++j)
        {
            double x;
            in>>x;
            v.push_back(x);
        }
        const_pDiff t1=Matrix::create(v,1,len);
        t1->save(out);

        int t2;
        in>>t2;
        out.write<int>(t2);
    }
}