//
// Created by Rebek on 11/21/2022.
//

#ifndef PROJECT_1_GRAPH_H
#define PROJECT_1_GRAPH_H
#include <iostream>
#include <vector>
#include <set>
#include <map>


class Graph {
private:
    std::map<int, std::set<int>> graph;
    std::vector<bool> visited;
    std::set<int> tree;
    std::vector<int> postorder;
public:
    Graph() = default;
    ~Graph() = default;

    void AddNode(int node, std::set<int> adjacency);
    void AddAdjacentNode(int node, int adjacent);
    int FindNode(int i);

    Graph ReverseGraph();
    std::set<int> DepthFirstSearch(int vertex);
    std::vector<int> DFSForestPostorder(std::vector<int> toVisit);
    std::vector<std::set<int>> DFSForestSCCs(std::vector<int> postorder);
    bool CheckDependency(int i);

    void To_String();
};


#endif //PROJECT_1_GRAPH_H
