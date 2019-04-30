#include "graph.h"
#include "node.h"
#include "message.h"
#include "datatype.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <exception>
namespace computational_graph
{
    using std::string;
    const_pNode Graph::join(std::unique_ptr<Node> curnode)
    {
        curnode->give_id(nodes.size());
        nodes.push_back(std::move(curnode));
        return nodes[nodes.size()-1];
    }
    const_pNode Graph::getnode(int id)
    {
        return nodes[id];
    }
    const_pNode Graph::getnode(string symbol)
    {
        return nodes[symbol_id[symbol]];
    }
    int Graph::get_symbol_id(string symbol)
    {
        return symbol_id[symbol];
    }
    void Graph::give_symbol(string symbol,int id)
    {
        symbol_id[symbol]=id;
    }
    Session::Session(Graph &_g):g(_g)
    {
        for(const_pNode i:g.nodes) if(i->get_type()==1)
            variable_value[i->get_id()]=dynamic_pointer_cast<Variable>(i)->get_default_value();
    }
    const_pData Session::dfs_eval(int id)
    {
        if(g.nodes[id]->gettype()==1) return variable_value[id];
        if(g.nodes[id]->gettype()==2)
        {
            auto it=temp_value.find(id);
            if(it==temp_value.end())
            {
                Message::error("Missing Placeholder: node #"+std::to_string(id));
                throw std::invalid_argument("ERROR: Placeholder missing");
            }
            return it->second;
        }
        auto it=temp_value.find(id);
        if(it!=temp_value.end()) return it->second;
        std::vector<const_pData> prev;
        for(int i: g.nodes[id]->get_father())
        {
            prev.push_back(dfs_eval(id));
        }
        return temp_value[id]=g.nodes[id]->run(this,prev);
    }
    const_pData eval(int id,std::map<int,const_pData> placeholder_value)
    {
        temp_value=std::move(placeholder_value);
        return dfs_eval(id);
    }
    void set_variable(int id,const_pData v)
    {
        variable_value[id]=v->copy();
    }
    void set_variable(string symbol,const_pData v)
    {
        variable_value[g.symbol_id[symbol]]=v->copy();
    }
}
