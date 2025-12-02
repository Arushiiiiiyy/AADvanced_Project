#This script will:Generate Watts-Strogatz (WS), Erdős–Rényi (ER), and Barabási–Albert (BA) graphs.Export the graph data as Adjacency Lists for your C++ programs.Calculate $L$ and $C$ for the Small-World proof.Generate plots for the metric comparison and the WS transition (rewiring sweep)

import networkx as nx
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import os
import time

# --- Configuration and Setup ---
OUTPUT_DIR = "small_world_analysis_data"
# Create directory for saving data files and plots
os.makedirs(OUTPUT_DIR, exist_ok=True)
print(f"Output files will be saved in: {OUTPUT_DIR}\n")

# --- 1. Core Utility Functions ---

def get_network_metrics(graph):
    """Calculates Average Clustering Coefficient (C) and Average Shortest Path Length (L)."""
    # L calculation is done on the largest connected component if the graph is disconnected
    try:
        if nx.is_connected(graph):
            L = nx.average_shortest_path_length(graph)
        else:
            largest_cc = max(nx.connected_components(graph), key=len)
            subgraph = graph.subgraph(largest_cc)
            L = nx.average_shortest_path_length(subgraph)
    except nx.NetworkXNoPath:
        L = float('inf') 
        
    C = nx.average_clustering(graph)
    return C, L

def export_graph_to_adj_list(graph, topology_name, filename_prefix):
    """
    Saves the graph structure to a text file in a format readable by C++:
    Line 1: N (Total number of nodes)
    Line 2+: node_id: neighbor1 neighbor2 ...
    """
    
    filename = os.path.join(OUTPUT_DIR, f"{filename_prefix}_{topology_name.replace(' ', '_')}.txt")
    N = graph.number_of_nodes()
    
    with open(filename, 'w') as f:
        # 1. Write the total number of nodes (N)
        f.write(f"{N}\n")
        
        # 2. Write the adjacency list
        for node in range(N):
            # Sort the neighbors for consistent output
            neighbors = sorted(list(graph.neighbors(node)))
            
            # Write the node ID and its neighbors, separated by spaces
            f.write(f"{node}: {' '.join(map(str, neighbors))}\n")
            
    print(f"   -> Exported {topology_name} graph data to {filename}")

# --- 2. Data Generation Functions ---

def generate_small_world_proof_set(N, K, P_WS, M_BA):
    """Generates the three core topologies and exports them for C++ analysis."""
    print(f"--- Generating Proof Set (N={N}) and Exporting Data ---")
    data = []
    
    # 1. Watts-Strogatz (WS) - Small-World
    print("  -> Generating WS (Small-World)...")
    ws_graph = nx.watts_strogatz_graph(N, K, p=P_WS)
    export_graph_to_adj_list(ws_graph, 'WS', 'data_proof')
    C_ws, L_ws = get_network_metrics(ws_graph)
    data.append({'Topology': 'Watts-Strogatz (Small-World)', 'Avg_C': C_ws, 'Avg_L': L_ws, 'N': N})

    # 2. Erdős–Rényi (ER) - Random
    P_ER = K / (N - 1)
    print("  -> Generating ER (Random)...")
    er_graph = nx.erdos_renyi_graph(N, p=P_ER)
    export_graph_to_adj_list(er_graph, 'ER', 'data_proof')
    C_er, L_er = get_network_metrics(er_graph)
    data.append({'Topology': 'Erdős–Rényi (Random)', 'Avg_C': C_er, 'Avg_L': L_er, 'N': N})
    
    # 3. Barabási–Albert (BA) - Scale-Free
    print("  -> Generating BA (Scale-Free)...")
    ba_graph = nx.barabasi_albert_graph(N, m=M_BA)
    export_graph_to_adj_list(ba_graph, 'BA', 'data_proof')
    C_ba, L_ba = get_network_metrics(ba_graph)
    data.append({'Topology': 'Barabási–Albert (Scale-Free)', 'Avg_C': C_ba, 'Avg_L': L_ba, 'N': N})

    return pd.DataFrame(data)

def generate_rewiring_sweep_set(N, K, P_VALUES):
    """Sweeps the WS rewiring probability (p) to show the Small-World transition."""
    print(f"\n--- Running WS Rewiring Sweep (N={N}, K={K}) ---")
    sweep_data = []

    for p_val in P_VALUES:
        # Generate the graph
        graph = nx.watts_strogatz_graph(N, K, p=p_val)
        C, L = get_network_metrics(graph)
        sweep_data.append({'P_Rewiring': p_val, 'Avg_C': C, 'Avg_L': L})

    return pd.DataFrame(sweep_data)

# --- 3. Visualization Functions ---

def plot_metrics_comparison(df, title, filename):
    """Plots a bar chart comparing C and L for different topologies (The Core Proof)."""
    fig, ax = plt.subplots(1, 2, figsize=(14, 6))

    df.set_index('Topology')[['Avg_C']].plot(kind='bar', ax=ax[0], legend=False, color='skyblue')
    ax[0].set_title('Average Clustering Coefficient (C)', fontsize=14)
    ax[0].set_ylabel('C Value')
    ax[0].tick_params(axis='x', rotation=45)
    
    df.set_index('Topology')[['Avg_L']].plot(kind='bar', ax=ax[1], legend=False, color='lightcoral')
    ax[1].set_title('Average Shortest Path Length (L)', fontsize=14)
    ax[1].set_ylabel('L Value')
    ax[1].tick_params(axis='x', rotation=45)
    
    plt.suptitle(title, fontsize=16)
    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig(os.path.join(OUTPUT_DIR, filename), bbox_inches='tight', dpi=300)
    plt.show()

def plot_rewiring_sweep(df, title, filename):
    """Plots C and L vs. rewiring probability for the WS model (The Transition)."""
    fig, ax1 = plt.subplots(figsize=(10, 6))

    color = 'tab:red'
    ax1.set_xlabel('Rewiring Probability (p)', fontsize=12)
    ax1.set_ylabel('Average Shortest Path Length (L)', color=color, fontsize=12)
    ax1.plot(df['P_Rewiring'], df['Avg_L'], color=color, marker='o', label='Average L')
    ax1.tick_params(axis='y', labelcolor=color)
    ax1.set_xscale('log')

    ax2 = ax1.twinx()  # Shared X-axis
    color = 'tab:blue'
    ax2.set_ylabel('Average Clustering Coefficient (C)', color=color, fontsize=12)
    ax2.plot(df['P_Rewiring'], df['Avg_C'], color=color, marker='x', label='Average C')
    ax2.tick_params(axis='y', labelcolor=color)

    fig.tight_layout()
    plt.title(title, fontsize=16)
    fig.legend(loc="upper right", bbox_to_anchor=(1,1), bbox_transform=ax1.transAxes)
    plt.savefig(os.path.join(OUTPUT_DIR, filename), bbox_inches='tight', dpi=300)
    plt.show()

# --- 4. Main Execution ---

if __name__ == "__main__":
    
    # --- PART A: Generate Proof Data and Export for C++ ---
    N_PROOF = 1000
    K_PROOF = 6
    P_WS = 0.1     # Small-World p
    M_BA = 3       # BA m
    
    proof_df = generate_small_world_proof_set(N_PROOF, K_PROOF, P_WS, M_BA)

    # Visualize the core proof (The "Why" of the Small-World)
    plot_metrics_comparison(
        proof_df, 
        f"Small-World Phenomenon Proof (N={N_PROOF})", 
        "1_metrics_comparison_bar.png"
    )
    
    print("\n✅ Data exported successfully. Run your C++ analysis on the generated .txt files.")
    print("   Example C++ input file: small_world_analysis_data/data_proof_WS.txt\n")

    # --- PART B: Generate Data for WS Transition Visualization ---
    N_SWEEP = 500
    K_SWEEP = 4
    P_VALUES = [0.0, 0.001, 0.005, 0.01, 0.05, 0.1, 0.5, 1.0] # Logarithmic sweep
    
    sweep_df = generate_rewiring_sweep_set(N_SWEEP, K_SWEEP, P_VALUES)

    # Visualize the Small-World transition (The "How" of the Small-World)
    plot_rewiring_sweep(
        sweep_df, 
        f"Watts-Strogatz Transition: Small-World Region (N={N_SWEEP})", 
        "2_ws_rewiring_sweep_plot.png"
    )
    
    print("\n--- Summary DataFrames ---")
    print("\nProof Set Metrics:")
    print(proof_df[['Topology', 'Avg_C', 'Avg_L']].to_string(index=False))
    print("\nWS Rewiring Sweep (for plot):")
    print(sweep_df[['P_Rewiring', 'Avg_C', 'Avg_L']].to_string(index=False))