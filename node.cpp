#include "node.h"
#include "graph.h"
#include "message.h"
#include "data.h"
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
        if(id==-1) id=newid;else Message::warning("trying to modify ID for node #"+to_string(id)+", failed.");
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
        return 0;
    }
    Node::Node(Graph *_g):g(_g),id(-1){}
    const_pData Node::run(Session *sess,vector<const_pData> father_value)
    {
        Message::error("raw node #"+std::to_string(id)+" can't be evaluated. returning nullptr.");
        return nullptr;
    }

    Variable::Variable(Graph *_g,const_pData default_v):
        Node(_g)
    {
        default_value=default_v->copy();
    }
    int Variable::get_type()
    {
        return 1;
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

    Placeholder::Placeholder(Graph *_g):Node(_g){}
    int Placeholder::get_type()
    {
        return 2;
    }
    const_pData Placeholder::run(Session *sess,vector<const_pData> father_value)
    {
        Message::error("trying to evaluate placeholder #"+to_string(id)+" directly. returning nullptr");
        return nullptr;
    }

    Constant::Constant(Graph *_g,const_pData v):
        Node(_g),value(v.copy())
    {}
    int Constant::get_type()
    {
        return 3;
    }
    const_pData Constant::run(Session *sess,vector<const_pData> father_value)
    {
        return value;
    }
    
    map<string,arith_op> Arith::str2op{{"+",plus},{"-",minus},{"*",multi},{"/",div}};
    Arith::Arith(Graph *_g,int left_id,int right_id,string op_str):
        Node(_g),father{left_id,right_id}
    {
        auto it=str2op.find(op_str);
        if(it!=str2op.end()) op=it->second;
        else {
            Message::error("binary operator is not +, -, *, /, set to + by default");
            op=str2op["+"];
        }
    }
    int Arith::get_type()
    {
        return 4;
    }
    const_pData Arith::run(Session *sess,vector<const_pData> father_value)
    {
        if(father_value.size()!=2)
        {
            Message::error("evaluating node #"+to_string(id)+", expecting 2 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return op(father_value[0],father_value[1]);
    }

    map<string,single_op> Single_op::str2op{{"sin",sin},{"log",log},{"exp",exp},{"tanh",tanh},{"sigmoid",sigmoid}};
    Single_op::Single_op(Graph *_g,int x_id,string op_str):
        Node(_g),father{x_id}
    {
        auto it=str2op.find(op_str);
        if(it!=str2op.end()) op=it->second;
        else {
            Message::error("single operator is not sin, log, exp, tanh, sigmoid, set to sin by default");
            op=str2op["sin"];
        }
    }
    int Single_op::get_type()
    {
        return 5;
    }
    const_pData Single_op::run(Session *sess,vector<const_pData> father_value)
    {
        if(father_value.size()!=1)
        {
            Message::error("evaluating node #"+to_string(id)+", expecting 1 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return op(father_value[0]);
    }

    Print::Print(Graph *_g,int x_id,string x_symbol):
        Node(_g),father{x_id},father_symbol(x_symbol)
    {}
    int Print::get_type()
    {
        return 6;
    }
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
        Node(_g),father{left_id,right_id}
    {
        auto it=str2op.find(op_str);
        if(it!=str2op.end()) op=it->second;
        else {
            Message::error("comparing operator is not <, >, <=, >=, ==, set to < by default");
            op=str2op["<"];
        }
    }
    int Cmp::get_type()
    {
        return 7;
    }
    const_pData Cmp::run(Session *sess,vector<const_pData> father_value)
    {
        if(father_value.size()!=2)
        {
            Message::error("evaluating node #"+to_string(id)+", expecting 2 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return op(father_value[0],father_value[1]);
    }

    Cond::Cond(Graph *_g,int cond_id,int true_id,int false_id):
        Node(_g),father{cond_id,true_id,false_id}
    {}
    int Cond::get_type()
    {
        return 8;
    }
    const_pData Cond::run(Session *sess,vector<const_pData> father_value)
    {
        if(father_value.size()!=3)
        {
            Message::error("evaluating node #"+to_string(id)+", expecting 3 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return (father_value[0]->boolean())?father_value[1]:father_value[2];
    }

    bool check_binary(const_pNode n1,const_pNode n2)
    {
	    if((!n1)||(!n2))
        {
            Message::error("previous node is null when creating node, returning null node.");
            return false;
        }
        if(n1->get_id()<0||n2->get_id()<0)
        {
            Message::error("previous node has not been assigned with a valid ID when creating new node, returning null node.");
            return false;
        }
        if(n1->get_graph()!=n2->get_graph())
        {
            Message::error("trying to connect nodes in different graph, returning null node.");
            return false;
        }
        return true;
    }
    bool check_single(const_pNode n)
    {
        if(!n)
        {
            Message::error("previous node is null when creating node, returning null node.");
            return false;
        }
        if(n->get_id()<0)
        {
            Message::error("previous node has not been assigned with a valid ID when creating new node, returning null node.");
            return false;
        }
        return true;
    }

    const_pNode create_arith_node(const_pNode left,const_pNode right,string op_str)
    {
        if(!check_binary(left,right)) return nullptr;
        Graph *g=left->get_graph();
        return g->join(std::make_unique<Arith>(g,left->get_id(),right->get_id(),op_str));
    }
    const_pNode create_single_op_node(const_pNode x,string op_str)
    {
        if(!check_single(x)) return nullptr;
        Graph *g=x->get_graph();
        return g->join(std::make_unique<Single_op>(g,x->get_id(),op_str));
    }
    const_pNode create_cmp_node(const_pNode left,const_pNode right,string op_str)
    {
        if(!check_binary(left,right)) return nullptr;
        Graph *g=left->get_graph();
        return g->join(std::make_unique<Cmp>(g,left->get_id(),right->get_id(),op_str));
    }

    const_pNode operator +(const_pNode left,const_pNode right)
    {
        return create_arith_node(left,right,"+");
    }
    const_pNode operator -(const_pNode left,const_pNode right)
    {
        return create_arith_node(left,right,"-");
    }
    const_pNode operator *(const_pNode left,const_pNode right)
    {
        return create_arith_node(left,right,"*");
    }
    const_pNode operator /(const_pNode left,const_pNode right)
    {
        return create_arith_node(left,right,"/");
    }
    const_pNode sin(const_pNode x)
    {
        return create_single_op_node(x,"sin");
    }
    const_pNode log(const_pNode x)
    {
        return create_single_op_node(x,"log");
    }
    const_pNode tanh(const_pNode x)
    {
        return create_single_op_node(x,"tanh");
    }
    const_pNode exp(const_pNode x)
    {
        return create_single_op_node(x,"exp");
    }
    const_pNode sigmoid(const_pNode x)
    {
        return create_single_op_node(x,"sigmoid");
    }
    const_pNode operator <(const_pNode left,const_pNode right)
    {
        return create_cmp_node(left,right,"<");
    }
    const_pNode operator >(const_pNode left,const_pNode right)
    {
        return create_cmp_node(left,right,">");
    }
    const_pNode operator <=(const_pNode left,const_pNode right)
    {
        return create_cmp_node(left,right,"<=");
    }
    const_pNode operator >=(const_pNode left,const_pNode right)
    {
        return create_cmp_node(left,right,">=");
    }
    const_pNode operator ==(const_pNode left,const_pNode right)
    {
        return create_cmp_node(left,right,"==");
    }

}
