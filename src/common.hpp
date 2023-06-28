#pragma once
#include <algorithm>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;
using namespace std::chrono;
using tp = time_point<system_clock>;
const int INF = 0x3f3f3f3f;

struct Edge {
    int from, to, flow, capacity, rev;
    Edge() {}
    Edge(int from, int to, int capacity, int rev)
        : from(from), to(to), flow(0), capacity(capacity), rev(rev) {}
};

using pii = pair<int, int>;
struct HashPair {
    size_t operator()(const pii& p) const {
        uint64_t x = p.first * 1000000000LL + p.second;
        return hash<uint64_t>()(x);
    }
};

struct Graph {
    int n;
    vector<vector<Edge>> edges;
    vector<int> id;
    unordered_set<pii, HashPair> edgeSet;
    unordered_set<int> strongSideVertex;
    void init(int n) {
        this->n = n;
        edges.assign(n, vector<Edge>());
    }
    Graph(int n = 0) {
        init(n);
    }
    void genEdgeSet() {
        edgeSet.clear();
        for (int i = 0; i < n; i++) {
            for (auto& e : edges[i]) {
                edgeSet.insert({i, e.to});
            }
        }
    }
    void addEdge(int from, int to, int capacity = 0, int rev = 0) {
        edges[from].emplace_back(from, to, capacity, rev);
    }
    void addFlowEdge(int from, int to, int capacity) {
        addEdge(from, to, capacity, edges[to].size());
        addEdge(to, from, 0, edges[from].size() - 1);
    }
    int minDegVertex() {
        int mn = INF, idx = -1;
        for (int i = 0; i < n; i++) {
            if ((int)edges[i].size() < mn) {
                mn = edges[i].size();
                idx = i;
            }
        }
        return idx;
    }
};

// 计时辅助类
using tp = time_point<system_clock>;
struct Timer {
    unordered_map<string, tp> m;
    tp start, end;
    Timer() {
        start = system_clock::now();
    }
    void tic() {
        start = system_clock::now();
    }
    void tic(string s) {
        m[s] = system_clock::now();
    }
    double toc() {
        end = system_clock::now();
        duration<double> elapsed_seconds = end - start;
        start = system_clock::now();
        return elapsed_seconds.count();
    }
    double toc(string s) {
        end = system_clock::now();
        duration<double> elapsed_seconds = end - m[s];
        cout << s << " time=" << elapsed_seconds.count() << endl;
        m[s] = system_clock::now();
        return elapsed_seconds.count();
    }
};