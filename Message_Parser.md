## Message类
```Message```类的设计目的在于输出信息。我们将Message类的所有成员均设为```static```类型。类的私有成员包括日志等级```log_level```以及输出流```*log_s，*message_s```，由日志等级来判断是否输出相应错误信息，并在实现文档一开始对其初始化。
```cpp
int Message::log_level=3;
std::ostream *Message::log_s = &std::cerr;
std::ostream *Message::message_s = &std::cout;
```  
我们在类中提供了```set_log_level,set_log_stream,set_message_stream```函数，以实现对```log_level,*log_s,*message_s```的可能的修改。

```cpp
void Message::set_log_level(int level)
void Message::set_log_stream(std::ostream &s)
void Message::set_message_stream(std::ostream &s)
```  
类的成员函数按日志等级，分为```debug,info,warning,error```函数以及```message```函数。

```cpp
void Message::debug(std::string s)
void Message::info(std::string s)
void Message::warning(std::string s)
void Message::error(std::string s)
void Message::message(std::string s)
```  
其中
```debug   	log_level<=1```
```info    	log_level<=2```
```warning 	log_level<=3```
```error   	log_level<=4```
均通过```*log_s```输出信息。
```message```通过```*message_s```输出。

## Parser类
```Parser```类设计的主要目的在于解析字符串。设计了一系列函数，来满足第一阶段需求。依照传入的字符串，创建一系列在```Graph```管理下的节点并执行赋值、求值等相关操作。并且在私有成员中用vector```<const_pNode>```
我们设计了```split```函数，用于在将传入的字符串以空格分割为若干段并存进```vector```中，便于进行下一步的操作。
```cpp
void split(std::string &s, std::vector<std::string> &res, char ch)
```  
在每次传入字符串的一开始，先对其进行分割，再依据分割的若干段字符串进行操作。

### 成员函数
```cpp
const_pNode start(std::string s, Graph *g)
```  
用于在一开始创建出相应类型的变量节点```Placeholder\Constant\Variable```并返回指向该节点的智能指针。

```cpp
const_pNode node(std::string s, Graph *g)
```  
用于根据字符串创建出依赖于其他节点的新节点，并返回新节点的智能指针。

```cpp
const_pData run(std::string s, Graph *g)
```  
用于执行计算或赋值操作，并返回求值结果的智能指针（若为赋值，则返回空指针）。
