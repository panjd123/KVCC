#include <bits/stdc++.h>
#include "common.hpp"
#include "cut.hpp"
#include "kvcc.hpp"
#include "third-party/argparse/argparse.hpp"
using namespace std;

argparse::ArgumentParser program("kvcc_baseline");
Timer timer;
fstream fout;

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    program.add_argument("-g", "--graph").default_value(string("dblp"));
    program.add_argument("-k", "--k").default_value(10).default_value(20).action([](const string& value) { return stoi(value); });
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
    cerr << "n=" << n << " m=" << m << endl;

    Graph graph(n);
    vector<pair<int, int>> rawEdges;
    vector<int> rawNodeId;           // hashId -> rawId
    unordered_map<int, int> nodeId;  // rawId -> hashId

    for (int i = 0; i < m; i++) {
        int u, v;
        fin >> u >> v;
        rawEdges.emplace_back(u, v);
        rawNodeId.emplace_back(u);
    }
    timer.toc("input");

    // 离散化
    timer.tic("preprocess");
    sort(rawNodeId.begin(), rawNodeId.end());
    auto end = unique(rawNodeId.begin(), rawNodeId.end());
    for (auto it = rawNodeId.begin(); it != end; it++) {
        nodeId[*it] = it - rawNodeId.begin();
    }
    for (auto e : rawEdges) {
        int u = nodeId[e.first], v = nodeId[e.second];
        graph.addEdge(u, v);
        graph.addEdge(v, u);
    }
    timer.toc("preprocess");

    timer.tic("kvcc");
    timer.tic("k-core");
    vector<Graph> kCore = getKCore(graph, k);
    timer.toc("k-core");
    timer.toc("kvcc");
}