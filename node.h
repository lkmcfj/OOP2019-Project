#ifndef _NODE_H_
#define _NODE_H_
#include "data.h"
#include <memory>
#include <vector>
#include <functional>
#include <map>
namespace computational_graph
{

    class Graph;
    class Session;

    //the member function "run" of Node and its derived class may throw exception std::range_error, std::runtime_error when calculating
    class Node
    {
    private:
        int id;
        Graph *g;
    protected:
        std::vector<int> father;
        Node(Graph *_g);
        Node(Graph *_g,std::vector<int> _father);
        void give_id(int newid);//warning when id is not -1
    public:
        const std::vector<int> &get_father() const;
        int get_id() const;
        Graph *get_graph() const;
        void give_symbol(std::string symbol) const;
        virtual int get_type() const;
        //type=0 Node
        //type=1 Variable
        //type=2 Placeholder
        //type=3 Constant
        //type=4 Arith
        //type=5 Single_op
        //type=6 Print
        //type=7 Cmp
        //type=8 Cond
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;//error
        friend class Graph;
    };
    typedef std::shared_ptr<const Node> const_pNode;

    class Variable : public Node
    {
    protected:
        const_pData default_value;
        Variable(Graph *_g,const_pData default_v);
    public:
        static const_pNode create(Graph *g,const_pData default_v);
        const_pData get_default_value() const;
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;//warning
    };

    class Placeholder : public Node
    {
    protected:
        Placeholder(Graph *_g);
    public:
        static const_pNode create(Graph *g);
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;//error
    };

    class Constant : public Node
    {
    protected:
        const_pData value;
        Constant(Graph *_g,const_pData v);
    public:
        static const_pNode create(Graph *g,const_pData v);
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;
    };
        
    typedef std::function<const_pData(const_pData,const_pData)> arith_op;
    class Arith : public Node
    {
        static std::map<std::string,arith_op> str2op;
    protected:
        arith_op op;
        Arith(Graph *_g,int left_id,int right_id,std::string op_str);
    public:
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;
        static const_pNode create(const_pNode left,const_pNode right,std::string op_str);
    };

    typedef std::function<const_pData(const_pData)> single_op;
    class Single_op : public Node
    {
        static std::map<std::string,single_op> str2op;
    protected:
        single_op op;
        Single_op(Graph *_g,int x_id,std::string op_str);
    public:
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;
        static const_pNode create(const_pNode x,std::string op_str);
    };

    class Print : public Node
    {
    protected:
        std::string father_symbol;
        Print(Graph *_g,int x_id,std::string x_symbol);
    public:
        static const_pNode create(Graph *g,int x_id,std::string x_symbol);
        static const_pNode create(const_pNode x,std::string x_symbol);
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;
    };

    typedef std::function<const_pData(const_pData,const_pData)> cmp_op;
    class Cmp : public Node
    {
        static std::map<std::string,cmp_op> str2op;
    protected:
        cmp_op op;
        Cmp(Graph *_g,int left_id,int right_id,std::string op_str);
    public:
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;
        static const_pNode create(const_pNode left,const_pNode right,std::string op_str);
    };

    class Cond : public Node
    {
    protected:
        Cond(Graph *_g,int cond_id,int true_id,int false_id);
    public:
        static const_pNode create(Graph *g,int cond_id,int true_id,int false_id);
        static const_pNode create(const_pNode cond_node,const_pNode true_node,const_pNode false_node);
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;
    };

    const_pNode operator +(const_pNode left,const_pNode right);
    const_pNode operator -(const_pNode left,const_pNode right);
    const_pNode operator *(const_pNode left,const_pNode right);
    const_pNode operator /(const_pNode left,const_pNode right);
    const_pNode sin_node(const_pNode x);
    const_pNode log_node(const_pNode x);
    const_pNode exp_node(const_pNode x);
    const_pNode tanh_node(const_pNode x);
    const_pNode sigmoid_node(const_pNode x);
    const_pNode operator <(const_pNode left,const_pNode right);
    const_pNode operator >(const_pNode left,const_pNode right);
    const_pNode operator <=(const_pNode left,const_pNode right);
    const_pNode operator >=(const_pNode left,const_pNode right);
    const_pNode operator ==(const_pNode left,const_pNode right);
}
#endif
