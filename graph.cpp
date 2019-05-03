#include "graph.h"
#include "node.h"
#include "message.h"
#include "data.h"
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <exception>
#include <utility>
namespace computational_graph
{
    using std::string;
    const_pNode Graph::join(std::unique_ptr<Node> curnode)
    {
        int cur=nodes.size();
        curnode->give_id(cur);
        nodes.push_back(std::move(curnode));
        if(std::dynamic_pointer_cast<const Variable>(nodes[cur])) variable_id.push_back(cur);
        return nodes[cur];
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
        last_variable_id=-1;
        for(const_pNode i:g.nodes) if(i->get_type()==1)
        {
            last_variable_id=i->get_id();
            variable_value[i->get_id()]=std::dynamic_pointer_cast<const Variable>(i)->get_default_value();
        }
    }
    Graph* Session::get_graph()
    {
        return &g;
    }
    const_pData Session::dfs_eval(int id)
    {
        if(g.nodes[id]->get_type()==1) return variable_value[id];
        if(g.nodes[id]->get_type()==2)
        {
            auto it=temp_value.find(id);
            if(it==temp_value.end())
            {
                Message::message("ERROR: Placeholder missing");
                throw std::invalid_argument("Missing Placeholder: node #"+std::to_string(id));
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
    const_pData Session::eval(int id,std::map<int,const_pData> placeholder_value)
    {
        for(int i=g.variable_id.size()-1;i>=0&&g.variable_id[i]>last_variable_id;--i)
        {
            if(variable_value.find(g.variable_id[i])==variable_value.end())
                variable_value[g.variable_id[i]]=std::dynamic_pointer_cast<const Variable>(g.nodes[g.variable_id[i]])->get_default_value();

        }
        if(g.variable_id.size()>0) last_variable_id=g.variable_id[g.variable_id.size()-1];
        temp_value=std::move(placeholder_value);
        return dfs_eval(id);
    }
    const_pData Session::eval(const_pNode p,std::map<const_pNode,const_pData> placeholder_value)
    {
        if((p->get_graph()!=&g)||(p->get_id()<0))
        {
            Message::error("In Session::eval(),the node evaluated is not in this session, returning null data");
            return nullptr;
        }
        std::map<int,const_pData> pvalue;
        for(auto i: placeholder_value)
        {
            if((i.first->get_graph()!=&g)||(i.first->get_id()<0))
            {
                Message::warning("In Session::eval(), get a placeholder node which is not in this session");
            } else pvalue[i.first->get_id()]=i.second;
        }
        return eval(p->get_id(),std::move(pvalue));
    }
    void Session::set_variable(int id,const_pData v)
    {
        if(id<0||id>=g.nodes.size())
        {
            Message::error("In Session::set_variable(), invalid node id");
            return;
        }
        variable_value[id]=v->copy();
    }
    void Session::set_variable(string symbol,const_pData v)
    {
        auto it=g.symbol_id.find(symbol);
        if(it==g.symbol_id.end())
        {
            Message::error("In Session::set_variable(),invalid node symbol");
            return;
        }
        variable_value[it->second]=v->copy();
    }
    void Session::set_variable(const_pNode p,const_pData v)
    {
        if((p->get_graph()!=&g)||(p->get_id()<0))
        {
            Message::error("in Session::set_variable():the variable node is not in this session, failed");
            return;
        }
        variable_value[p->get_id()]=v->copy();
    }
}
