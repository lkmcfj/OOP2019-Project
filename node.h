#ifndef _NODE_H_
#define _NODE_H_
#include "data.h"
#include "dataop.h"
#include <memory>
#include <vector>
#include <functional>
#include <map>
namespace computational_graph
{

    class Graph;
    class Session;
    typedef std::weak_ptr<Graph> wGraph;
    typedef std::shared_ptr<Graph> pGraph;

    //the member function "run" of Node and its derived class may throw exception std::range_error, std::runtime_error when calculating
    class Node
    {
    private:
        int id;
        wGraph g;
    protected:
        std::vector<int> father;
        Node(wGraph _g);
        Node(wGraph _g,std::vector<int> _father);
        void give_id(int newid);//warning when id is not -1
    public:
        const std::vector<int> &get_father() const;
        int get_id() const;
        wGraph get_graph() const;
        virtual int get_type() const =0;
        //type=0 Node
        //type=1 Variable
        //type=2 Placeholder
        //type=3 Constant
        //type=4 Arith
        //type=5 Single_op
        //type=6 Print
        //type=7 Cmp
        //type=8 Cond
        //type=9 Assert
        //type=10 Bind
        //type=11 Grad
        //type=12 At
        //type=13 Assign
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value)const =0;//error
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value)const =0;
        friend class Graph;
        virtual ~Node() = default;
    };
    typedef std::shared_ptr<const Node> const_pNode;

    class Variable : public Node
    {
    protected:
        const_pData default_value;
        Variable(wGraph _g,const_pData default_v);
    public:
        static const_pNode create(pGraph g,const_pData default_v);
        const_pData get_default_value() const;
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;//warning
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
    };

    class Placeholder : public Node
    {
    protected:
        Placeholder(wGraph _g);
    public:
        static const_pNode create(pGraph g);
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;//error
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
    };

    class Constant : public Node
    {
    protected:
        const_pData value;
        Constant(wGraph _g,const_pData v);
    public:
        static const_pNode create(pGraph g,const_pData v);
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
    };
        
    class Arith : public Node
    {
        static std::map<std::string,BinaryTensorOp> str2op;
    protected:
        BinaryTensorOp op;
        Arith(wGraph _g,int left_id,int right_id,std::string op_str);
    public:
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;
        static const_pNode create(const_pNode left,const_pNode right,std::string op_str);
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
    };

    class Single_op : public Node
    {
        static std::map<std::string,SingleTensorOp> str2op;
    protected:
        SingleTensorOp op;
        Single_op(wGraph _g,int x_id,std::string op_str);
    public:
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;
        static const_pNode create(const_pNode x,std::string op_str);
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
    };

    class Print : public Node
    {
    protected:
        std::string father_symbol;
        Print(wGraph _g,int x_id,std::string x_symbol);
    public:
        static const_pNode create(pGraph g,int x_id,std::string x_symbol);
        static const_pNode create(const_pNode x,std::string x_symbol);
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
    };

    typedef std::function<const_pData(const_pData,const_pData)> cmp_op;
    class Cmp : public Node
    {
        static std::map<std::string,cmp_op> str2op;
    protected:
        cmp_op op;
        Cmp(wGraph _g,int left_id,int right_id,std::string op_str);
    public:
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;
        static const_pNode create(const_pNode left,const_pNode right,std::string op_str);
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
    };

    class Cond : public Node
    {
    protected:
        Cond(wGraph _g,int cond_id,int true_id,int false_id);
    public:
        static const_pNode create(pGraph g,int cond_id,int true_id,int false_id);
        static const_pNode create(const_pNode cond_node,const_pNode true_node,const_pNode false_node);
        virtual int get_type() const;
        virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const;
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
    };

    class Assert : public Node
    {
    protected:
        Assert(wGraph _g, int x_id);
    public:
        static const_pNode create(pGraph g, int x_id);
        static const_pNode create(const_pNode x);       
        virtual int get_type() const;
        virtual const_pData run(Session *sess, std::vector<const_pData> father_value) const;
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
    };

    class Bind : public Node
    {
    protected:
        Bind(wGraph _g, int left_id, int right_id);
    public: 
        static const_pNode create(pGraph g, int left_id, int right_id);
        static const_pNode create(const_pNode left, const_pNode right);     
        virtual int get_type() const;
        virtual const_pData run(Session *sess, std::vector<const_pData> father_value) const;        
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
    };

    class Grad : public Node
    {
    protected:
        Grad(wGraph _g, int x_id);
    public:
        static const_pNode create(pGraph g, int x_id);
        static const_pNode create(const_pNode x);       
        virtual int get_type() const;
        virtual const_pData run(Session *sess, std::vector<const_pData> father_value) const;        
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
    };

    class At : public Node
    {
    protected:
        At(wGraph _g, int grad_id, int x_id);
    public:
        static const_pNode create(pGraph g, int grad_id, int x_id);
        static const_pNode create(const_pNode grad, const_pNode x);      
        virtual int get_type() const;
        virtual const_pData run(Session *sess, std::vector<const_pData> father_value) const;            
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
    };

    class Assign : public Node
    {
    protected:
        Assign(wGraph _g, int left_id, int right_id);
    public:
        static const_pNode create(pGraph g, int left_id, int right_id);
        static const_pNode create(const_pNode left, const_pNode right);
        virtual int get_type() const;
        virtual const_pData run(Session *sess, std::vector<const_pData> father_value) const;
        virtual std::vector<const_pDiff> run_diff(Session *sess, std::vector<const_pData> father_value) const;
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
