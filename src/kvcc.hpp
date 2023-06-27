#include <algorithm>
#include <vector>
#include "common.hpp"
#include "cut.hpp"

vector<Graph> getKCore(Graph graph, int k) {
    vector<Graph> kCore;
    int n = graph.n;
    int graphCnt = 0;            // 子图数量
    int nodeCnt = 0;             // 当前子图中节点数量
    vector<int> color(n, -1);    // 在第几个子图中
    vector<int> id(n, -1);       // 在子图中的编号
    vector<bool> inKCore(n, 1);  // 是否在 k-core 中
    vector<int> degree(n, 0);
    for (int i = 0; i < n; i++) {
        degree[i] = graph.edges[i].size();
    }
    queue<int> q;
    for (int i = 0; i < n; i++) {
        if (degree[i] < k) {
            q.push(i);
        }
    }
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        inKCore[u] = 0;
        for (auto& e : graph.edges[u]) {
            int v = e.to;
            degree[v]--;
            if (degree[v] == k - 1) {  // 保证每个点只会在第一次小于 k 时被 push
                q.push(v);
            }
        }
    }

    Timer timer;
    for (int i = 0; i < n; i++) {
        if (inKCore[i] && color[i] == -1) {
            timer.tic("BFS");
            vector<pair<int, int>> subGraphEdges;
            q.push(i);
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                color[u] = graphCnt;
                id[u] = nodeCnt++;
                for (auto& e : graph.edges[u]) {
                    int v = e.to;
                    if (inKCore[v] && color[v] == -1) {
                        q.push(v);
                        subGraphEdges.emplace_back(u, v);
                    }
                }
            }
            timer.toc("BFS");

            timer.tic("build subgraph");
            kCore.emplace_back(nodeCnt);
            auto& subGraph = kCore.back();
            for (auto [u, v] : subGraphEdges) {
                subGraph.addEdge(id[u], id[v]);
                subGraph.addEdge(id[v], id[u]);
            }
            graphCnt++;
            nodeCnt = 0;
            timer.toc("build subgraph");
        }
    }
    return kCore;
}