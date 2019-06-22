#ifndef _GRAPH_H_
#define _GRAPH_H_
#include "node.h"
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <set>
#include "fileop.h"
namespace computational_graph
{

    class Graph
    {
    private:
        std::vector<const_pNode> nodes;
        Graph() =default;
    public:
        static const flag_t _flag;
        void save(FileWriter &out);
        static std::shared_ptr<Graph> load(FileReader &in);

        const_pNode join(std::unique_ptr<Node> curnode);
        const_pNode getnode(int id);
        void save(FileWriter &out);
        friend class Session;
        friend class Grad;
        static std::shared_ptr<Graph> create();
    };
    typedef std::shared_ptr<Graph> pGraph;

    class Session
    {
    private:
        std::map<int,const_pData> variable_value,temp_value;
        std::set<int> vis;
        std::vector<int> vislist;
        std::vector<std::pair<int,const_pData>> assign_tasks;
        pGraph g;
    public:
        static const flag_t _flag;
        void save(FileWriter &out);
        static Session load(FileReader &in);

        Session(pGraph _g);
        void save(FileWriter &out);
        pGraph get_graph();
        const_pData eval(int id,std::map<int,const_pData> placeholder_value);
        const_pData eval(const_pNode p,std::map<const_pNode,const_pData> placeholder_value);
	//may throw std::invalid_argument, std::range_error or std::runtime_error when calculating
        void set_variable(int id,const_pData v);
        void set_variable(const_pNode p,const_pData v);
        void add_assign_task(int id, const_pData v);
        friend class Grad;
    };

}
#endif
