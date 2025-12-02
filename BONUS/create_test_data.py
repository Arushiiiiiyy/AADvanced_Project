#!/usr/bin/env python3
"""
Simple test script to create sample network data
Creates a small test network without requiring NetworkX
"""

import os

# Create output directory
OUTPUT_DIR = "small_world_analysis_data"
os.makedirs(OUTPUT_DIR, exist_ok=True)

print("=" * 60)
print("Creating Sample Test Network")
print("=" * 60)
print()

# Create a simple small-world-like network (20 nodes, ring with shortcuts)
N = 20
edges = []

# Create ring lattice
for i in range(N):
    # Connect to next 2 neighbors on each side
    edges.append((i, (i + 1) % N))
    edges.append((i, (i + 2) % N))

# Add some shortcuts (small-world property)
shortcuts = [(0, 10), (5, 15), (3, 13), (7, 17)]
edges.extend(shortcuts)

# Remove duplicates
edges = list(set(edges))

# Build adjacency list
adj = {i: [] for i in range(N)}
for u, v in edges:
    adj[u].append(v)
    adj[v].append(u)

# Save as text file
filename = os.path.join(OUTPUT_DIR, "data_proof_WS.txt")
with open(filename, 'w') as f:
    f.write(f"{N}\n")
    for node in range(N):
        neighbors = sorted(adj[node])
        f.write(f"{node}: {' '.join(map(str, neighbors))}\n")

print(f"✓ Created: {filename}")
print(f"  Nodes: {N}")
print(f"  Edges: {len(edges)}")
print()

# Create a random-like network
filename_er = os.path.join(OUTPUT_DIR, "data_proof_ER.txt")
import random
random.seed(42)

adj_er = {i: [] for i in range(N)}
for i in range(N):
    for j in range(i + 1, N):
        if random.random() < 0.15:  # Random connection probability
            adj_er[i].append(j)
            adj_er[j].append(i)

with open(filename_er, 'w') as f:
    f.write(f"{N}\n")
    for node in range(N):
        neighbors = sorted(adj_er[node])
        f.write(f"{node}: {' '.join(map(str, neighbors))}\n")

print(f"✓ Created: {filename_er}")
print(f"  Nodes: {N}")
print()

# Create a hub-based network (scale-free-like)
filename_ba = os.path.join(OUTPUT_DIR, "data_proof_BA.txt")
adj_ba = {i: [] for i in range(N)}

# Node 0 is a major hub
hub_nodes = [0, 5, 10, 15]
for hub in hub_nodes:
    for i in range(N):
        if i != hub and random.random() < (0.5 if hub == 0 else 0.3):
            if i not in adj_ba[hub]:
                adj_ba[hub].append(i)
                adj_ba[i].append(hub)

# Add some regular connections
for i in range(N):
    for j in range(i + 1, N):
        if random.random() < 0.05:
            if j not in adj_ba[i]:
                adj_ba[i].append(j)
                adj_ba[j].append(i)

with open(filename_ba, 'w') as f:
    f.write(f"{N}\n")
    for node in range(N):
        neighbors = sorted(adj_ba[node])
        f.write(f"{node}: {' '.join(map(str, neighbors))}\n")

print(f"✓ Created: {filename_ba}")
print(f"  Nodes: {N}")
print()

print("=" * 60)
print("✓ Sample networks created successfully!")
print("=" * 60)
print()
print("Network files saved in:", OUTPUT_DIR)
print()
print("Next steps:")
print("  1. Test C++ analyzer: ./network_analyzer small_world_analysis_data/data_proof_WS.txt")
print("  2. Test disease spread: ./disease_spread small_world_analysis_data/data_proof_WS.txt 0.3 0.1 5")
print("  3. Launch web dashboard: python3 web_dashboard.py")
print()
