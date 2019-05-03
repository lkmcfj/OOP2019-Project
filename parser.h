#include "node.h"
namespace computational_graph
{
	class Parser
	{
	public:
		const_pNode start(std::string s, Graph *g);
		const_pNode node(std::string s, Graph *g);
		const_pData run(std::string s, Session *sess);
	};	
} 
