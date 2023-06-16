#pragma once
#include <vector>

#include "third-party/argparse/argparse.hpp"

struct Edge {
    int from, to, flow, capacity;
    Edge(int from, int to, int capacity)
        : from(from), to(to), flow(0), capacity(capacity) {}
};
struct Graph {
    std::vector<Edge> edges;
    std::vector<std::vector<size_t>> adj;
    Graph() {}
    Graph(int vertex_num) {
        adj.resize(vertex_num);
    }
    void add_directed_edge(int from, int to, int capacity) {
        edges.emplace_back(from, to, capacity);
        edges.emplace_back(to, from, 0);
        adj[from].push_back(edges.size() - 2);
        adj[to].push_back(edges.size() - 1);
    }
    void add_undirected_edge(int from, int to, int capacity) {
        edges.emplace_back(from, to, capacity);
        edges.emplace_back(to, from, capacity);
        adj[from].push_back(edges.size() - 2);
        adj[to].push_back(edges.size() - 1);
    }
    size_t vertex_num() const { return adj.size(); }
    size_t size() const { return adj.size(); }
    static size_t getRevId(size_t idx) { return idx ^ 1; }
    std::vector<size_t>& operator[](int idx) { return adj[idx]; }
    const std::vector<size_t>& operator[](int idx) const { return adj[idx]; }
};

Graph generate_graph(int vertex_num, int edge_num, bool connected = false) {
    if (connected) {
        Graph G(vertex_num);
        for (int i = 0; i < vertex_num - 1; i++) {
            G.add_directed_edge(i, i + 1, 1);
        }
        for (int i = 0; i < edge_num - vertex_num + 1; i++) {
            int from = rand() % vertex_num;
            int to = rand() % vertex_num;
            int capacity = rand() % 10 + 1;
            while (from == to) {
                to = rand() % vertex_num;
            }
            G.add_directed_edge(from, to, capacity);
        }
        return G;
    } else {
        Graph G(vertex_num);
        for (int i = 0; i < edge_num; i++) {
            int from = rand() % vertex_num;
            int to = rand() % vertex_num;
            int capacity = rand() % 10 + 1;
            while (from == to) {
                to = rand() % vertex_num;
            }
            G.add_directed_edge(from, to, capacity);
        }
        return G;
    }
}

std::tuple<int, int, int, int> parse(int argc, char* argv[]) {
    argparse::ArgumentParser program("maxflowmincut");
    program.add_argument("vertex_num").default_value(10);
    program.add_argument("edge_num").default_value(30);
    program.add_argument("source").default_value(0);
    program.add_argument("sink").default_value(9);
    int vertex_num, edge_num, source, sink;
    try {
        program.parse_args(argc, argv);
        vertex_num = program.get<int>("vertex_num");
        edge_num = program.get<int>("edge_num");
        source = program.get<int>("source");
        sink = program.get<int>("sink");
    } catch (const std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        exit(0);
    }
    return std::make_tuple(vertex_num, edge_num, source, sink);
}