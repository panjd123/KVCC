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

vector<vector<int>> getKVCC(Graph graph, int k, bool runnning = false) {
    static vector<vector<int>> result;
    if (!runnning) {
        result.clear();
    }
    vector<bool> vis;
    vis.assign(graph.n, 0);
    vector<Graph> kCore;
    vector<vector<int>> kvcc;
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
        inKCore[u] = false;
        for (auto& e : graph.edges[u]) {
            int v = e.to;
            degree[v]--;
            if (degree[v] == k - 1) {  // 保证每个点只会在第一次小于 k 时被 push
                q.push(v);
            }
        }
    }

    // 求 k-core 的连通分量
    for (int i = 0; i < n; i++) {
        if (inKCore[i] && color[i] == -1) {
            vector<pair<int, int>> subGraphEdges;
            vector<int> rawNodeId;
            q.push(i);
            color[i] = graphCnt;
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                id[u] = nodeCnt++;
                rawNodeId.emplace_back(graph.id[u]);
                for (auto& e : graph.edges[u]) {
                    int v = e.to;
                    if (inKCore[v] && color[v] == -1) {
                        color[v] = graphCnt;
                        q.push(v);
                    }
                    if (inKCore[v] && u < v) {
                        subGraphEdges.emplace_back(u, v);
                    }
                }
            }

            kCore.push_back(Graph(nodeCnt));
            Graph& subGraph = kCore.back();
            for (auto [u, v] : subGraphEdges) {
                subGraph.addEdge(id[u], id[v]);
                subGraph.addEdge(id[v], id[u]);
            }
            subGraph.id = rawNodeId;
            graphCnt++;
            nodeCnt = 0;
        }
    }

    // 遍历每个连通分量 subGraph，考察是否是 kvcc
    for (auto& subGraph : kCore) {
        auto [cut, source, sink] = calGlobalCut(subGraph, k);
        if (cut.empty()) {  // 是 kvcc
            result.push_back(subGraph.id);
        } else {  // 不是 kvcc，利用割点将子图继续分成多个连通分量 subSubGraph，每个分量重复调用 getKVCC
            graphCnt = 0;
            color.assign(subGraph.n, -1);
            vector<int> rawNodeId;
            unordered_set<int> cutSet(cut.begin(), cut.end());

            // 因为每次 BFS 都不会加入割点，所以子图中只有非割点到（割点+非割点）的边，而没有割点到割点的边
            // 这一部分的边每次都要加入到 KVCC 中，所以单独求
            vector<pair<int, int>> cutSubGraphEdges;
            nodeCnt = 0;
            for (auto u : cut) {
                id[u] = nodeCnt++;
                rawNodeId.emplace_back(subGraph.id[u]);
                for (auto& e : subGraph.edges[u]) {
                    int v = e.to;
                    if (u < v && cutSet.find(v) != cutSet.end()) {  // 是割点
                        cutSubGraphEdges.emplace_back(u, v);
                    }
                }
            }

            for (int i = 0; i < subGraph.n; i++) {
                if (color[i] == -1 && cutSet.find(i) == cutSet.end()) {  // 从非割点开始 BFS
                    nodeCnt = cut.size();                                // 从割点后开始编号，割点的编号总是 0 ~ cut.size()-1
                    rawNodeId.resize(cut.size());                        // 割点部分的 id 不变
                    vector<pair<int, int>> subGraphEdges;
                    q.push(i);
                    color[i] = graphCnt;
                    while (!q.empty()) {
                        int u = q.front();
                        q.pop();
                        id[u] = nodeCnt++;
                        rawNodeId.emplace_back(subGraph.id[u]);
                        for (auto& e : subGraph.edges[u]) {
                            int v = e.to;
                            if (color[v] == -1 && cutSet.find(v) == cutSet.end()) {  // 非割点才能继续 BFS
                                color[v] = graphCnt;
                                q.push(v);
                            }
                            if (u < v || cutSet.find(v) != cutSet.end()) {  // 能连通的所有点
                                subGraphEdges.emplace_back(u, v);
                            }
                        }
                    }
                    Graph subSubGraph(nodeCnt);
                    for (auto [u, v] : cutSubGraphEdges) {
                        subSubGraph.addEdge(id[u], id[v]);
                        subSubGraph.addEdge(id[v], id[u]);
                    }
                    for (auto [u, v] : subGraphEdges) {
                        subSubGraph.addEdge(id[u], id[v]);
                        subSubGraph.addEdge(id[v], id[u]);
                    }
                    subSubGraph.id = rawNodeId;
                    graphCnt++;
                    nodeCnt = 0;
                    getKVCC(subSubGraph, k, true);
                }
            }
        }
    }
    return result;
}