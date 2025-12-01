#include <bits/stdc++.h>
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
