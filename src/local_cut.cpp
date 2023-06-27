#include <bits/stdc++.h>
#include "common.hpp"
#include "cut.hpp"
#include "third-party/argparse/argparse.hpp"
using namespace std;

argparse::ArgumentParser program("local_cut");
Timer timer;
fstream fout;

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    program.add_argument("-g", "--graph").default_value(string("tiny"));
    program.add_argument("-s", "--source").default_value(0);
    program.add_argument("-t", "--sink").default_value(6);
    program.add_argument("-o", "--output").default_value(string("/dev/fd/1"));
    program.parse_args(argc, argv);

    timer.tic("input");
    int n, m, source, sink;
    string filename = program.get<string>("--graph");
    string path = "../dataset/" + filename + ".txt";
    fstream fin(path);
    source = program.get<int>("--source");
    sink = program.get<int>("--sink");
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
    auto cut = calLocolCut(graph, source, sink);
    timer.toc("local_cut");

    fstream fout(program.get<string>("--output"), ios::out);
    fout << "cut size: " << cut.size() << endl;
    for (auto v : cut) {
        fout << v << endl;
    }
}