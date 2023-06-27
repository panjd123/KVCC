#pragma once
#include <algorithm>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;
using namespace std::chrono;
using tp = time_point<system_clock>;
const int INF = 0x3f3f3f3f;

struct Edge {
    int from, to, flow, capacity, next;
    Edge() {}
    Edge(int from, int to, int capacity, int next)
        : from(from), to(to), flow(0), capacity(capacity), next(next) {}
};

struct Graph {
    int n;
    vector<int> head;
    vector<int> degree;
    vector<Edge> edges;
    void init(int n) {
        this->n = n;
        head.assign(n, -1);
        degree.assign(n, 0);
        edges.clear();
    }
    Graph(int n = 0) {
        init(n);
    }
    void addEdge(int from, int to, int capacity = 0) {
        edges.emplace_back(from, to, capacity, head[from]);
        head[from] = edges.size() - 1;
    }
    void addFlowEdge(int from, int to, int capacity) {
        addEdge(from, to, capacity);
        addEdge(to, from, 0);
    }
    void calDeg() {
        for (auto e : edges) {
            degree[e.to]++;
        }
    }
    int minDegVertex() {
        calDeg();
        return min_element(degree.begin(), degree.end()) - degree.begin();
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
        cout << s << " time: " << elapsed_seconds.count() << endl;
        m[s] = system_clock::now();
        return elapsed_seconds.count();
    }
};