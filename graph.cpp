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
#include <queue>
#include "fileop.h"
namespace computational_graph
{
    using std::string;
    using std::to_string;
    using std::dynamic_pointer_cast;

    const flag_t Graph::_flag=0x0201, Session::_flag=0x0101;
    void Graph::save(FileWriter &out)
    {
        out.write(_flag);
        out.write<int>(nodes.size());
        for(auto i:nodes) i->save(out);
    }
    void Session::save(FileWriter &out)
    {
        out.write(_flag);
        out.write<int>(variable_value.size());
        for(auto &i:variable_value)
        {
            out.write(i.first);
            i.second->save(out);
        }
        g->save(out);
    }

    Graph::Graph(){}
    pGraph Graph::create()
    {
		return std::shared_ptr<Graph>(new Graph());
	}
    const_pNode Graph::join(std::unique_ptr<Node> curnode)
    {
        int cur=nodes.size();
        curnode->give_id(cur);
        nodes.push_back(std::move(curnode));
        return nodes[cur];
    }
    const_pNode Graph::getnode(int id)
    {
        return nodes[id];
    }

    Session::Session(pGraph _g):g(_g){}
    pGraph Session::get_graph()
    {
        return g;
    }
    void Session::add_assign_task(int id,const_pData v)
    {
		assign_tasks.push_back(std::make_pair(id,v->copy()));
	}
    const_pData Session::eval(int xid,std::map<int,const_pData> placeholder_value)
    {
		assign_tasks.clear();
        temp_value=std::move(placeholder_value);
        for(auto i: variable_value) temp_value[i.first]=i.second;
        vis.clear();
        vislist.clear();
        std::queue<int> q;
        q.push(xid);
        vis.insert(xid);
        while(!q.empty())
        {
            int cur=q.front();
            q.pop();
            for(int i: g->nodes[cur]->get_father()) if(vis.find(i)==vis.end())
            {
                q.push(i);
                vis.insert(i);
            }
        }
        for(int id:vis)
        {
            vislist.push_back(id);
            Message::debug("In Session::eval(),evaluating node#"+to_string(id));
            if(g->nodes[id]->get_type()==1) 
            {
                auto i=temp_value.find(id);
                if(i==temp_value.end())
                    variable_value[id]=temp_value[id]=dynamic_pointer_cast<const Variable>(g->nodes[id])->get_default_value();
            } else
            if(g->nodes[id]->get_type()==2)
            {
                auto i=temp_value.find(id);
                if(i==temp_value.end())
                {
                    Message::message("ERROR: Placeholder missing");
                    throw std::invalid_argument("Missing Placeholder: node #"+std::to_string(id));
                }
            } else
            {
				std::vector<const_pData> prev;
				for(int i: g->nodes[id]->get_father())
					prev.push_back(temp_value[i]);
				temp_value[id]=g->nodes[id]->run(this,prev);
			}
		}
		for(auto &i: assign_tasks) variable_value[i.first]=i.second;
        return temp_value[xid];
    }
    const_pData Session::eval(const_pNode p,std::map<const_pNode,const_pData> placeholder_value)
    {
        if(( p->get_graph().lock() != g ) || (p->get_id()<0))
        {
            Message::error("In Session::eval(),the node evaluated is not in this session, returning null data");
            return nullptr;
        }
        std::map<int,const_pData> pvalue;
        for(auto i: placeholder_value)
        {
            if((i.first->get_graph().lock() !=g)||(i.first->get_id()<0))
            {
                Message::warning("In Session::eval(), get a placeholder value pair which is not in this session");
            } else if(i.first->get_type()!=2)
            {
                Message::warning("In Session::eval(), get a placeholder value pair which is not placeholder");
            } else pvalue[i.first->get_id()]=i.second;
        }
        return eval(p->get_id(),std::move(pvalue));
    }
    void Session::set_variable(int id,const_pData v)
    {
        if(id<0||id>=g->nodes.size())
        {
            Message::error("In Session::set_variable(), invalid node id");
            return;
        }
        variable_value[id]=v->copy();
    }
    void Session::set_variable(const_pNode p,const_pData v)
    {
        if((p->get_graph().lock() !=g)||(p->get_id()<0))
        {
            Message::error("in Session::set_variable():the variable node is not in this session, failed");
            return;
        }
        variable_value[p->get_id()]=v->copy();
    }
}
