#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <iostream>
namespace computational_graph
{
	class Message
	{
	private:
		static int log_level;
		static std::ostream *log_s,*message_s;
	public:
		static void debug(std::string s);
		static void info(std::string s);
		static void warning(std::string s);
		static void error(std::string s);
		static void message(std::string s);
		static void set_log_level(int level);
		static void set_log_stream(std::ostream &s);
		static void set_message_stream(std::ostream &s);
	};
}
#endif
