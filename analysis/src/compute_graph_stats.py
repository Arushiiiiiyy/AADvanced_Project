import networkx as nx
import pandas as pd

graphs = {
    "sparse": "sparse_network_edges.txt",
    "dense": "dense_network_edges.txt",
    "scale_free": "scale_free_network_edges.txt",
    "small_world": "small_world_network_edges.txt"
}

results = []

for name, path in graphs.items():

    print(f"Loading {name} graph...")
    G = nx.read_edgelist(path, nodetype=int)

    num_nodes = G.number_of_nodes()
    num_edges = G.number_of_edges()
    avg_degree = 2 * num_edges / num_nodes
    density = nx.density(G)
    clustering = nx.average_clustering(G)

    try:
        diameter = nx.approximation.diameter(G)
    except:
        diameter = "DISCONNECTED"

    results.append([name, num_nodes, num_edges, avg_degree, density, clustering, diameter])

df = pd.DataFrame(
    results,
    columns=["Graph", "Nodes", "Edges", "Avg Degree", "Density", "Clustering", "Diameter"]
)

df.to_csv("results/graph_stats.csv", index=False)
print("\n📁 Saved → results/graph_stats.csv")
