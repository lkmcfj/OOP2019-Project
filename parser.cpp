#include "node.h"
#include "graph.h"
#include "message.h"
#include "data.h"
#include "parser.h"
#include <string> 
#include <vector>
#include <map>
namespace computational_graph
{
	using std::string;
	using std::vector;
	using std::map;
	void split(string &s,vector<string> &res,char ch)
	{
    	int last=-1;
    	for(int i=0;i<s.length();++i) 
			if(s[i]==ch)
    		{
        		if(last<i-1) 
					res.push_back(s.substr(last+1,i-last-1));
        		last=i;
    		}
    	if(last<s.length()-1) res.push_back(s.substr(last+1));
	}
	const_pNode Parser::start(string s, pGraph g)
	{
        Message::debug("Parser::start() called, input string is "+s);
		vector<string> nodes;
		split(s, nodes, ' ');
		const_pNode ret;
		if(nodes[1]=="P")
		{
			ret=Placeholder::create(g); 
		}//Placeholder
		else
		{
			double value=stod(nodes[2]);
			const_pData v = Float::create(value);
			if(nodes[1]=="C")
			{
				ret=Constant::create(g, v);
			}//Constant
			if(nodes[1]=="V")
			{
				ret=Variable::create(g, v);
			}//Variable
		}
		symbol[nodes[0]]=ret;
		return ret;
	}
	const_pNode Parser::node(string s, pGraph g)
	{
        Message::debug("Parser::node() called, input string is "+s);
		vector<string> new_node;
		split(s, new_node, ' ');
		const_pNode ret;
		if(new_node[3]=="+"||new_node[3]=="-"||new_node[3]=="*"||new_node[3]=="/")
		{
			ret=Arith::create(symbol[new_node[2]],symbol[new_node[4]],new_node[3]);
		}//Arith
		if(new_node[2]=="SIN"||new_node[2]=="LOG"||new_node[2]=="EXP"||new_node[2]=="SIGMOID"||new_node[2]=="TANH")
		{
			ret=Single_op::create(symbol[new_node[3]],new_node[2]);
		}//Single_op
		if(new_node[2]=="PRINT")
		{
			ret=Print::create(symbol[new_node[3]],new_node[3]);
		}//Print
		if(new_node[3]=="<="||new_node[3]=="<"||new_node[3]==">="||new_node[3]==">"||new_node[3]=="==")
		{
			ret=Cmp::create(symbol[new_node[2]],symbol[new_node[4]],new_node[3]);
		}//Cmp
		if(new_node[2]=="COND")
		{
			ret=Cond::create(symbol[new_node[3]],symbol[new_node[4]],symbol[new_node[5]]);
		}//Cond
		if(new_node[2]=="ASSERT")
		{
			ret=Assert::create(symbol[new_node[3]]);
		}//Assert
		if(new_node[2]=="BIND")
		{
			ret=Bind::create(symbol[new_node[3]],symbol[new_node[4]]);
		}//Bind
		if(new_node[2]=="GRAD")
		{
			ret=Grad::create(symbol[new_node[3]]);
		}//Grad
		if(new_node[3]=="AT")
		{
			ret=At::create(symbol[new_node[2]],symbol[new_node[4]]);
		}//At
		symbol[new_node[0]]=ret;
        return ret;
	}
	const_pData Parser::run(string s, Session *sess)
	{
        Message::debug("Parser::run() called, input string is "+s);
		vector <string> eval;
		map <const_pNode, const_pData> placeholder_value;
		split(s,eval,' ');
		const_pData ret;
		if(eval[0]=="SETCONSTANT")
		{
			const_pData v=Float::create(stod(eval[2]));
			sess->set_variable(symbol[eval[1]],v);
			ret=nullptr;
		}//Variable-setconstant
		if(eval[0]=="SETANSWER")
		{
			const_pData v=res[stoi(eval[2])-1];
			sess->set_variable(symbol[eval[1]],v);
			ret=nullptr;
		}//Variable-setanswer	
		if(eval[0]=="EVAL")
		{
			int k;
			if(eval.size()>2) k=stoi(eval[2]);else k=0;
			for(int i=1;i<=k;i++)
			{
				placeholder_value[symbol[eval[2*i+1]]]=Float::create(stod(eval[2*i+2]));
			}
			try
            {
                ret=sess->eval(symbol[eval[1]],placeholder_value);
            }catch(std::invalid_argument err)
            {
                Message::error(err.what());
                ret=nullptr;
            }catch(std::range_error err)
            {
                Message::error(err.what());
                ret=nullptr;
            }catch(std::runtime_error err)
            {
                Message::error(err.what());
                ret=nullptr;
            }
            if(ret) Message::message(ret->to_string());
		}//Eval
		res.push_back(ret);
		return ret;
	}
} 
