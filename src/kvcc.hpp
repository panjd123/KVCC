#include <algorithm>
#include <vector>
#include "common.hpp"
#include "cut.hpp"

vector<Graph> kCore(Graph graph, int k) {
    vector<Graph> kCore;
    int n = graph.n;
    vector<int> degree(n, 0);
    for (auto e : graph.edges) {
        degree[e.to]++;
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
        for (int i = graph.head[u]; ~i; i = graph.edges[i].next) {
            int v = graph.edges[i].to;
            degree[v]--;
            if (degree[v] == k - 1) {  // 保证每个点只会被加入一次
                q.push(v);
            }
        }
    }
}