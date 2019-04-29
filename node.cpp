#include "node.h"
#include "graph.h"
#include "message.h"
#include "datatype.h"
#include <vector>
#include <memory>
#include <string>
namespace computational_graph
{
    using std::vector;
    using std::string;
    using std::map;
    using std::to_string;
    using std::make_unique;
    const vector<int>& Node::get_father()
    {
        return father;
    }
    int Node::get_id()
    {
        return id;
    }
    void Node::give_id(int newid)
    {
        if(id==0) id=newid;else Message::warning("trying to modify ID for node #"+to_string(id)+", failed.");
    }
    Graph* Node::get_graph()
    {
        return g;
    }
    void Node::give_symbol(string symbol)
    {
        g->give_symbol(symbol,id);
    }
    int Node::get_type()
    {
        return type;
    }
    Node::Node(Graph *_g):g(_g),id(0),type(0){}
    const_pData Node::run(Session *sess,vector<const_pData> father_value)
    {
        Message::error("raw node #"+std::to_string(id)+" can't be evaluated. returning nullptr.");
        return nullptr;
    }

    Variable::Variable(Graph *_g,const_pData default_v):
        Node(_g),type(1)
    {
        default_value=default_v->copy();
    }
    const_pData Variable::get_default_value()
    {
        return default_value;
    }
    const_pData Variable::run(Session *sess,vector<const_pData> father_value)
    {
        Message::warning("trying to evaluate variable #"+to_string(id)+" directly. returning default value");
        return default_value;
    }

    Placeholder::Placeholder(Graph *_g):Node(_g),type(2){}
    const_pData Placeholder::run(Session *sess,vector<const_pData> father_value)
    {
        Message::error("trying to evaluate placeholder #"+to_string(id)+" directly. returning nullptr");
        return nullptr;
    }

    Constant::Constant(Graph *_g,const_pData v):
        Node(_g),type(3),value(v.copy())
    {}
    const_pData Constant::run(Session *sess,vector<const_pData> father_value)
    {
        return value;
    }
    
    map<string,binary_op> Arith::str2op{{"+",plus},{"-",minus},{"*",multi},{"/",div}};
    Arith::Arith(Graph *_g,int left_id,int right_id,string op_str):
        Node(_g),father{left_id,right_id},type(4)
    {
        auto it=str2op.find(op_str);
        if(it!=str2op.end()) op=it->second;
        else {
            Message::error("binary operator is not +, -, *, /, set to + by default");
            op=str2op["+"];
        }
    }
    const_pData Arith::run(Session *sess,vector<const_pData> father_value)
    {
        if(father_value.size()!=2)
        {
            Message::error("evaluating node #"+to_string(id)+", expecting 2 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return op(*father_value[0],*father_value[1]);
    }

    map<string,single_op> Single_op::str2op{{"sin",sin},{"log",log},{"exp",exp},{"tanh",tanh},{"sigmoid",sigmoid}};
    Single_op::Single_op(Graph *_g,int x_id,string op_str):
        Node(_g),father{x_id},type(5)
    {
        auto it=str2op.find(op_str);
        if(it!=str2op.end()) op=it->second;
        else {
            Message::error("single operator is not sin, log, exp, tanh, sigmoid, set to sin by default");
            op=str2op["sin"];
        }
    }
    const_pData Single_op::run(Session *sess,vector<const_pData> father_value)
    {
        if(father_value.size()!=1)
        {
            Message::error("evaluating node #"+to_string(id)+", expecting 1 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return op(*father_value[0]);
    }

    Print::Print(Graph *_g,int x_id,string x_symbol):
        Node(_g),type(6),father{x_id},father_symbol(x_symbol)
    {}
    const_pData Print::run(Session *sess,vector<const_pData> father_value)
    {
        if(father_value.size()!=1)
        {
            Message::error("evaluating node #"+to_string(id)+", expecting 1 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        Message::message("Print Operator: "+father_symbol+"="+father_value[0]->to_string());
        return father_value[0];
    }

    map<string,cmp_op> Cmp::str2op{{"<",less_float},{">",greater_float},{"<=",leq_float},{">=",geq_float},{"==",equal_float}};
    Cmp::Cmp(Graph *_g,int left_id,int right_id,string op_str):
        Node(_g),type(7),father{left_id,right_id}
    {
        auto it=str2op.find(op_str);
        if(it!=str2op.end()) op=it->second;
        else {
            Message::error("comparing operator is not <, >, <=, >=, ==, set to < by default");
            op=str2op["<"];
        }
    }
    const_pData Cmp::run(Session *sess,vector<const_pData> father_value)
    {
        if(father_value.size()!=2)
        {
            Message::error("evaluating node #"+to_string(id)+", expecting 2 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return op(*father_value[0],*father_value[1]);
    }

    Cond::Cond(Graph *_g,int cond_id,int true_id,int false_id):
        Node(_g),type(8),father{cond_id,true_id,false_id}
    {}
    const_pData Cond::run(Session *sess,vector<const_pData> father_value)
    {
        if(father_value.size()!=3)
        {
            Message::error("evaluating node #"+to_string(id)+", expecting 3 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return (father_value[0]->boolean())?father_value[1]:father_value[2];
    }

    bool check_same_graph(const Node &n1,const Node &n2)
    {
        if(n1.get_graph()==n2.get_graph()) return true; else
        {
            Message::error("trying to connect nodes in different graph,returning nullptr");
            return false;
        }
    }
    const_pNode operator +(const Node &left,const Node &right)
    {
        if(!check_same_graph(left,right)) return nullptr;
        Graph *g=left.get_graph();
        return g->join(std::make_unique<Arith>(g,left.get_id(),right.get_id(),"+"));
    }
    const_pNode operator -(const Node &left,const Node &right)
    {
        if(!check_same_graph(left,right)) return nullptr;
        Graph *g=left.get_graph();
        return g->join(std::make_unique<Arith>(g,left.get_id(),right.get_id(),"-"));
    }
    const_pNode operator *(const Node &left,const Node &right)
    {
        if(!check_same_graph(left,right)) return nullptr;
        Graph *g=left.get_graph();
        return g->join(std::make_unique<Arith>(g,left.get_id(),right.get_id(),"*"));
    }
    const_pNode operator /(const Node &left,const Node &right)
    {
        if(!check_same_graph(left,right)) return nullptr;
        Graph *g=left.get_graph();
        return g->join(make_unique<Arith>(g,left.get_id(),right.get_id(),"/"));
    }
    const_pNode sin(const Node &x)
    {
        Graph *g=x.get_graph();
        return g->join(make_unique<Single_op>(g,x.get_id(),"sin"));
    }
    const_pNode log(const Node &x)
    {
        Graph *g=x.get_graph();
        return g->join(make_unique<Single_op>(g,x.get_id(),"log"));
    }
    const_pNode tanh(const Node &x)
    {
        Graph *g=x.get_graph();
        return g->join(make_unique<Single_op>(g,x.get_id(),"tanh"));
    }
    const_pNode exp(const Node &x)
    {
        Graph *g=x.get_graph();
        return g->join(make_unique<Single_op>(g,x.get_id(),"exp"));
    }
    const_pNode sigmoid(const Node &x)
    {
        Graph *g=x.get_graph();
        return g->join(make_unique<Single_op>(g,x.get_id(),"sigmoid"));
    }
    const_pNode operator <(const Node &left,const Node &right)
    {
        if(!check_same_graph(left,right)) return nullptr;
        Graph *g=left.get_graph();
        return g->join(make_unique<Cmp>(g,left.get_id(),right.get_id(),"<"));
    }
    const_pNode operator >(const Node &left,const Node &right)
    {
        if(!check_same_graph(left,right)) return nullptr;
        Graph *g=left.get_graph();
        return g->join(make_unique<Cmp>(g,left.get_id(),right.get_id(),">"));
    }
    const_pNode operator <=(const Node &left,const Node &right)
    {
        if(!check_same_graph(left,right)) return nullptr;
        Graph *g=left.get_graph();
        return g->join(make_unique<Cmp>(g,left.get_id(),right.get_id(),"<="));
    }
    const_pNode operator >=(const Node &left,const Node &right)
    {
        if(!check_same_graph(left,right)) return nullptr;
        Graph *g=left.get_graph();
        return g->join(make_unique<Cmp>(g,left.get_id(),right.get_id(),">="));
    }
    const_pNode operator ==(const Node &left,const Node &right)
    {
        if(!check_same_graph(left,right)) return nullptr;
        Graph *g=left.get_graph();
        return g->join(make_unique<Cmp>(g,left.get_id(),right.get_id(),"=="));
    }

}
