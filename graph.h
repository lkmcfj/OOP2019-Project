#ifndef _GRAPH_H_
#define _GRAPH_H
#include "node.h"
#include <vector>
#include <memory>
#include <string>
#include <map>
namespace computational_graph
{

    class Graph
    {
    private:
        std::vector<const_pNode> nodes;
        std::vector<int> variable_id;
        std::map<std::string,int> symbol_id;
    public:
        const_pNode join(std::unique_ptr<Node> curnode);
        const_pNode getnode(int id);
        const_pNode getnode(std::string symbol);
        int get_symbol_id(std::string symbol);
        void give_symbol(std::string symbol,int id);
        friend class Session;
    };

    class Session
    {
    private:
        std::map<int,const_pData> variable_value,temp_value;
        Graph &g;
        const_pData dfs_eval(int id);
        int last_variable_id;
    public:
        Session(Graph &_g);
        Graph* get_graph();
        const_pData eval(int id,std::map<int,const_pData> placeholder_value);
        const_pData eval(const_pNode p,std::map<const_pNode,const_pData> placeholder_value);
	//may throw std::invalid_argument, std::range_error or std::runtime_error when calculating
        void set_variable(int id,const_pData v);
        void set_variable(std::string symbol,const_pData v);
        void set_variable(const_pNode p,const_pData v);
    };

}
#endif
