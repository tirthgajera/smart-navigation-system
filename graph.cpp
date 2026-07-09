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
            "Chandigarh",
            "Shimla",
            "Narkanda",
            "Rampur",
            "Kalpa",
            "Kaza",
            "Manali"
        };

    graph.assign(cities.size(), {});

    auto addEdge = [&](int u,int v,int w)
    {
        graph[u].push_back({v,w});
        graph[v].push_back({u,w});
    };

    addEdge(0,1,120);
    addEdge(1,2,65);
    addEdge(2,3,40);
    addEdge(3,4,90);
    addEdge(4,5,200);
    addEdge(5,6,180);
    addEdge(1,6,250);
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