#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
using namespace std;

/*
    Tarjan's algorithm to find Strongly Connected Components (SCCs)
    Input:
        g : directed graph, adjacency list, nodes [0..n-1]
    Output:
        vector of SCCs, each SCC is a vector<int> of node indices
*/
void tarjanDFS(int u,
               const vector<vector<int>> &g,
               vector<int> &disc,
               vector<int> &low,
               vector<bool> &inStack,
               stack<int> &st,
               int &timer,
               vector<vector<int>> &sccs) 
{
    disc[u] = low[u] = timer++;
    st.push(u);
    inStack[u] = true;

    for (int v : g[u]) {
        if (disc[v] == -1) {
           
            tarjanDFS(v, g, disc, low, inStack, st, timer, sccs);
            low[u] = min(low[u], low[v]);
        } else if (inStack[v]) {
            low[u] = min(low[u], disc[v]);
        }
    }

    if (low[u] == disc[u]) {
        vector<int> component;
        while (true) {
            int v = st.top();
            st.pop();
            inStack[v] = false;
            component.push_back(v);
            if (v == u) break;
        }
        sccs.push_back(component);
    }
}

vector<vector<int>> tarjan_scc(const vector<vector<int>> &g) {
    int n = (int)g.size();
    vector<int> disc(n, -1), low(n, -1);
    vector<bool> inStack(n, false);
    stack<int> st;
    vector<vector<int>> sccs;
    int timer = 0;

    for (int i = 0; i < n; ++i) {
        if (disc[i] == -1) {
            tarjanDFS(i, g, disc, low, inStack, st, timer, sccs);
        }
    }

    return sccs;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <edge_file>" << endl;
        return 1;
    }
    
    string filename = argv[1];
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Cannot open file " << filename << endl;
        return 1;
    }
    
    // Read edges and find max node
    vector<pair<int, int>> edges;
    int u, v, max_node = 0;
    while (file >> u >> v) {
        edges.push_back({u, v});
        max_node = max(max_node, max(u, v));
    }
    file.close();
    
    int n = max_node + 1;
    vector<vector<int>> g(n);
    
    // Build graph
    for (auto& edge : edges) {
        g[edge.first].push_back(edge.second);
    }
    
    // Find SCCs using Tarjan's algorithm
    auto sccs = tarjan_scc(g);
    
    cout << "Found " << sccs.size() << " strongly connected components:" << endl;
    for (int i = 0; i < sccs.size(); ++i) {
        cout << "SCC " << i+1 << ": ";
        for (int node : sccs[i]) {
            cout << node << " ";
        }
        cout << endl;
    }
    
    return 0;
}
