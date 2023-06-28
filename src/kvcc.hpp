#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <queue>
#include <unordered_set>
#include <vector>
#include "common.hpp"
#include "cut.hpp"

// 用于检查KVCC的正确性
void graphCheck(Graph& graph, string_view msg = "") {
    cerr << msg << endl;
    for (int i = 0; i < graph.n; i++) {
        unordered_set<int> st;
        for (auto& e : graph.edges[i]) {
            if (e.to == i) {
                cerr << "self loop: " << i << endl;
                assert(false);
            }
            if (st.count(e.to)) {
                cerr << "multi edge: " << i << " " << e.to << endl;
                assert(false);
            }
            st.insert(e.to);
        }
    }
    cerr << "graph check pass" << endl;
}

// 输出图的信息
void graphInfo(Graph& graph) {
    cerr << "graph info: " << endl;
    cerr << "n: " << graph.n << endl;
    int m = 0;
    vector<int> deg(graph.n);
    for (int i = 0; i < graph.n; i++) {
        deg[i] = graph.edges[i].size();
        m += deg[i];
    }
    cerr << "m: " << m << endl;
    vector<int> d(graph.n);
    iota(d.begin(), d.end(), 0);
    sort(d.begin(), d.end(), [&](int i, int j) { return deg[i] < deg[j]; });
    cerr << "deg: ";
    for (int i = 0; i < 10; i++) {
        cerr << d[i] << ":" << deg[d[i]] << " ";
    }
    cerr << "... ";
    for (int i = deg.size() - 5; i < (int)deg.size(); i++) {
        cerr << d[i] << ":" << deg[d[i]] << " ";
    }
    cerr << endl;
    cerr << "-------------------------------" << endl
         << endl;
}

void kvccCheck(int k, Graph& graph, vector<int> kvcc) {
    // 是否是连通图
    queue<int> q;
    q.push(kvcc[0]);
    vector<bool> vis(graph.n, 0);
    vis[kvcc[0]] = true;
    int cnt = 1;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (auto& e : graph.edges[u]) {
            int v = e.to;
            if (!vis[v] && binary_search(kvcc.begin(), kvcc.end(), v)) {
                vis[v] = true;
                cnt++;
                q.push(v);
            }
        }
    }
    assert(cnt == (int)kvcc.size());
    // 是否是 kvcc
    Graph subGraph(kvcc.size());
    for (int i = 0; i < (int)kvcc.size(); i++) {
        for (auto& e : graph.edges[kvcc[i]]) {
            if (binary_search(kvcc.begin(), kvcc.end(), e.to)) {
                subGraph.addEdge(i, lower_bound(kvcc.begin(), kvcc.end(), e.to) - kvcc.begin());
            }
        }
    }
    auto [cut, source, sink] = calGlobalCut(subGraph);
    assert((int)cut.size() >= k || cut.empty());
}