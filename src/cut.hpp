#pragma once
#include <iostream>
#include <vector>
#include "common.hpp"
#include "dinic.hpp"

// 寻找小于等于k的最小局部割
vector<int> calLocolCut(Graph& graph, int source, int sink, int k = INF) {
    static Dinic dinic;

    // 初始化图，建图
    int n = graph.n;
    Graph& g = dinic.graph;
    g.init(n * 2);
    for (int u = 0; u < n; u++) {
        if (u != source && u != sink) {
            g.addFlowEdge(u, u + n, 1);
        } else {
            g.addFlowEdge(u, u + n, INF);
        }
    }
    for (auto& es : graph.edges) {
        for (auto& e : es) {
            g.addFlowEdge(e.from + n, e.to, INF);
        }
    }

    // 初始化dinic
    dinic.init(source, sink + n);
    auto cutE = dinic.calCut();
    if (dinic.max_flow >= k) {
        return vector<int>();
    }
    vector<int> cut;
    for (auto& e : cutE) {
        cut.push_back(e.from);
    }
    return cut;
}

// 寻找小于等于k的全局割
tuple<vector<int>, int, int> calGlobalCut(Graph& graph, int k = INF) {
    int source = graph.minDegVertex();
    // case1: 如果有直接的割
    for (int sink = 0; sink < graph.n; sink++) {
        if (sink != source) {
            auto cut = calLocolCut(graph, source, sink, k);
            if (!cut.empty()) {
                return make_tuple(cut, source, sink);
            }
        }
    }
    // case2: 两两邻居
    for (int i = 0; i < (int)graph.edges[source].size(); i++) {
        auto& e1 = graph.edges[source][i];
        int v1 = e1.to;
        for (int j = i + 1; j < (int)graph.edges[source].size(); j++) {
            auto& e2 = graph.edges[source][j];
            int v2 = e2.to;
            auto cut = calLocolCut(graph, v1, v2, k);
            if (!cut.empty()) {
                return make_tuple(cut, v1, v2);
            }
        }
    }
    return make_tuple(vector<int>(), -1, -1);
}