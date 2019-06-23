#include "../computational_graph.h"
#include <vector>
#include <iostream>
using namespace computational_graph;
const int n=10000;
int getmax(const_pData x)
{
    using std::vector;
    vector<double> v=to_Tensor(x)->get_val();
    int ret=0;
    for(int i=1;i<v.size();++i) if(v[i]>v[ret]) ret=i;
    return ret;
}
int main()
{
    using std::vector;
    using std::pair;
    using std::make_pair;
    FileReader test_data("mnist_test.cg");
    FileReader network("net.cg");
    Session s(Session::load(network));
    int correct=0;
    using std::cout;
    for(int i=0;i<n;++i)
    {
        const_pData cur=load_data(test_data);
        int ans;
        test_data.read(ans);
        const_pData res=s.eval(11,{{0,cur}});
        int resid=getmax(res);
        cout<<"Test data#"<<i<<": ";
        if(ans==resid) 
        {
            cout<<"Correct\n";
            ++correct;
        } else cout<<"Wrong\n";
    }
    cout<<correct<<'/'<<n<<'\n';
}
