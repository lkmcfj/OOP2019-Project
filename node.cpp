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
    Node::Node(Graph *_g):g(_g),id(-1){}
    Node::Node(Graph *_g,vector<int> _father): g(_g), id(-1), father(std::move(_father)) {}

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
    std::vector<const_pDiff> Variable::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        return std::vector<const_pDiff>();
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
    std::vector<const_pDiff> Placeholder::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        return std::vector<const_pDiff>();
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
    std::vector<const_pDiff> Constant::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        return std::vector<const_pDiff>();
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
    std::vector<const_pDiff> Arith::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        if(father_value.size()!=2)
        {
            Message::error("evaluating diff of node #"+to_string(get_id())+", expecting 2 input value,get "+to_string(father_value.size())+". returning empty vector.");
            return std::vector<const_pDiff>();
        }
        //TODO
    }

    map<string,const SingleTensorOp&> Single_op::str2op{{"sin",SingleTensorOp::sin},{"log",SingleTensorOp::log},{"exp",SingleTensorOp::exp},{"tanh",SingleTensorOp::tanh},{"sigmoid",SingleTensorOp::sigmoid},{"SIN",SingleTensorOp::sin},{"LOG",SingleTensorOp::log},{"EXP",SingleTensorOp::exp},{"TANH",SingleTensorOp::tanh},{"SIGMOID",SingleTensorOp::sigmoid}};
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
    std::vector<const_pDiff> Single_op::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        if(father_value.size()!=1)
        {
            Message::error("evaluating diff of node #"+to_string(get_id())+", expecting 1 input value,get "+to_string(father_value.size())+". returning empty vector.");
            return std::vector<const_pDiff>();
        }
        return vector<const_pDiff>{op.diff(father_value[0])};
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
        Message::message("Print Operator: " + father_symbol+" = "+father_value[0]->to_string());
        return father_value[0];
    }
    std::vector<const_pDiff> Print::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        if(father_value.size()!=1)
        {
            Message::error("evaluating diff of node #"+to_string(get_id())+", expecting 1 input value,get "+to_string(father_value.size())+". returning empty vector.");
            return std::vector<const_pDiff>();
        }
        const_pTensor f = to_Tensor(father_value[0]);
        auto shape = f -> get_shape();
        return vector<const_pDiff>{Diff::identity(shape)} ;

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
    std::vector<const_pDiff> Cmp::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        if(father_value.size() != 2)
        {
            Message::error("evaluating diff of node #"+to_string(get_id())+", expecting 2 input value,get "+to_string(father_value.size())+". returning empty vector.");
            return std::vector<const_pDiff>();
        }
        const_pTensor f = to_Tensor(father_value[0]);
        auto shape = f -> get_shape();
        return vector<const_pDiff>{Tensor::zeros(shape)} ;
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
    std::vector<const_pDiff> Cond::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        if(father_value.size()!=3)
        {
            Message::error("evaluating diff of node #"+to_string(get_id())+", expecting 3 input value,get "+to_string(father_value.size())+". returning empty vector.");
            return std::vector<const_pDiff>();
        }
        const_pTensor f[3]; vector <int> shape[3];
        for (int i = 0; i < 3; i++){
            f[i] = to_Tensor(father_value[i]);
            shape[i] = f[i] -> get_shape();
        }
        if (father_value[0]->boolean())
            return vector<const_pDiff>{ Tensor::zeros(shape[0]), Diff::identity(shape[1]), Tensor::zeros(shape[2])};
        return vector<const_pDiff>{ Tensor::zeros(shape[0]), Tensor::zeros(shape[1]), Diff::identity(shape[2])};
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
    std::vector<const_pDiff> Assert::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        if(father_value.size() != 1)
        {
            Message::error("evaluating diff of node #"+to_string(get_id())+", expecting 1 input value,get "+to_string(father_value.size())+". returning empty vector.");
            return std::vector<const_pDiff>();
        }
        const_pTensor f = to_Tensor(father_value[0]);
        auto shape = f -> get_shape();
        return vector<const_pDiff>{Tensor::zeros(shape)} ;
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
    std::vector<const_pDiff> Bind::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        if(father_value.size()!=2)
        {
            Message::error("evaluating diff of node #"+to_string(get_id())+", expecting 2 input value,get "+to_string(father_value.size())+". returning empty vector.");
            return std::vector<const_pDiff>();
        }
        const_pTensor f = to_Tensor(father_value[0]);
        auto shape = f -> get_shape();
        return vector<const_pDiff>{Diff::(shape)} ;
    }

    Grad::Grad(Graph *_g, int x_id): Node(_g, vector<int>{x_id}) {}
    const_pNode Grad::create(Graph *g, int x_id)
    {
        Message::debug("Grad::create() (ID ver) called");
        return g->join(unique_ptr<Grad>(new Grad(g, x_id)));        
    }
    const_pNode Grad::create(const_pNode x)
    {
        Message::debug("Grad::create() (const_pNode ver) called");
        if(!check_single(x)) return nullptr;
        return create(x->get_graph(), x->get_id());       
    }   
    int Grad::get_type() const
    {
        return 11;        
    }
    const_pData Grad::run(Session *sess, std::vector<const_pData> father_value) const
    {
        if(father_value.size() != 1)
        {
            Message::error("evaluating node #"+to_string(get_id())+", expecting 1 input value,get "+to_string(father_value.size())+". returning nullptr.");
            return nullptr;
        }
        const_pTensor f=to_Tensor(father_value[0]);
        int size=f->getsize();
        vector<int> shape=f->get_shape();
        vector<double> t(size*size,0);
        for(int i=0;i<size;++i) t[i*size+i]=1;
        map<int,const_pDiff> res;
        res[father[0]]=Diff::create(t,shape+shape,shape.size());
        vector<int> &q=sess->vislist;
        for(int i=q.size()-1;i>=0;--i)
        {
            auto it=res.find(q[i]);
            const_pNode cur=sess->g.nodes[q[i]];
            if(it!=res.end())
            {
                vector<const_pData> prev;
                for(int id:cur->get_father()) prev.push_back(sess->temp_value[id]);
                vector<const_pDiff> pred=sess->g.nodes[q[i]]->run_diff(sess,prev);
                for(int j=0;j<cur->get_father().size();++j)
                {
                    int id=cur->getfather()[j];
                    if(res.find(id)!=res.end()) res[id]=res[id]+it->second*pred[j];
                    else res[id]=it->second*pred[j];
                }
            }
        }
        return Graddata::create(res,shape);
    }
    std::vector<const_pData> Grad::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        Message::error("Not Support");
        throw std::runtime_error("Not Support");
    }

    At::At(Graph *_g, int grad_id, int x_id): Node(_g, vector<int>{grad_id, x_id}) {}
    const_pNode At::create(Graph *g, int grad_id, int x_id)
    {
        Message::debug("At::create() (ID ver) called");
        return g->join(unique_ptr<At>(new At(g, grad_id, x_id)));          
    }
    const_pNode At::create(const_pNode grad, const_pNode x)
    {
        Message::debug("Grad::create() (const_pNode ver) called");
        if(!check_binary(grad, x)) return nullptr;
        return create(grad->get_graph(), grad->get_id());              
    }      
    int At::get_type() const
    {
        return 12;  
    }
    const_pData At::run(Session *sess, std::vector<const_pData> father_value) const
    {
        if(father_value.size() != 2)
        {
            Message::error("evaluating node #"+to_string(get_id())+", expecting 2 input value,get "+to_string(father_value.size())+". returning nullptr.")
            return nullptr;
        }
        if(const_pData temp = father_value[0].get_grad(father[1]))
                return temp;
        if (auto x_t = std::dynamic_pointer_cast<const Tensor>(father_value[1]))
        {
            int n  = i;
            vector<int> shape = father[0].get_fshape() + x_t.get_shape();
            dimf = shape.size();
            for(auto i : shape) n *= i;
            return create(vector<double>(n,0), shape, dimf);
        }
        throw std::runtime_error("x is not a tensor");
    }
    std::vector<const_pDiff> At::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        Message::error("Not Support");
        throw std::runtime_error("Not Support");
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
        return 13;
    }
    const_pData Assign::run(Session *sess, std::vector<const_pData> father_value) const
    {
        if(father_value.size()!=2)
        {
            Message::error("evaluating node #"+to_string(get_id())+", expecting 2 input value,get "+to_string(father_value.size())+". returning nullptr.");
        }
        sess->add_assign_task(father[0], father_value[1]);
        return father_value[1];
    }  
    std::vector<const_pDiff> Assign::run_diff(Session *sess, std::vector<const_pData> father_value) const
    {
        if(father_value.size()!=2)
        {
            Message::error("evaluating diff of node #"+to_string(get_id())+", expecting 2 input value,get "+to_string(father_value.size())+". returning empty vector.");
            return std::vector<const_pDiff>();
        }
        const_pTensor f = to_Tensor(father_value[1]);
        auto shape = f -> get_shape();
        return vector<const_pDiff>{Diff::(shape)} ;
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
