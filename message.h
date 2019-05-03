#ifndef _MESSAGE_H
#define _MESSAGE_H
#include "node.h"
#include "graph.h"
#include "data.h"
 
namespace computational_graph
{
	class Message
	{
	private:	
		void debug(std::string s);
		void info(std::string s);
		void warning(std::string s);
		void error(std::string s);
		void message(std::string s);
		static int log_level;
		static std::ostream *log_s,*message_s;
	public:
		void Message::set_log_level(int level);
		void Message::set_log_stream(std::ostream &s);
		void Message::set_message_stream(std::ostream &s);
		friend class Node;
		friend class Graph;
		friend class Data;
		friend class Session;
	};
}
#endif
