#include "../computational_graph.h"
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <string>
#define tf(v,s) Tensor::create((v),(s))
#define pf(v) Float::create(v)
using namespace computational_graph;
using namespace std;
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
    try
    {
        pGraph g=Graph::create();
        Session s1(g),s2(g);
        const_pNode x=Variable::create(g,pf(2)),
                    y=Constant::create(g,pf(1));
        const_pNode inc=Assign::create(x,x+y);
        s1.eval(inc,{});
        s2.eval(inc,{});
        s2.eval(inc,{});
        int x_id=x->get_id();
        cout<<"In session #1, variable x = "<<*s1.eval(x,{})<<"\n"
              "In session #2, variable x = "<<*s2.eval(x,{})<<"\n";
        {
            FileWriter out("test.cg");
            cout<<"Saving session #1 into file test.cg\n";
            s1.save(out);
        }
        getchar();
        {
            FileReader in("test.cg");
            cout<<"Loading a session from test.cg\n";
            Session s3(Session::load(in));
            cout<<"In session just loaded, variable x = "<<*s3.eval(x_id,{})<<"\n";
        }
    } catch(std::runtime_error e)
    {
        Message::error(e.what());
    } catch(std::invalid_argument e)
    {
        Message::error(e.what());
    }
}
    