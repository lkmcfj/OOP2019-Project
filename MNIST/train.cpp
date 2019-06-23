#include "../computational_graph.h"
#include <vector>
#include <random>
std::mt19937 gen(7);
using namespace computational_graph;
using std::vector;
constexpr int mini_batch=1, epoches=2, eta=1, n=50000;
const vector<int> layers{784,100,10};
const int c=layers.size();
vector<double> rand_normal(int n,double mean,double stddev)
{
    vector<double> ret;
    ret.reserve(n);
    std::normal_distribution<double> d(mean,stddev);
    for(int i=0;i<n;++i) ret.push_back(d(gen));
    return std::move(ret);
}
int main()
{
    FileReader in("mnist_training.cg");
    pGraph g=Graph::create();
    Session s(g);

    using std::pair;
    using std::make_pair;
    using std::cerr;
    cerr<<"start loading data\n";
    vector<pair<const_pData,const_pData>> data;
    for(int i=0;i<n;++i)
    {
        const_pData l1=load_data(in);
        const_pData l2=load_data(in);
        data.push_back(make_pair(l1,l2));
    }

    cerr<<"start building neural network\n";
    const_pNode inputn=Placeholder::create(g), ansn=Placeholder::create(g);
    vector<const_pNode> w,b;
    for(int i=0;i<c-1;++i)
    {
        int l=layers[i],r=layers[i+1];
        const_pNode curw=Variable::create(g,Matrix::create(rand_normal(l*r,0,1/sqrt(l)),l,r)),
                    curb=Variable::create(g,Matrix::create(rand_normal(r,0,1),1,r));
        w.push_back(curw);
        b.push_back(curb);
    }
    vector<const_pNode> layer;
    layer.push_back(inputn);
    for(int i=1;i<c;++i) layer.push_back(sigmoid_node(layer[i-1]*w[i-1]+b[i-1]));
    const_pNode D=ansn-layer[c-1],
                t1=D*D,
                t2=Reduce::create(t1,"sum"),
                loss=Reduce::create(t2,"avg");
    const_pNode grad=Grad::create(loss);
    const_pNode learning_rate=Constant::create(g,Float::create(eta));
    vector<const_pNode> op;
    for(auto i:w)
    {
        const_pNode d=At::create(grad,i);
        op.push_back(Assign::create(i,i-learning_rate*d));
    }
    for(auto i:b)
    {
        const_pNode d=At::create(grad,i);
        op.push_back(Assign::create(i,i-learning_rate*d));
    }
    const_pNode optimizer=op[0];
    for(int i=1;i<op.size();++i) optimizer=Bind::create(optimizer,op[i]);
    cerr<<"input layer: "<<inputn->get_id()<<"\noutput layer: "<<layer[c-1]->get_id()<<"\nloss function: "<<loss->get_id()<<'\n';

    cerr<<"start packing training_data\n";
    vector<pair<const_pData,const_pData>> training_data;
    for(int i=0;i<n/mini_batch;++i)
    {
        const_pData first=data[i*mini_batch].first, second=data[i*mini_batch].second;
        for(int j=1;j<mini_batch;++j)
        {
            first=concat(first,data[i*mini_batch+j].first);
            second=concat(second,data[i*mini_batch+j].second);
        }
        training_data.push_back(make_pair(first,second));
    }
    /*
    const_pData allinput=training_data[0].first,allans=training_data[0].second;
    for(int i=1;i<training_data.size();++i)
    {
        allinput=concat(allinput,training_data[i].first);
        allans=concat(allans,training_data[i].second);
    }
    */

    /*
    cerr<<"layer1";
    s.eval(layer[1],{{inputn,training_data[0].first}});
    cerr<<"layer2";
    s.eval(layer[2],{{inputn,training_data[0].first}});
    cerr<<"loss function";
    s.eval(loss,{{inputn,training_data[0].first},{ansn,training_data[0].second}});
    */
    /*
    const_pData t2v=s.eval(t2,{{inputn,training_data[0].first},{ansn,training_data[0].second}});
    vector<int> sh=to_Tensor(t2v)->get_shape();
    for(int i:sh) cerr<<i<<' ';
    cerr<<'\n';
    */

    cerr<<"start training\n";
    using std::cout;
    using std::endl;
    int count=0;
    for(int T=0;T<epoches;++T)
    {
        cout<<"Epoch #"<<T<<'\n';
        for(auto i:training_data)
        {
            cout<<"Before: loss="<<*s.eval(loss,{{inputn,i.first},{ansn,i.second}})<<endl;
            s.eval(optimizer,{{inputn,i.first},{ansn,i.second}});
            cout<<"After: loss="<<*s.eval(loss,{{inputn,i.first},{ansn,i.second}})<<endl;
            ++count;
            if(count%500==0)
            {
                FileWriter out("net.cg");
                s.save(out);
            }
        }
    }
    FileWriter out("net.cg");
    s.save(out);
}
