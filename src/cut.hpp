#pragma once
#include <iostream>
#include <vector>
#include "common.hpp"
#include "dinic.hpp"

vector<int> calLocolCut(Graph& graph, int source, int sink) {
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
    for (auto& e : graph.edges) {
        g.addFlowEdge(e.from + n, e.to, INF);
    }

    // 初始化dinic
    dinic.init(source, sink + n);
    auto cutE = dinic.calCut();
    vector<int> cut;
    for (auto& e : cutE) {
        cut.push_back(e.from);
    }
    return cut;
}

tuple<vector<int>, int, int> calGlobalCut(Graph& graph) {
    int source = graph.minDegVertex();
    // case1: 如果有直接的割
    for (int sink = 0; sink < graph.n; sink++) {
        if (sink != source) {
            auto cut = calLocolCut(graph, source, sink);
            if (!cut.empty()) {
                return make_tuple(cut, source, sink);
            }
        }
    }
    // case2: 两两邻居
    for (int i = graph.head[source]; ~i; i = graph.edges[i].next) {
        auto& e1 = graph.edges[i];
        int v1 = e1.to;
        for (int j = graph.edges[i].next; ~j; j = graph.edges[j].next) {
            auto& e2 = graph.edges[j];
            int v2 = e2.to;
            auto cut = calLocolCut(graph, v1, v2);
            if (!cut.empty()) {
                return make_tuple(cut, v1, v2);
            }
        }
    }
    return make_tuple(vector<int>(), -1, -1);
}