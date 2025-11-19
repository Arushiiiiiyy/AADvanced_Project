import time
import psutil
import os
import subprocess
import pandas as pd
import networkx as nx
from networkx.algorithms.community.quality import modularity

graphs = {
    "sparse": "sparse_network_edges.txt",
    "dense": "dense_network_edges.txt",
    "scale_free": "scale_free_network_edges.txt",
    "small_world": "small_world_network_edges.txt",
}

EXEC = "community/"   # folder where the C++ binaries live

algorithms = {
    "girvan_newman": EXEC + "girwan_newman",      # your binary name
    "label_propagation": EXEC + "label_propagation",
}

rows = []

for gname, fpath in graphs.items():
    G = nx.read_edgelist(fpath, nodetype=int)

    for algo_name, exe in algorithms.items():
        print(f"⏳ Running {algo_name} on {gname}...")

        # Start child process
        start_time = time.perf_counter()
        proc = subprocess.Popen([exe, fpath])
        child = psutil.Process(proc.pid)

        peak_mem_mb = 0.0

        # Poll while running, sample memory
        while proc.poll() is None:
            try:
                mem_mb = child.memory_info().rss / (1024 ** 2)
                if mem_mb > peak_mem_mb:
                    peak_mem_mb = mem_mb
            except psutil.NoSuchProcess:
                break
            time.sleep(0.01)  # 10 ms sampling

        end_time = time.perf_counter()
        time_ms = (end_time - start_time) * 1000.0

        # --- Read communities from community_output.txt (one line per community) ---
        communities = []
        try:
            with open("community_output.txt", "r") as f:
                for line in f:
                    line = line.strip()
                    if not line:
                        continue
                    nodes = list(map(int, line.split()))
                    communities.append(set(nodes))
        except FileNotFoundError:
            print(f"⚠️ community_output.txt not found for {algo_name} on {gname}")
            communities = []

        num_comms = len(communities)
        mod = modularity(G, communities) if num_comms > 0 else 0.0

        rows.append([
            gname,
            algo_name,
            round(time_ms, 3),
            round(peak_mem_mb, 3),
            num_comms,
            round(mod, 4),
        ])

# Save to CSV
df = pd.DataFrame(rows, columns=[
    "Graph", "Algorithm", "Time_ms", "Memory_MB", "Communities", "Modularity"
])

os.makedirs("results", exist_ok=True)
df.to_csv("results/community_results.csv", index=False)

print("\n🔥 SAVED → results/community_results.csv\n")
