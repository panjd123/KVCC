#include <bits/stdc++.h>
#include "common.hpp"
#include "cut.hpp"
#include "third-party/argparse/argparse.hpp"
using namespace std;

argparse::ArgumentParser program("global_cut");
Timer timer;
fstream fout;

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    program.add_argument("-g", "--graph").default_value(string("tiny"));
    program.add_argument("-o", "--output").default_value(string("/dev/fd/1"));
    program.parse_args(argc, argv);

    timer.tic("input");
    int n, m;
    string filename = program.get<string>("--graph");
    string path = "../dataset/" + filename + ".txt";
    fstream fin(path);
    fin >> n >> m;

    Graph graph(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        fin >> u >> v;
        graph.addEdge(u, v);
        graph.addEdge(v, u);
    }
    timer.toc("input");

    timer.tic("global_cut");
    auto [cut, source, sink] = calGlobalCut(graph);
    timer.toc("global_cut");

    fout.open(program.get<string>("--output"), ios::out);
    fout << "source: " << source << endl;
    fout << "sink: " << sink << endl;
    fout << "cut size: " << cut.size() << endl;
    for (auto v : cut) {
        fout << v << endl;
    }
}