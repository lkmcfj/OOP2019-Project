#include "computational_graph.h"
#include <iostream>
#include <string>
using namespace computational_graph;
int main()
{
    Graph default_graph;
    Parser default_parser;
    int n;
    std::cin>>n;
    while(n--)
    {
        std::string input_s;
        std::getline(std::cin,input_s);
        default_parser.start(input_s,&default_graph);
    }
    int m;
    std::cin>>m;
    while(m--)
    {
        std::string input_s;
        std::getline(std::cin,input_s);
        default_parser.node(input_s,&default_graph);
    }
    Session default_sess(default_graph);
    int q;
    std::cin>>q;
    while(q--)
    {
        std::string input_s;
        std::getline(std::cin,input_s);
        default_parser.run(input_s,&default_sess);
    }
}