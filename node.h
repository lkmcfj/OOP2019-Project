#ifndef _NODE_H_
#define _NODE_H_
#include "datatype.h"
#include <memory>
#include <vector>
#include <functional>
namespace computational_graph
{

    class Graph;
    class Session;

    //the member function "run" of Node and its derived class may throw exception std::range_error when calculating
    class Node
    {
    protected:
        int id,type;
        //type=0 Node
        //type=1 Variable
        //type=2 Placeholder
        //type=3 Constant
        //type=4 Arith
        //type=5 Single_op
        //type=6 Print
        //type=7 Cmp
        //type=8 Cond
        Graph *g;
        std::vector<int> father;
        Node(Graph *_g);
    public:
        const std::vector<int> &get_father();
        int get_id();
        void give_id(int newid);//warning when id is not 0
        Graph *get_graph();
        void give_symbol(std::string symbol);
        int get_type();
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value);//error
    };
    typedef std::shared_ptr<const Node> const_pNode;

    class Variable : public Node
    {
    protected:
        const_pData default_value;
    public:
        Variable(Graph *_g,const_pData default_v);
        const_pData get_default_value();
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value);//warning
    };

    class Placeholder : public Node
    {
    public:
        Placeholder(Graph *_g);
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value);//error
    };

    class Constant : public Node
    {
    protected:
        const_pData value;
    public:
        Constant(Graph *_g,const_pData v);
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value);
    };
        
    typedef std::function<const_pData(const Data&,const Data&)> binary_op;
    class Arith : public Node
    {
        static std::map<std::string,binary_op> str2op;
    protected:
        binary_op op;
    public:
        Arith(Graph *_g,int left_id,int right_id,std::string op_str);
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value);
    };

    typedef std::function<const_pData(const Data&)> single_op;
    class Single_op : public Node
    {
        static std::map<std::string,single_op> str2op;
    protected:
        single_op op;
    public:
        Single_op(Graph *_g,int x_id,std::string op);
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value);
    }

    class Print : public Node
    {
    protected:
        std::string father_symbol;
    public:
        Print(Graph *_g,int x_id,std::string x_symbol);
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value);
    };

    typedef std::function<const_pData(const Data&,const Data&)> cmp_op;
    class Cmp : public Node
    {
        static std::map<std::string,cmp_op> str2op;
    protected:
        cmp_op op;
    public:
        Cmp(Graph *_g,int left_id,int right_id,std::string op_str);
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value);
    }

    class Cond : public Node
    {
    public:
        Cond(Graph *_g,int cond_id,int true_id,int false_id);
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value);
    };

    const_pNode operator +(const Node &left,const Node &right);
    const_pNode operator -(const Node &left,const Node &right);
    const_pNode operator *(const Node &left,const Node &right);
    const_pNode operator /(const Node &left,const Node &right);
    const_pNode sin(const Node &x);
    const_pNode log(const Node &x);
    const_pNode exp(const Node &x);
    const_pNode tanh(const Node &x);
    const_pNode sigmoid(const Node &x);
    const_pNode operator <(const Node &left,const Node &right);
    const_pNode operator >(const Node &left,const Node &right);
    const_pNode operator <=(const Node &left,const Node &right);
    const_pNode operator >=(const Node &left,const Node &right);
    const_pNode operator ==(const Node &left,const Node &right);
}
#endif
