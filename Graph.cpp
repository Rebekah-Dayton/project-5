//
// Created by Rebek on 11/21/2022.
//

#include "Graph.h"

void Graph::AddAdjacentNode(int node, int adjacent) {
    for (auto it = graph.begin(); it != graph.end(); it++) {
        if (it->first == node) {
            it->second.insert(adjacent);
        }
    }
}

void Graph::AddNode(int node, std::set<int> adjacency) {
    graph.insert(std::pair<int, std::set<int>>(node, adjacency));
    visited.push_back(false);
}

void Graph::To_String() {
    std::string tostring = "";
    for (auto it = graph.begin(); it != graph.end(); it++) {
        tostring += "R" + std::to_string(it->first) + ":";
        for (int i : it->second) {
            tostring += "R" + std::to_string(i) + ",";
        }
        if (it->second.empty()) {
            tostring += " ";
        }
        tostring.pop_back();
        tostring += "\n";
    }
    std::cout << tostring << std::endl;
}

std::set<int> Graph::DepthFirstSearch(int vertex) {
    visited.at(vertex) = true;
    for (auto it = graph.begin(); it != graph.end(); it++) {
        if (it->first == vertex) {
            if (!it->second.empty()) {
                for (int i: it->second) {
                    if (!visited.at(i)) {
                        DepthFirstSearch(i);
                    }
                }
            }
        }
    }
    tree.insert(vertex);
    postorder.push_back(vertex);
    return tree;
}

std::vector<int> Graph::DFSForestPostorder(std::vector<int> toVisit) {
    tree.clear();
    std::vector<int> tree;
    for (unsigned int i = 0; i < visited.size(); i++) {
        visited.at(i) = false;
    }
    if (toVisit.size() == graph.size()) {
        for (unsigned int i = 0; i < toVisit.size(); i++) {
            if (!visited.at(toVisit.at(i))) {
                DepthFirstSearch(toVisit.at(i));
            }
        }
    }
    return postorder;
}

std::vector<std::set<int>> Graph::DFSForestSCCs(std::vector<int> postorder) {
    tree.clear();
    std::vector<std::set<int>> forest;
    std::set<int> tree;
    for (unsigned int i = 0; i < visited.size(); i++) {
        visited.at(i) = false;
    }

    if (postorder.size() == graph.size()) {
        for (unsigned int i = 0; i < postorder.size(); i++) {
            if (!visited.at(postorder.at(i))) {
                this->tree.clear();
                tree = DepthFirstSearch(postorder.at(i));
                forest.push_back(tree);
            }
        }
    }
    return forest;
}

Graph Graph::ReverseGraph() {
    Graph reverse;
    for (auto it = graph.begin(); it != graph.end(); it++) {
        reverse.AddNode(it->first, {});
        for (auto node = graph.begin(); node != graph.end(); node++) {
            for (int i : node->second) {
                if (i == it->first) {
                    reverse.AddAdjacentNode(it->first, node->first);
                }
            }
        }
    }
    return reverse;
}

int Graph::FindNode(int i) {
    for (auto it = graph.begin(); it != graph.end(); it++) {
        if (it->first == i) {
            return i;
        }
    }
    return -1;
}

