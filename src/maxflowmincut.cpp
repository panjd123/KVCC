#include <iostream>
#include <queue>
#include <tuple>
#include <utility>
#include <vector>

#include "graph.hpp"

namespace dinic {
class Dinic {
   public:
    Dinic() = default;
    void solve(Graph& graph, int source, int sink);

   private:
    int bfs();
    int dfs(int v, int pre_flow);

   private:
    Graph graph;
    int source, sink;
    std::vector<int> level;
};
}  // namespace dinic

int main(int argc, char* argv[]) {
    int vertex_num, edge_num, source, sink;
    std::tie(vertex_num, edge_num, source, sink) = parse(argc, argv);
    Graph g = generate_graph(vertex_num, edge_num, true);
    dinic::Dinic dinic;
    dinic.solve(g, source, sink);
}

namespace dinic {
int Dinic::bfs() {
    std::fill(level.begin(), level.end(), -1);
    level[source] = 0;
    std::queue<int> que;
    que.push(source);
    while (!que.empty()) {
        int v = que.front();
        que.pop();
        for (auto eId : graph[v]) {
            auto e = graph.edges[eId];
            if (e.capacity > 0 && level[e.to] < 0) {
                level[e.to] = level[v] + 1;
                que.push(e.to);
            }
        }
    }
    return level[sink];
}

int Dinic::dfs(int v, int pre_flow) {
    if (v == sink || pre_flow == 0)
        return pre_flow;

    int current_flow = 0;
    for (auto eId : graph[v]) {
        auto& e = graph.edges[eId];
        auto& reve = graph.edges[Graph::getRevId(eId)];
        if (level[e.to] == level[v] + 1) {
            int next_flow = dfs(e.to, std::min(pre_flow - current_flow, e.capacity - e.flow));
            if (next_flow > 0) {
                e.flow += next_flow;
                reve.flow -= next_flow;
                current_flow += next_flow;
                if (current_flow == pre_flow) {
                    break;
                }
            }
        }
    }
    return current_flow;
}

void Dinic::solve(Graph& graph, int source, int sink) {
    this->graph = graph;
    this->source = source;
    this->sink = sink;
    this->level.resize(graph.vertex_num());

    int max_flow = 0;
    while (bfs() >= 0) {
        int flow = dfs(source, std::numeric_limits<int>::max());
        while (flow > 0) {
            max_flow += flow;
            flow = dfs(source, std::numeric_limits<int>::max());
        }
    }

    std::cout << max_flow << std::endl;
}
}  // namespace dinic