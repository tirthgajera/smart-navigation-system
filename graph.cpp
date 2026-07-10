#include "graph.h"

#include <queue>
#include <algorithm>
#include <climits>

using namespace std;

Graph::Graph()
{
}

void Graph::initializeGraph()
{
    cities =
        {
            "Chandigarh",      //0
            "Mohali",          //1
            "Panchkula",       //2
            "Solan",           //3
            "Shimla",          //4
            "Kufri",           //5
            "Narkanda",        //6
            "Rampur",          //7
            "Sangla",          //8
            "Kalpa",           //9
            "Reckong Peo",     //10
            "Tabo",            //11
            "Kaza",            //12
            "Keylong",         //13
            "Manali",          //14
            "Kullu",           //15
            "Mandi",           //16
            "Bilaspur",        //17
            "Hamirpur",        //18
            "Dharamshala"      //19
        };

    graph.assign(cities.size(), {});

    auto addEdge=[&](int u,int v,int w)
    {
        graph[u].push_back({v,w});
        graph[v].push_back({u,w});
    };

    addEdge(0,1,12);
    addEdge(0,2,10);
    addEdge(1,3,55);
    addEdge(2,3,48);
    addEdge(3,4,46);
    addEdge(4,5,16);
    addEdge(5,6,60);
    addEdge(6,7,40);
    addEdge(7,8,92);
    addEdge(8,9,38);
    addEdge(9,10,12);
    addEdge(10,11,150);
    addEdge(11,12,50);
    addEdge(12,13,185);
    addEdge(13,14,115);
    addEdge(14,15,40);
    addEdge(15,16,72);
    addEdge(16,17,70);
    addEdge(17,18,63);
    addEdge(18,19,32);

    addEdge(4,14,248);
    addEdge(16,4,145);
    addEdge(7,14,230);
    addEdge(3,17,105);
    addEdge(15,19,205);
    addEdge(2,17,92);
    addEdge(5,15,190);
}

const vector<QString>& Graph::getCities() const
{
    return cities;
}

const vector<vector<pair<int,int>>>& Graph::getGraph() const
{
    return graph;
}

bool Graph::shortestPath(
    int src,
    int dest,
    vector<int>& path,
    int& totalDistance)
{
    int n = graph.size();

    vector<int> dist(n,INT_MAX);
    vector<int> parent(n,-1);

    priority_queue<
        pair<int,int>,
        vector<pair<int,int>>,
        greater<pair<int,int>>
        > pq;

    dist[src]=0;

    pq.push({0,src});

    while(!pq.empty())
    {
        auto cur=pq.top();
        pq.pop();

        int d=cur.first;
        int u=cur.second;

        if(d>dist[u])
            continue;

        for(auto edge:graph[u])
        {
            int v=edge.first;
            int w=edge.second;

            if(dist[u]+w<dist[v])
            {
                dist[v]=dist[u]+w;
                parent[v]=u;

                pq.push({dist[v],v});
            }
        }
    }

    if(dist[dest]==INT_MAX)
        return false;

    path.clear();

    for(int v=dest;v!=-1;v=parent[v])
        path.push_back(v);

    reverse(path.begin(),path.end());

    totalDistance=dist[dest];

    return true;
}
void Graph::dfsUtil(int node,
                    std::vector<bool>& visited,
                    std::vector<int>& order)
{
    visited[node] = true;

    order.push_back(node);

    for(auto edge : graph[node])
    {
        int next = edge.first;

        if(!visited[next])
        {
            dfsUtil(next, visited, order);
        }
    }
}

void Graph::DFS(int start,
                std::vector<int>& order)
{
    order.clear();

    std::vector<bool> visited(graph.size(), false);

    dfsUtil(start, visited, order);
}
void Graph::DFSAnimation(
    int start,
    std::vector<DFSStep>& steps)
{
    steps.clear();

    std::vector<bool> visited(graph.size(), false);

    dfsAnimationUtil(start, -1, visited, steps);
}

void Graph::dfsAnimationUtil(
    int node,
    int parent,
    std::vector<bool>& visited,
    std::vector<DFSStep>& steps)
{
    visited[node] = true;

    for(auto edge : graph[node])
    {
        int next = edge.first;

        if(!visited[next])
        {
            steps.push_back({node, next, true});   // forward

            dfsAnimationUtil(next, node, visited, steps);

            steps.push_back({next, node, false});  // backtrack
        }
    }
}
void Graph::BFSLevels(int start,
                      std::vector<std::vector<int>>& levels)
{
    levels.clear();

    std::vector<bool> visited(graph.size(), false);

    std::queue<int> q;

    q.push(start);

    visited[start] = true;

    while(!q.empty())
    {
        int sz = q.size();

        std::vector<int> level;

        while(sz--)
        {
            int u = q.front();
            q.pop();

            level.push_back(u);

            for(auto edge : graph[u])
            {
                int v = edge.first;

                if(!visited[v])
                {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }

        levels.push_back(level);
    }

}

int Graph::Find(int x,
                std::vector<int>& parent)
{
    if(parent[x]==x)
        return x;

    return parent[x]=Find(parent[x],parent);
}

void Graph::Union(
    int a,
    int b,
    std::vector<int>& parent,
    std::vector<int>& rank)
{
    a=Find(a,parent);
    b=Find(b,parent);

    if(a==b)
        return;

    if(rank[a]<rank[b])
        std::swap(a,b);

    parent[b]=a;

    if(rank[a]==rank[b])
        rank[a]++;
}

int Graph::MST(std::vector<std::pair<int,int>>& mstEdges)
{
    mstEdges.clear();

    struct Edge
    {
        int u,v,w;
    };

    std::vector<Edge> edges;

    for(int u=0;u<graph.size();u++)
    {
        for(auto edge:graph[u])
        {
            int v=edge.first;
            int w=edge.second;

            if(u<v)
                edges.push_back({u,v,w});
        }
    }

    std::sort(edges.begin(),edges.end(),
              [](Edge a,Edge b)
              {
                  return a.w<b.w;
              });

    std::vector<int> parent(graph.size());

    std::vector<int> rank(graph.size(),0);

    for(int i=0;i<graph.size();i++)
        parent[i]=i;

    int totalCost=0;

    for(auto e:edges)
    {
        if(Find(e.u,parent)!=Find(e.v,parent))
        {
            Union(e.u,e.v,parent,rank);

            mstEdges.push_back({e.u,e.v});

            totalCost+=e.w;
        }
    }

    return totalCost;
}