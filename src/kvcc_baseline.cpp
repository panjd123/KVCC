#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <unordered_map>
#include <vector>
#include "common.hpp"
#include "cut.hpp"
#include "third-party/argparse/argparse.hpp"
using namespace std;

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

argparse::ArgumentParser program("kvcc_baseline");
Timer timer;
fstream fout;

int main(int argc, char** argv) {
    timer.tic("program");
    ios::sync_with_stdio(false);
    program.add_argument("-g", "--graph").default_value(string("dblp"));
    program.add_argument("-k", "--k").default_value(20).action([](const string& value) { return stoi(value); });
    program.add_argument("-o", "--output").default_value(string("/dev/fd/1"));
    program.parse_args(argc, argv);

    timer.tic("input");
    int n, m, k;
    k = program.get<int>("--k");
    string filename = program.get<string>("--graph");
    string path = "../dataset/" + filename + ".txt";
    fstream fin(path);
    fout.open(program.get<string>("--output"), ios::out);
    fin >> n >> m;
    cout << "graph=" << filename << endl;
    cout << "k=" << k << endl;
    cout << "n=" << n << endl;
    cout << "m=" << m << endl;

    Graph graph(n);
    vector<pair<int, int>> rawEdges;
    vector<int> rawNodeId;           // hashId -> rawId
    unordered_map<int, int> nodeId;  // rawId -> hashId

    for (int i = 0; i < m; i++) {
        int u, v;
        fin >> u >> v;
        rawEdges.emplace_back(u, v);
        rawNodeId.emplace_back(u);
        rawNodeId.emplace_back(v);
    }
    timer.toc("input");

    // 离散化
    timer.tic("preprocess");
    sort(rawNodeId.begin(), rawNodeId.end());
    auto end = unique(rawNodeId.begin(), rawNodeId.end());
    nodeId.reserve(end - rawNodeId.begin());
    for (auto it = rawNodeId.begin(); it != end; it++) {
        nodeId[*it] = it - rawNodeId.begin();
    }
    for (auto e : rawEdges) {
        int u = nodeId[e.first], v = nodeId[e.second];
        graph.addEdge(u, v);
        graph.addEdge(v, u);
    }
    graph.id = rawNodeId;
    timer.toc("preprocess");

    timer.tic("kvcc");
    auto kvcc = getKVCC(graph, k);
    timer.toc("kvcc");
    for (auto& cc : kvcc) {
        sort(cc.begin(), cc.end());
    }
    sort(kvcc.begin(), kvcc.end(), [](const vector<int>& a, const vector<int>& b) {
        return a.size() == b.size() ? a[0] < b[0] : a.size() < b.size();
    });

    cout << "kvcc num=" << kvcc.size() << endl;
    fout << "k = " << k << ", "
         << "KVCC num = " << kvcc.size() << endl;
    for (auto& cc : kvcc) {
        fout << "Node num = " << cc.size() << endl;
        for (auto& u : cc) {
            fout << u << " ";
        }
        fout << endl;
    }
    timer.toc("program");
}