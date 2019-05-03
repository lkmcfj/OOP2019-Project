#include "message.h"
#include <iostream>

namespace computational_graph
{
	int Message::log_level=3; //modify from 1 to 5
	std::ostream *Message::log_s = &std::cerr;
	std::ostream *Message::message_s = &std::cout;
	void Message::debug(std::string s)
	{
		if(log_level<=1)
		*log_s << "[debug] " << s << std::endl;
	}
	void Message::info(std::string s)
	{
		if(log_level<=2)
		*log_s << "[info] " << s << std::endl;
	}
	void Message::warning(std::string s)
	{
		if(log_level<=3)
		*log_s << "[warning] " << s << std::endl;
	}
	void Message::error(std::string s)
	{
		if(log_level<=4)
		*log_s << "[error] " << s << std::endl;
	}
	void Message::message(std::string s)
	{
		*message_s << s << "\n";
	}
	void Message::set_log_level(int level)
	{
		log_level=level;		
	}
	void Message::set_log_stream(std::ostream &s)
	{
		log_s=&s;
	}
	void Message::set_message_stream(std::ostream &s)
	{
		message_s=&s;
	}
}
