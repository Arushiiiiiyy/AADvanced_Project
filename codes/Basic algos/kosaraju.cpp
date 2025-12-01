#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
using namespace std;

void dfs1(int v, const vector<vector<int>> &g, vector<int> &vis, vector<int> &order) {
    vis[v] = 1;
    for (int to : g[v]) {
        if (!vis[to]) dfs1(to, g, vis, order);
    }
    order.push_back(v);  
}

void dfs2(int v, const vector<vector<int>> &gt, vector<int> &vis, vector<int> &component) {
    vis[v] = 1;
    component.push_back(v);
    for (int to : gt[v]) {
        if (!vis[to]) dfs2(to, gt, vis, component);
    }
}

vector<vector<int>> kosaraju_scc(const vector<vector<int>> &g) {
    int n = (int)g.size();
    vector<int> vis(n, 0), order;
    order.reserve(n);

    for (int i = 0; i < n; ++i) {
        if (!vis[i]) dfs1(i, g, vis, order);
    }

    vector<vector<int>> gt(n);
    for (int v = 0; v < n; ++v) {
        for (int to : g[v]) {
            gt[to].push_back(v);
        }
    }

    fill(vis.begin(), vis.end(), 0);
    vector<vector<int>> sccs;

    for (int i = n - 1; i >= 0; --i) {
        int v = order[i];
        if (!vis[v]) {
            vector<int> component;
            dfs2(v, gt, vis, component);
            sccs.push_back(component);
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
    vector<vector<int>> g(n), gt(n);
    
    // Build graph and transpose
    for (auto& edge : edges) {
        g[edge.first].push_back(edge.second);
        gt[edge.second].push_back(edge.first);
    }
    
    // Find SCCs
    auto sccs = kosaraju_scc(g);
    
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
