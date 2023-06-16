#include <bits/stdc++.h>
#include "third-party/argparse/argparse.hpp"
using namespace std;
using namespace std::chrono;
const int N = 1005;
const int M = 100005;
const int INF = 0x3f3f3f3f;

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
        cout << s << ": " << elapsed_seconds.count() << endl;
        m[s] = system_clock::now();
        return elapsed_seconds.count();
    }
};

struct Edge {
    int from, to, flow, capacity, next;
    Edge() {}
    Edge(int from, int to, int capacity, int next)
        : from(from), to(to), flow(0), capacity(capacity), next(next) {}
};

struct Graph {
    int n;
    int cnt;
    int head[N];
    int du[N];
    Edge edges[M];
    Graph(int n = 0)
        : n(n), cnt(1) {
        memset(head, 0, sizeof(head));
        memset(du, 0, sizeof(du));
    }
    void addEdge(int from, int to, int capacity = 0) {
        edges[++cnt] = Edge(from, to, capacity, head[from]);
        head[from] = cnt;
    }
    void addFlowEdge(int from, int to, int capacity) {
        addEdge(from, to, capacity);
        addEdge(to, from, 0);
    }
    void calDeg() {
        if (du[0])
            return;
        for (int i = 0; i < n; i++) {
            for (int j = head[i]; j; j = edges[j].next) {
                du[edges[j].to]++;
            }
        }
    }
    int minDegVertex() {
        calDeg();
        return min_element(du, du + n) - du;
    }
};

struct Dinic {
    int depth[N];
    int cur[N];
    Graph graph;
    int source, sink;
    int max_flow;
    bool vis[N];
    Dinic(int source, int sink) {
        this->source = source;
        this->sink = sink;
        max_flow = 0;
    }
    int bfs() {
        fill(depth, depth + N, 0);
        copy(graph.head, graph.head + N, cur);
        queue<int> q;
        q.push(source);
        depth[source] = 1;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int i = graph.head[u]; i; i = graph.edges[i].next) {
                auto& e = graph.edges[i];
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
        for (int& i = cur[u]; i; i = graph.edges[i].next) {
            auto& e = graph.edges[i];
            if (e.flow < e.capacity && depth[e.to] == depth[u] + 1) {
                int f = dfs(e.to, min(flow_in - flow_out, e.capacity - e.flow));
                e.flow += f;
                graph.edges[i ^ 1].flow -= f;
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
        for (int i = graph.head[u]; i; i = graph.edges[i].next) {
            auto& e = graph.edges[i];
            if (e.flow < e.capacity && !vis[e.to]) {
                dfsCut(e.to);
            }
        }
    }
    vector<Edge> getCut() {
        if (max_flow >= INF) {
            return vector<Edge>();
        }
        fill(vis, vis + N, false);
        dfsCut(source);
        vector<Edge> cut;
        for (int i = 1; i <= graph.cnt; i++) {
            auto e = graph.edges[i];
            if (vis[e.from] && !vis[e.to] && e.capacity > 0) {
                cut.push_back(e);
            }
        }
        return cut;
    }
    vector<Edge> calCut() {
        solve();
        return getCut();
    }
};

Dinic buildLocalCutDinicGraph(Graph& graph, int source, int sink) {
    int n = graph.n;
    Dinic dinic(source, sink + n);
    for (int u = 0; u < n; u++) {
        if (u != source && u != sink) {
            dinic.graph.addFlowEdge(u, u + n, 1);
        } else {
            dinic.graph.addFlowEdge(u, u + n, INF);
        }
    }
    for (int i = 1; i <= graph.cnt; i++) {
        dinic.graph.addFlowEdge(graph.edges[i].from + n, graph.edges[i].to, INF);
    }
    return dinic;
}

vector<int> calLocolCut(Graph& graph, int source, int sink) {
    auto localCutDinic = buildLocalCutDinicGraph(graph, source, sink);
    auto cutE = localCutDinic.calCut();
    vector<int> cut;
    for (auto e : cutE) {
        cut.push_back(e.from);
    }
    return cut;
}

fstream fout;

vector<int> calGlobalCut(Graph& graph) {
    int source = graph.minDegVertex();
    for (int sink = 0; sink < graph.n; sink++) {
        if (sink != source) {
            auto cut = calLocolCut(graph, source, sink);
            if (!cut.empty()) {
                fout << source << " " << sink << endl
                     << cut.size() << endl;
                return cut;
            }
        }
    }
    for (int i = graph.head[source]; i; i = graph.edges[i].next) {
        auto& e1 = graph.edges[i];
        int v1 = e1.to;
        for (int j = graph.edges[i].next; j; j = graph.edges[j].next) {
            auto& e2 = graph.edges[j];
            int v2 = e2.to;
            auto cut = calLocolCut(graph, v1, v2);
            if (!cut.empty()) {
                fout << v1 << " " << v2 << endl
                     << cut.size() << endl;
                return cut;
            }
        }
    }
    return vector<int>();
}

argparse::ArgumentParser program("global_cut");
Timer timer;

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    program.add_argument("-g", "--graph").default_value(string("../dataset/tiny.txt"));
    program.add_argument("-o", "--output").default_value(string("/dev/fd/1"));
    program.parse_args(argc, argv);

    timer.tic("input");
    int n, m;
    fstream fin(program.get<string>("--graph"));
    fout.open(program.get<string>("--output"), ios::out);
    fin >> n >> m;

    Graph graph(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        fin >> u >> v;
        graph.addEdge(u, v);
        graph.addEdge(v, u);
    }
    timer.toc("input");

    timer.tic("local_cut");
    auto cut = calGlobalCut(graph);
    timer.toc("local_cut");
    for (auto v : cut) {
        fout << v << endl;
    }
}