import subprocess, time, psutil, os, pandas as pd
import networkx as nx

graphs = {
    "sparse": "sparse_network_edges.txt",
    "dense": "dense_network_edges.txt",
    "scale_free": "scale_free_network_edges.txt",
    "small_world": "small_world_network_edges.txt"
}
EXEC_PATH = "centrailty/"

algorithms = {
    "degree": EXEC_PATH + "degree_centrality",
    "closeness": EXEC_PATH + "closeness_centrality",
    "betweenness": EXEC_PATH + "betweenness_centrality",
    "eigenvector": EXEC_PATH + "eigenvector_centrality",
    "pagerank": EXEC_PATH + "pagerank"
}


def estimate_ops(algo, V, E):
    if algo == "degree": return E
    if algo == "closeness": return V * (V + E)
    if algo == "betweenness": return V * E
    if algo == "eigenvector": return 10 * E       # approx 10 iterations
    if algo == "pagerank": return 20 * E          # approx 20 iterations

rows = []

for graph_name, file in graphs.items():
    G = nx.read_edgelist(file, nodetype=int)
    V = G.number_of_nodes()
    E = G.number_of_edges()

    for algo, exe in algorithms.items():

        print(f"Running {algo} on {graph_name}...")

        process = psutil.Process()

        start_mem = process.memory_info().rss / (1024*1024)
        start_t = time.perf_counter()

        subprocess.run([exe, file], stdout=subprocess.DEVNULL)

        end_t = time.perf_counter()
        end_mem = process.memory_info().rss / (1024*1024)

        rows.append([
            graph_name,
            algo,
            round((end_t - start_t) * 1000, 3),     # ms
            round(end_mem - start_mem, 3),          # MB gained
            estimate_ops(algo, V, E)                # theoretical ops
        ])

df = pd.DataFrame(rows, columns=[
    "Graph", "Algorithm", "Time_ms", "Memory_MB", "Ops"
])

os.makedirs("results", exist_ok=True)
df.to_csv("results/centrality_results.csv", index=False)

print("\n🔥 SAVED → results/centrality_results.csv")
