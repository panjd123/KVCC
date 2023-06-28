#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "common.hpp"
#include "kvcc.hpp"
#include "third-party/argparse/argparse.hpp"
using namespace std;

argparse::ArgumentParser program("kvcc_baseline");
Timer timer;
fstream fout;

int main(int argc, char** argv) {
    timer.tic("program");
    ios::sync_with_stdio(false);
    program.add_argument("-g", "--graph").default_value(string("dblp"));
    program.add_argument("-k", "--k").default_value(35).action([](const string& value) { return stoi(value); });
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