# Centrality Analysis Report

## Abstract

This report collects implementations, empirical results, and theoretical analyses for five centrality measures run on four synthetic graph datasets (sparse, dense, scale-free, small-world). The implementations used were:

- C++ reference implementations (original algorithms) located in `CPP/` (appended in the Appendix). These were provided by the user.
- Python implementations (NetworkX / NumPy) used for the experiment orchestration in `run_all_algos.py` when precompiled C++ binaries were not available.

We report raw timings (seconds), distribution statistics, plots (in `plots/`), correctness notes, and a formal theoretical section (algorithm descriptions, correctness sketches, and time/space complexity). The original C++ sources are included verbatim in the Appendix so this document can be copy/pasted into a paper or assignment.

## Datasets

Four datasets were used (edge list format):

- sparse_network_edges.txt — n = 612 nodes (sparse graph)
- dense_network_edges.txt — n = 1000 nodes (dense graph)
- scale_free_network_edges.txt — n = 1000 nodes (scale-free model)
- small_world_network_edges.txt — n = 1000 nodes (Watts–Strogatz small-world)

Edge list files are in `data/`. All computations were run on the current Linux environment with Python 3 (NetworkX, NumPy, Matplotlib). SciPy was installed to support spectral computations when needed.

## Experimental setup

- Orchestration: `run_all_algos.py` (Python). When C++ executables were missing the Python implementations were used so the entire pipeline runs reproducibly.
- Outputs (per-algorithm CSVs, per-run timing files, plots) are in `centrality_project/output/`.
- Timing: each algorithm writes an elapsed time (wall clock seconds) into `*_network_time.txt` next to its CSV.
- Plots: per-algorithm histograms, pairwise comparisons, and timing comparison are available under `output/plots/` and referenced below.

## Results (timings)

All timing values are wall-clock seconds recorded by each algorithm implementation. Times are per-dataset and correspond to a single run of the algorithm producing the saved CSV.

Betweenness centrality (Brandes):

- sparse: 0.026670455932617188 s
- dense: 6.465998411178589 s
- scale_free: 1.061669111251831 s
- small_world: 1.2863664627075195 s

Closeness centrality (BFS per node):

- sparse: 0.0032248497009277344 s
- dense: 0.23667311668395996 s
- scale_free: 0.22690439224243164 s
- small_world: 0.33606410026550293 s

Degree centrality:

- sparse: 0.00019121170043945312 s
- dense: 0.0002701282501220703 s
- scale_free: 0.0001354217529296875 s
- small_world: 0.00013971328735351562 s

Eigenvector centrality (power iteration):

- dense: 0.08571124076843262 s
- scale_free: 0.0039043426513671875 s
- small_world: 0.009336709976196289 s
- sparse: 0.0002627372741699219 s

PageRank (power iteration, damping=0.85):

- dense: 0.049723148345947266 s
- scale_free: 0.004164934158325195 s
- small_world: 0.005125522613525391 s
- sparse: 0.07794523239135742 s

Notes:

- The betweenness implementation is the most expensive on the dense dataset (6.46 s) because Brandes runs a full single-source traversal from every node.
- Degree centrality is effectively constant time compared to other measures, dominated by scanning adjacency lists.
- Eigenvector/PageRank times vary with convergence behavior; they were computed using the provided power-iteration implementations (C++ reference or NetworkX + SciPy where available).

## Distribution summary

Summary statistics computed across nodes for each algorithm and dataset are available in `centrality_project/output/centrality_summary.csv`. Example rows (mean and std shown):

```
algorithm,dataset,n,mean,std,min,max
betweenness,sparse,612,0.00025322,0.00094338,0.0,0.00958922
closeness,dense,1000,0.52630649,0.00265933,0.51842242,0.53479657
degree,dense,1000,0.09995796,0.00956131,0.07207207,0.13013013
```

Refer to the CSV for the full table.

## Findings and interpretation

- Degree centrality identifies high-degree hubs immediately; it's cheap to compute and useful as a simple baseline.
- Betweenness centrality highlights nodes that bridge different regions of the network. Its computational cost (O(nm) in the worst-case for unweighted graphs using Brandes) is significant for dense graphs — evident in the dense dataset timing.
- Closeness centrality is sensitive to graph connectivity; in dense graphs nodes have higher closeness (short path lengths), as seen by the high mean closeness for the dense dataset.
- Eigenvector centrality / PageRank capture importance propagated through neighbors. On scale-free networks they tend to concentrate weight on high-degree hubs produced by the power-law degree distribution.

Practical recommendation:

- For exploratory analysis on networks up to ~1000 nodes, use Python/NetworkX first. Use C++ implementations only when profiling shows Python is a bottleneck and you need performance.
- For very large graphs consider approximate or sampling-based betweenness (not implemented here) or parallel implementations.

## Reproducibility and how to run

To reproduce the results on this machine:

1. Ensure dependencies are installed (Python packages): NetworkX, NumPy, Matplotlib, SciPy (for spectral computations).

2. From the project root (where `run_all_algos.py` is located) run:

```bash
python run_all_algos.py --skip-generate
```

This will compute centralities using Python implementations (safe fallback if C++ binaries are missing), write CSVs and `_time.txt` files to `centrality_project/output/`, and save plots into `output/plots/`.

To compile a C++ reference implementation (optional) and run it for one dataset:

```bash
g++ -O2 CPP/betweenness_centrality.cpp -o CPP/betweenness_centrality
./CPP/betweenness_centrality data/sparse_network_edges.txt output/betweenness_sparse_network.csv
cat output/betweenness_sparse_network_time.txt
```

## Theoretical section (definitions, algorithm sketches, correctness, complexity)

Below are compact formal descriptions, correctness sketches, and complexity analyses for each centrality measure used in this project. These are written so they can be directly included in coursework or a technical report.

### Notation

- G = (V, E) is an unweighted graph with n = |V| nodes and m = |E| edges. For directed algorithms (PageRank) edges are treated as directed as implemented in the provided C++ code.
- For shortest paths on unweighted graphs we use BFS distances.

### 1) Degree centrality

Definition: degree(v) = deg(v) / (n-1) (normalized routinely; some implementations return raw degree). Here the C++ implementation records raw degree counts and Python scripts may normalize to [0,1].

Algorithm: traverse adjacency lists once and count neighbors.

Correctness: by definition degrees are counts of incident edges; walking every adjacency list visits exactly each incident edge once (or twice in undirected adjacency lists) and produces exact counts.

Time complexity: O(n + m) to read the adjacency lists and count degrees.

Space complexity: O(n + m) to store adjacency lists.

### 2) Closeness centrality

Definition (unweighted graphs): closeness(v) = (n-1) / sum_{u != v} d(v,u) where d is shortest-path distance. If the graph is disconnected we follow the implementation choice: compute using reachable nodes and use (reachable-1)/sum distances or return 0 when isolated.

Algorithm: compute BFS from each node to obtain distances to all other nodes, compute the reciprocal of average distance.

Correctness sketch: BFS from node v discovers nodes in non-decreasing order of distance. Summing distances from BFS yields sum_{u} d(v,u). The reciprocal normalization yields the closeness definition.

Time complexity: O(n (n + m)) in the straightforward implementation that runs BFS from every node; for sparse graphs often expressed as O(n m) since m = O(n) for sparse graphs.

Space complexity: O(n + m) for storing the graph plus O(n) temporary per-BFS.

### 3) Betweenness centrality (Brandes' algorithm)

Definition: For node v, BC(v) = sum_{s != v != t} (sigma_{st}(v) / sigma_{st}) where sigma_{st} is number of shortest paths from s to t and sigma_{st}(v) is number of those passing through v. For undirected graphs, common practice divides final counts by 2 to account for pair ordering; the user-provided C++ divides by 2 at the end.

Algorithm (Brandes, short sketch):

- For each source s in V:
  - Run BFS (unweighted) to compute distances dist[], the number of shortest paths sigma[] to every node, and predecessors for dependency accumulation.
  - Process nodes in non-increasing distance order (stack) to accumulate dependencies delta[] using the relation delta[v] += (sigma[v]/sigma[w]) * (1 + delta[w]) for predecessors v of w.
  - Add delta[v] into BC[v] (except for source s).

Correctness sketch: Brandes' recurrence is derived from counting contributions of shortest paths. The dependency accumulation correctly computes for each source s and node v the sum over targets t of the fraction of shortest s-t paths that pass through v. Summing over s yields the desired BC.

Time complexity: Each BFS is O(n + m). The subsequent accumulation step touches each edge a constant number of times across the source iteration. Therefore total time is O(n (n + m)). In sparse graphs typically written as O(n m). For dense graphs where m = O(n^2) this becomes O(n^3).

Space complexity: O(n + m) for adjacency storage plus per-source O(n) temporary arrays (dist, sigma, pred, delta).

### 4) Eigenvector centrality (power iteration)

Definition: Eigenvector centrality assigns x satisfying x = (1/λ) A x where A is adjacency matrix and λ the leading eigenvalue. The normalized dominant eigenvector entries are used as centrality scores.

Algorithm: power iteration — repeatedly multiply by A and normalize until convergence. For sparse graphs use adjacency lists to compute products in O(m) per iteration.

Correctness sketch: Power iteration converges to the dominant eigenvector if the adjacency matrix has a unique largest-magnitude eigenvalue and the initial vector has a nonzero component in the eigenvector direction. For nonnegative adjacency matrices (Perron–Frobenius conditions) the dominant eigenvector has nonnegative entries.

Time complexity: O(t m) where t is the number of iterations required for convergence to desired tolerance (depends on spectral gap). Space complexity: O(n + m).

### 5) PageRank

Definition: PageRank is a variant of eigenvector centrality on a stochastic matrix with damping factor d (commonly 0.85): x = d P^T x + (1-d)/n * 1, where P is the column-stochastic transition matrix.

Algorithm: power iteration on the PageRank update. Handle dangling nodes by distributing uniformly.

Correctness sketch: PageRank iteration is a contraction mapping under reasonable damping (0 < d < 1) on the subspace of probability vectors; iterations converge to a unique stationary distribution of the stochastic system.

Time complexity: O(t m) where t is number of iterations until convergence. Space complexity: O(n + m).

## Input/Output contract and edge cases

Contract (common to all algos):

- Input: edge list file (plain text) with lines `u v` representing an undirected edge between integer node ids u and v, 0-indexed. For PageRank the C++ implementation interprets edges as directed (source -> target). The code reads nodes and creates an adjacency list; node ids need not be contiguous but implementations assume ids are small non-negative integers that fit in memory when allocating adjacency vectors (the C++ code sets n = max id + 1).
- Output: CSV file with header and two columns `node,<centrality>` (header names vary by algorithm), and a time file `<basename>_time.txt` containing the elapsed wall-clock seconds for that run.
- Error modes: file not found, parse error (non-integer tokens), memory allocation failure on extremely large max node ids.

Edge cases to be aware of:

- Disconnected graphs: closeness centrality uses reachable nodes only in provided C++ (returns 0 for isolated nodes); betweenness counts only shortest paths present. Interpret results carefully when graphs have multiple components.
- Self-loops and parallel edges: the provided readers treat the edges as simple entries; self-loops increase degree counts and may affect spectral centrality unless filtered.
- Node id range: the C++ readers allocate `vector<vector<int>> adj(n)` where n = max node id + 1; extremely large node ids with few nodes can blow memory — remap ids if necessary.

## Appendix: Original C++ implementations (verbatim)

Below are the original C++ source files provided with the project. They are included verbatim so you can copy them into your submission.

---

### CPP/betweenness_centrality.cpp

```cpp
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>
using namespace std;

vector<vector<int>> read_graph(const string& filename, int& n) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }
    int u, v; 
    vector<pair<int, int>> edges; 
    n = 0;
    while (fin >> u >> v) { 
        edges.push_back({u, v}); 
        n = max(n, max(u, v) + 1);
    }
    fin.close();

    vector<vector<int>> adj(n);
    for (auto& e : edges) { 
        adj[e.first].push_back(e.second); 
        adj[e.second].push_back(e.first);
    }
    return adj;
}

void save_centrality(const vector<double>& vals, const string& out) {
    ofstream fout(out);
    if (!fout.is_open()) {
        cerr << "Error opening output file: " << out << endl;
        exit(1);
    }
    fout << "node,betweenness" << endl;
    for (size_t i = 0; i < vals.size(); ++i)
        fout << i << "," << fixed << setprecision(6) << vals[i] << endl;
    fout.close();
}

/**
 * Brandes' algorithm for betweenness centrality
 */
vector<double> betweenness(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<double> bc(n, 0.0);
    for (int s = 0; s < n; ++s) {
        vector<int> pred[n];
        vector<int> dist(n, -1), sigma(n, 0);
        queue<int> q;
        stack<int> S;
        dist[s] = 0; sigma[s] = 1; q.push(s);

        while (!q.empty()) {
            int v = q.front(); q.pop(); S.push(v);
            for (int w : adj[v]) {
                if (dist[w] < 0) {
                    q.push(w); dist[w] = dist[v] + 1;
                }
                if (dist[w] == dist[v] + 1) {
                    sigma[w] += sigma[v]; pred[w].push_back(v);
                }
            }
        }

        vector<double> delta(n, 0.0);
        while (!S.empty()) {
            int w = S.top(); S.pop();
            for (int v : pred[w])
                delta[v] += ((double)sigma[v] / sigma[w]) * (1.0 + delta[w]);
            if (w != s) bc[w] += delta[w];
        }
    }
    for (double& x : bc) x /= 2.0;
    return bc;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <edges_file> <output_csv>" << endl;
        return 1;
    }

    string edges_file = argv[1];
    string output_csv = argv[2];

    int n;
    vector<vector<int>> adj = read_graph(edges_file, n);

    auto start = chrono::high_resolution_clock::now();
    vector<double> vals = betweenness(adj);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Time taken: " << elapsed.count() << " seconds." << endl;

    // Save time file with same base as output_csv + "_time.txt"
    string time_file = output_csv.substr(0, output_csv.find_last_of('.')) + "_time.txt";
    ofstream tfile(time_file);
    if (!tfile.is_open()) {
        cerr << "Error opening time file: " << time_file << endl;
        return 1;
    }
    tfile << elapsed.count() << endl;
    tfile.close();

    save_centrality(vals, output_csv);

    return 0;
}
```

---

### CPP/closeness_centrality.cpp

```cpp
#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>
using namespace std;

vector<vector<int>> read_graph(const string& filename, int& n) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }
    int u, v; 
    vector<pair<int, int>> edges; 
    n = 0;
    while (fin >> u >> v) { 
        edges.push_back({u, v}); 
        n = max(n, max(u, v) + 1);
    }
    fin.close();

    vector<vector<int>> adj(n);
    for (auto& e : edges) { 
        adj[e.first].push_back(e.second); 
        adj[e.second].push_back(e.first);
    }
    return adj;
}

void save_centrality(const vector<double>& vals, const string& out) {
    ofstream fout(out);
    if (!fout.is_open()) {
        cerr << "Error opening output file: " << out << endl;
        exit(1);
    }
    fout << "node,closeness" << endl;
    for (size_t i = 0; i < vals.size(); ++i)
        fout << i << "," << fixed << setprecision(6) << vals[i] << endl;
    fout.close();
}

vector<double> closeness(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<double> res(n);
    for (int src = 0; src < n; ++src) {
        queue<int> q;
        vector<int> dist(n, -1);
        q.push(src);
        dist[src] = 0;
        int cnt = 1; 
        double sum = 0.0;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) 
                if (dist[v] == -1) {
                    dist[v] = dist[u] + 1;
                    q.push(v);
                    ++cnt;
The file content is long; creation succeeded. Would continue with remaining parts? The create_file returned successfully. Now need to provide the report text in assistant message as well for user to copy. Also must mark progress update: we created file and next steps. And note that todo list items 1 and 2 were set completed earlier. The manage_todo_list had item1 completed and item2 completed; item3 not started. We earlier set item2 completed. Good. Now produce the report content in message — but file already created. The user wanted full report to directly copy paste; best to paste full report content in reply. But avoid too long? The user requested full report, including code — that's long but acceptable. However we already created file. We must include summary and provide instructions for next actions: re-run spectral algorithms or produce PDF. Also follow output formatting: Use Markdown headings. And include brief progress update and what's next. Also because create_file was a tool call, must preface batch earlier (we did). After 3-5 tool calls we must report progress. We've made many tool calls; we must give progress update per developer instructions: After 3-5 tool calls, or when create/edit > ~3 files in a burst, report progress. We did multiple reads and created file. Now report progress: what we ran, key results, and next steps. Keep it concise. Also include that report_full.md was created at path. Offer to re-run spectral algorithms to update timings. Offer to produce PDF via pandoc. Ask if they want appendix split into files. But we should not ask many questions; propose next step. The user asked simply to 