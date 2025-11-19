import networkx as nx
import random
import os
import csv

# --- 1. Define Personality Tagging Function ---
def add_personality_tags(G):
    interests = ['Cricket', 'Books', 'Coding', 'Music', 'Travel', 'Art', 'Gaming']
    for node in G.nodes():
        G.nodes[node]['Interest'] = random.choice(interests)
        G.nodes[node]['Extraversion'] = round(random.random(), 2)
    return G

# --- 2. Save Graph to Text Files ---
def save_graph_to_text_files(G, base_filename):
    edgelist_file = f"{base_filename}_edges.txt"
    G_int = nx.convert_node_labels_to_integers(G, first_label=0)
    nx.write_edgelist(G_int, edgelist_file, data=False)

    nodes_file = f"{base_filename}_nodes.csv"
    headers = ['Node_ID', 'Interest', 'Extraversion']
    with open(nodes_file, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerow(headers)
        for node, attrs in G_int.nodes(data=True):
            writer.writerow([node, attrs['Interest'], attrs['Extraversion']])
    return edgelist_file, nodes_file

# --- 3. Set Graph Parameters ---
N = 1000
print(f"Generating synthetic datasets for {N} nodes...")

# --- Sparse ---
p_sparse = 0.001
G_sparse = nx.gnp_random_graph(N, p_sparse, seed=42)
G_sparse = add_personality_tags(G_sparse)
save_graph_to_text_files(G_sparse, "sparse_network")

# --- Dense ---
p_dense = 0.1
G_dense = nx.gnp_random_graph(N, p_dense, seed=42)
G_dense = add_personality_tags(G_dense)
save_graph_to_text_files(G_dense, "dense_network")

# --- Scale-Free ---
G_scale_free = nx.barabasi_albert_graph(N, 3, seed=42)
G_scale_free = add_personality_tags(G_scale_free)
save_graph_to_text_files(G_scale_free, "scale_free_network")

# --- Small-World ---
G_small_world = nx.watts_strogatz_graph(N, 10, 0.05, seed=42)
G_small_world = add_personality_tags(G_small_world)
save_graph_to_text_files(G_small_world, "small_world_network")

print("\nAll datasets generated and saved as text files.")
