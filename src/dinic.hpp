#pragma once
#include <queue>
#include <vector>
#include "common.hpp"

struct Dinic {
    Graph graph;
    vector<int> depth;
    vector<int> cur;
    vector<bool> vis;
    int source, sink;
    int max_flow;
    void init(int source, int sink) {
        depth.resize(graph.n);
        cur.resize(graph.n);
        vis.resize(graph.n);
        this->source = source;
        this->sink = sink;
        max_flow = 0;
    }
    int bfs() {
        fill(depth.begin(), depth.end(), 0);
        fill(cur.begin(), cur.end(), 0);
        queue<int> q;
        q.push(source);
        depth[source] = 1;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (auto& e : graph.edges[u]) {
                if (e.flow < e.capacity && !depth[e.to]) {
                    depth[e.to] = depth[u] + 1;
                    q.push(e.to);
                }
            }
        }
        return depth[sink];
    }
    int dfs(int u, int flow_in) {
        if (u == sink || flow_in == 0)
            return flow_in;
        int flow_out = 0;
        for (int& i = cur[u]; i < (int)graph.edges[u].size(); i++) {
            auto& e = graph.edges[u][i];
            if (e.flow < e.capacity && depth[e.to] == depth[u] + 1) {
                int f = dfs(e.to, min(flow_in - flow_out, e.capacity - e.flow));
                e.flow += f;
                graph.edges[e.to][e.rev].flow -= f;
                flow_out += f;
                if (flow_in == flow_out)
                    break;
            }
        }
        return flow_out;
    }
    int solve() {
        while (bfs()) {
            int f = dfs(source, INF);
            max_flow += f;
        }
        return max_flow;
    }
    void dfsCut(int u) {
        vis[u] = true;
        for (auto& e : graph.edges[u]) {
            if (e.flow < e.capacity && !vis[e.to]) {
                dfsCut(e.to);
            }
        }
    }
    vector<Edge> getCut() {
        if (max_flow >= INF) {
            return vector<Edge>();
        }
        fill(vis.begin(), vis.end(), false);
        dfsCut(source);
        vector<Edge> cut;
        for (auto& es : graph.edges) {
            for (auto& e : es) {
                if (e.capacity == e.flow && e.capacity > 0 && vis[e.from] && !vis[e.to]) {
                    cut.push_back(e);
                }
            }
        }
        return cut;
    }
    vector<Edge> calCut() {
        solve();
        return getCut();
    }
};