#include "node.h"
#include "graph.h"
#include "message.h"
#include "data.h"
#include <vector>
#include <memory>
#include <string>
#include <utility>

namespace computational_graph
{
    using std::vector;
    using std::string;
    using std::map;
    using std::to_string;
    using std::unique_ptr;
    
    bool check_triple(const_pNode n1,const_pNode n2,const_pNode n3)
    {
        if((!n1)||(!n2)||(!n3))
        {
            Message::error("previous node is null when creating node, returning null node.");
            return false;
        }
        if(n1->get_id()<0||n2->get_id()<0||n3->get_id()<0)
        {
            Message::error("previous node has not been assigned with a valid ID when creating new node, returning null node.");
            return false;
        }
        if(n1->get_graph()!=n2->get_graph()||n2->get_graph()!=n3->get_graph())
        {
            Message::error("trying to connect nodes in different graph, returning null node.");
            return false;
        }
        return true;
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
    
    const vector<int>& Node::get_father() const
    {
        return father;
    }
    int Node::get_id() const
    {
        return id;
    }
    void Node::give_id(int newid)
    {
        if(id==-1) id=newid;else Message::warning("trying to modify ID for node #"+to_string(id)+", failed.");
    }
    Graph* Node::get_graph() const
    {
        return g;
    }
    int Node::get_type() const
    {
        return 0;
    }
    Node::Node(Graph *_g):g(_g),id(-1){}
    Node::Node(Graph *_g,vector<int> _father): g(_g), id(-1), father(std::move(_father)) {}
    const_pData Node::run(Session *sess,vector<const_pData> father_value) const
    {
        Message::error("raw node #"+std::to_string(id)+" can't be evaluated. returning nullptr.");
        return nullptr;
    }

    Variable::Variable(Graph *_g,const_pData default_v):
        Node(_g), default_value(default_v->copy())
    {}
    const_pNode Variable::create(Graph *g,const_pData default_v)
    {
        Message::debug("Variable::create() called");
        return g->join(unique_ptr<Variable>(new Variable(g,default_v)));
    }
    int Variable::get_type() const
    {
        return 1;
    }
    const_pData Variable::get_default_value() const
    {
        return default_value;
    }
    const_pData Variable::run(Session *sess,vector<const_pData> father_value) const
    {
        Message::warning("trying to evaluate variable #"+to_string(get_id())+" directly. returning default value");
        return default_value;
    }

    Placeholder::Placeholder(Graph *_g):Node(_g){}
    const_pNode Placeholder::create(Graph *g)
    {
        Message::debug("Placeholder::create() called");
        return g->join(unique_ptr<Placeholder>(new Placeholder(g)));
    }
    int Placeholder::get_type() const
    {
        return 2;
    }
    const_pData Placeholder::run(Session *sess,vector<const_pData> father_value) const
    {
        Message::error("trying to evaluate placeholder #"+to_string(get_id())+" directly. returning nullptr");
        return nullptr;
    }

    Constant::Constant(Graph *_g,const_pData v):
        Node(_g),value(v->copy())
    {}
    const_pNode Constant::create(Graph *g,const_pData v)
    {
        Message::debug("Constant::create() called");
        return g->join(unique_ptr<Constant>(new Constant(g,v)));
    }
    int Constant::get_type() const
    {
        return 3;
    }
    const_pData Constant::run(Session *sess,vector<const_pData> father_value) const
    {
        return value;
    }
    
    map<string,arith_op> Arith::str2op{{"+",plus},{"-",minus},{"*",multi},{"/",div}};
    Arith::Arith(Graph *_g,int left_id,int right_id,string op_str):
        Node(_g,vector<int>{left_id,right_id})
    {
        auto it=str2op.find(op_str);
        if(it!=str2op.end()) op=it->second;
        else {
            Message::error("binary operator is not +, -, *, /, set to + by default");
            op=str2op["+"];
        }
    }
    const_pNode Arith::create(const_pNode left,const_pNode right,string op_str)
    {
        Message::debug("Arith::create() called");
        if(!check_binary(left,right)) return nullptr;
        Graph *g=left->get_graph();
        return g->join(unique_ptr<Arith>(new Arith(g,left->get_id(),right->get_id(),op_str)));
    }
    int Arith::get_type() const
    {
        return 4;
    }
    const_pData Arith::run(Session *sess,vector<const_pData> father_value) const
    {
        if(father_value.size()!=2)
        {
            Message::error("evaluating node #"+to_string(get_id())+", expecting 2 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return op(father_value[0],father_value[1]);
    }

    map<string,single_op> Single_op::str2op{{"sin",sin},{"log",log},{"exp",exp},{"tanh",tanh},{"sigmoid",sigmoid},{"SIN",sin},{"LOG",log},{"EXP",exp},{"TANH",tanh},{"SIGMOID",sigmoid}};
    Single_op::Single_op(Graph *_g,int x_id,string op_str):
        Node(_g,vector<int>{x_id})
    {
        auto it=str2op.find(op_str);
        if(it!=str2op.end()) op=it->second;
        else {
            Message::error("single operator is not sin, log, exp, tanh, sigmoid, set to sin by default");
            op=str2op["sin"];
        }
    }
    const_pNode Single_op::create(const_pNode x,string op_str)
    {
        Message::debug("Single_op::create() called");
        if(!check_single(x)) return nullptr;
        Graph *g=x->get_graph();
        return g->join(unique_ptr<Single_op>(new Single_op(g,x->get_id(),op_str)));
    }
    int Single_op::get_type() const
    {
        return 5;
    }
    const_pData Single_op::run(Session *sess,vector<const_pData> father_value) const
    {
        if(father_value.size()!=1)
        {
            Message::error("evaluating node #"+to_string(get_id())+", expecting 1 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return op(father_value[0]);
    }

    Print::Print(Graph *_g,int x_id,string x_symbol):
        Node(_g,vector<int>{x_id}),father_symbol(x_symbol)
    {}
    const_pNode Print::create(Graph *g,int x_id,string x_symbol)
    {
        Message::debug("Print::create() (ID ver) called");
        return g->join(unique_ptr<Print>(new Print(g,x_id,x_symbol)));
    }
    const_pNode Print::create(const_pNode x,string x_symbol)
    {
        Message::debug("Print::create() (const_pNode ver) called");
        if(!check_single(x)) return nullptr;
        return create(x->get_graph(),x->get_id(),x_symbol);
    }
    int Print::get_type() const
    {
        return 6;
    }
    const_pData Print::run(Session *sess,vector<const_pData> father_value) const
    {
        if(father_value.size()!=1)
        {
            Message::error("evaluating node #"+to_string(get_id())+", expecting 1 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        Message::message("Print Operator: "+father_symbol+" = "+father_value[0]->to_string());
        return father_value[0];
    }

    map<string,cmp_op> Cmp::str2op{{"<",less_float},{">",greater_float},{"<=",leq_float},{">=",geq_float},{"==",equal_float}};
    Cmp::Cmp(Graph *_g,int left_id,int right_id,string op_str):
        Node(_g,vector<int>{left_id,right_id})
    {
        auto it=str2op.find(op_str);
        if(it!=str2op.end()) op=it->second;
        else {
            Message::error("comparing operator is not <, >, <=, >=, ==, set to < by default");
            op=str2op["<"];
        }
    }
    const_pNode Cmp::create(const_pNode left,const_pNode right,string op_str)
    {
        Message::debug("Cmp::create() called");
        if(!check_binary(left,right)) return nullptr;
        Graph *g=left->get_graph();
        return g->join(unique_ptr<Cmp>(new Cmp(g,left->get_id(),right->get_id(),op_str)));
    }
    int Cmp::get_type() const
    {
        return 7;
    }
    const_pData Cmp::run(Session *sess,vector<const_pData> father_value) const
    {
        if(father_value.size()!=2)
        {
            Message::error("evaluating node #"+to_string(get_id())+", expecting 2 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return op(father_value[0],father_value[1]);
    }

    Cond::Cond(Graph *_g,int cond_id,int true_id,int false_id):
        Node(_g,vector<int>{cond_id,true_id,false_id})
    {}
    const_pNode Cond::create(Graph *g,int cond_id,int true_id,int false_id)
    {
        Message::debug("Cond::create() (ID ver) called");
        return g->join(unique_ptr<Cond>(new Cond(g,cond_id,true_id,false_id)));
    }
    const_pNode Cond::create(const_pNode cond_node,const_pNode true_node,const_pNode false_node)
    {
        Message::debug("Cond::create() (const_pNode ver) called");
        if(!check_triple(cond_node,true_node,false_node)) return nullptr;
        return create(cond_node->get_graph(),cond_node->get_id(),true_node->get_id(),false_node->get_id());
    }
    int Cond::get_type() const
    {
        return 8;
    }
    const_pData Cond::run(Session *sess,vector<const_pData> father_value) const
    {
        if(father_value.size()!=3)
        {
            Message::error("evaluating node #"+to_string(get_id())+", expecting 3 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return (father_value[0]->boolean())?father_value[1]:father_value[2];
    }
    Assert::Assert(Graph *_g, int x_id): Node(_g, vector<int>{x_id}){}
    const_pNode Assert::create(Graph *g,int x_id)
    {
        Message::debug("Assert::create() (ID ver) called");
        return g->join(unique_ptr<Assert>(new Assert(g, x_id)));
    }
    const_pNode Assert::create(const_pNode x)
    {
        Message::debug("Assert::create() (const_pNode ver) called");
        if(!check_single(x)) return nullptr;
        return create(x->get_graph(), x->get_id());
    }
    int Assert::get_type() const
    {
        return 9;
    }
    const_pData Assert::run(Session *sess,std::vector<const_pData> father_value) const
    {
        if(father_value.size() != 1)
        {
            Message::error("evaluating node #"+to_string(get_id())+", expecting 1 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        if (father_value[0]->boolean())
            return std::make_shared<const Float>(0);
        else
        {
            Message::message("ERROR: Assertion failed");
            throw std::runtime_error("Assertion failed");
        }
    }

    Bind::Bind(Graph *_g, int left_id, int right_id): Node(_g, vector<int>{left_id, right_id}) {}
    const_pNode Bind::create(Graph *g,int left_id, int right_id)
    {
        Message::debug("Bind::create() (ID ver) called");
        return g->join(unique_ptr<Bind>(new Bind(g, left_id, right_id)));
    }
    const_pNode Bind::create(const_pNode left, const_pNode right)
    {
        Message::debug("Bind::create() (const_pNode ver) called");
        if(!check_binary(left, right)) return nullptr;
        return create(left->get_graph(), left->get_id(), right->get_id());
    }
    int Bind::get_type() const
    {
        return 10;
    }
    const_pData Bind::run(Session *sess,std::vector<const_pData> father_value) const
    {
        if(father_value.size() != 2)
        {
            Message::error("evaluating node #"+to_string(get_id())+", expecting 2 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        return father_value[0];
    }
    
    Assign::Assign(Graph *_g, int left_id, int right_id):Node(_g, vector<int>{left_id, right_id}) {}
	const_pNode Assign::create(Graph *g, int left_id,int right_id)
	{
		Message::debug ("Assign::create() (ID ver) called");
		return g->join(unique_ptr<Assign>(new Assign(g,left_id,right_id)));
	} 
	const_pNode Assign::create(const_pNode left, const_pNode right)
	{
		Message::debug("Assign::create() (const_pNode ver) called");
		if(!check_binary(left, right)) return nullptr;
		return create(left->get_graph(),left->get_id(),right->get_id());
	}
	int Assign::get_type() const
	{
		return 11;
	}
	const_pData Assign::run(Session *sess, std::vector<const_pData> father_value) const
	{
		if(father_value.size()!=2)
		{
			Message::error("evaluating node #"+to_string(get_id())+", expecting 2 input value,get "+to_string(father_value.size())+". returning nullptr.");
		}
		sess->assign_task(father[0], father_value[1]);
		return father_value[1];
	}

    const_pNode operator +(const_pNode left,const_pNode right)
    {
        return Arith::create(left,right,"+");
    }
    const_pNode operator -(const_pNode left,const_pNode right)
    {
        return Arith::create(left,right,"-");
    }
    const_pNode operator *(const_pNode left,const_pNode right)
    {
        return Arith::create(left,right,"*");
    }
    const_pNode operator /(const_pNode left,const_pNode right)
    {
        return Arith::create(left,right,"/");
    }
    const_pNode sin_node(const_pNode x)
    {
        return Single_op::create(x,"sin");
    }
    const_pNode log_node(const_pNode x)
    {
        return Single_op::create(x,"log");
    }
    const_pNode tanh_node(const_pNode x)
    {
        return Single_op::create(x,"tanh");
    }
    const_pNode exp_node(const_pNode x)
    {
        return Single_op::create(x,"exp");
    }
    const_pNode sigmoid_node(const_pNode x)
    {
        return Single_op::create(x,"sigmoid");
    }
    const_pNode operator <(const_pNode left,const_pNode right)
    {
        return Cmp::create(left,right,"<");
    }
    const_pNode operator >(const_pNode left,const_pNode right)
    {
        return Cmp::create(left,right,">");
    }
    const_pNode operator <=(const_pNode left,const_pNode right)
    {
        return Cmp::create(left,right,"<=");
    }
    const_pNode operator >=(const_pNode left,const_pNode right)
    {
        return Cmp::create(left,right,">=");
    }
    const_pNode operator ==(const_pNode left,const_pNode right)
    {
        return Cmp::create(left,right,"==");
    }

}
