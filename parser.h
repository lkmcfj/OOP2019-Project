#ifndef _PARSER_H_
#define _PARSER_H_
#include "node.h"
#include "data.h"
#include "graph.h"
#include <string>
namespace computational_graph
{
	class Parser
	{
        std::vector<const_pData> res;
        std::map<std::string, const_pNode> symbol;
	public:
		const_pNode start(std::string s, pGraph g);
		const_pNode node(std::string s, pGraph g);
		const_pData run(std::string s, Session *sess);
	};	
} 
#endif
