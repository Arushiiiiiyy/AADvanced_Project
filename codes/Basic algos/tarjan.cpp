#include <bits/stdc++.h>
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
