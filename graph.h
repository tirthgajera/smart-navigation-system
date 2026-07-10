#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <QString>
struct DFSStep
{
    int from;
    int to;
    bool forward;
};
class Graph
{
public:
    Graph();

    void initializeGraph();

    bool shortestPath(
        int src,
        int dest,
        std::vector<int>& path,
        int& distance
        );

    void DFS(int start, std::vector<int>& order);

    void DFSAnimation(
        int start,
        std::vector<DFSStep>& steps
        );

    void BFSLevels(
        int start,
        std::vector<std::vector<int>>& levels
        );

    int MST(
        std::vector<std::pair<int,int>>& mstEdges
        );

    const std::vector<QString>& getCities() const;
    const std::vector<std::vector<std::pair<int,int>>>& getGraph() const;

private:

    std::vector<QString> cities;

    std::vector<
        std::vector<
            std::pair<int,int>
            >
        > graph;

    void dfsUtil(
        int node,
        std::vector<bool>& visited,
        std::vector<int>& order
        );
    void dfsAnimationUtil(
        int node,
        int parent,
        std::vector<bool>& visited,
        std::vector<DFSStep>& steps
        );
    int Find(int x,std::vector<int>& parent);

    void Union(
        int a,
        int b,
        std::vector<int>& parent,
        std::vector<int>& rank
        );
};

#endif