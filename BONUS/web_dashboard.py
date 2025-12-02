#!/usr/bin/env python3
"""
Web Dashboard for Small-World Network Simulations
Flask-based interactive visualization interface
"""

from flask import Flask, render_template, jsonify, request
import networkx as nx
import json
import os
import subprocess
import random
import math
from collections import defaultdict

app = Flask(__name__)

# Global storage for generated networks
networks = {}
simulation_results = {}

def load_or_generate_networks():
    """Load or generate small-world networks"""
    global networks
    
    if not networks:
        # Generate networks for visualization (larger for better simulations)
        networks['small_world'] = nx.watts_strogatz_graph(100, 4, 0.1)
        networks['random'] = nx.erdos_renyi_graph(100, 0.06)
        networks['scale_free'] = nx.barabasi_albert_graph(100, 2)
    
    return networks

def network_to_json(G, name):
    """Convert NetworkX graph to JSON format for visualization"""
    nodes = []
    links = []
    
    # Calculate metrics for node sizing
    degree_centrality = nx.degree_centrality(G)
    
    # Create nodes
    for node in G.nodes():
        nodes.append({
            'id': node,
            'name': str(node),
            'degree': G.degree(node),
            'centrality': degree_centrality[node]
        })
    
    # Create edges
    for edge in G.edges():
        links.append({
            'source': edge[0],
            'target': edge[1]
        })
    
    return {'nodes': nodes, 'links': links}

def calculate_metrics(G):
    """Calculate network metrics"""
    try:
        if nx.is_connected(G):
            L = nx.average_shortest_path_length(G)
        else:
            largest_cc = max(nx.connected_components(G), key=len)
            subgraph = G.subgraph(largest_cc)
            L = nx.average_shortest_path_length(subgraph)
    except:
        L = float('inf')
    
    C = nx.average_clustering(G)
    
    return {
        'clustering': round(C, 4),
        'path_length': round(L, 4),
        'nodes': G.number_of_nodes(),
        'edges': G.number_of_edges(),
        'avg_degree': round(sum(dict(G.degree()).values()) / G.number_of_nodes(), 2)
    }

def simulate_sir_model(G, beta=0.3, gamma=0.1, steps=100):
    """Simulate disease spread (SIR model)"""
    N = G.number_of_nodes()
    
    # States: 0=Susceptible, 1=Infected, 2=Recovered
    state = [0] * N
    
    # Infect multiple patient zeros for faster spread
    num_initial = max(1, N // 50)
    initial_infected = random.sample(range(N), num_initial)
    for node in initial_infected:
        state[node] = 1
    
    # Track over time
    timeline = []
    
    for step in range(steps):
        S = state.count(0)
        I = state.count(1)
        R = state.count(2)
        
        timeline.append({
            'step': step,
            'susceptible': S,
            'infected': I,
            'recovered': R
        })
        
        if I == 0:
            # Pad remaining steps with final state
            for remaining in range(step + 1, min(step + 10, steps)):
                timeline.append({
                    'step': remaining,
                    'susceptible': S,
                    'infected': 0,
                    'recovered': R
                })
            break
        
        new_state = state.copy()
        
        # Spread infection
        for node in range(N):
            if state[node] == 1:  # Infected
                # Try to infect neighbors
                for neighbor in G.neighbors(node):
                    if state[neighbor] == 0 and random.random() < beta:
                        new_state[neighbor] = 1
                
                # Try to recover
                if random.random() < gamma:
                    new_state[node] = 2
        
        state = new_state
    
    return timeline

def simulate_threshold_model(G, threshold=0.3, initial_adopters=5):
    """Simulate social influence (threshold model)
    
    This models how information/behavior spreads through a network.
    A node adopts when the fraction of its neighbors who adopted >= threshold.
    
    Different networks show different dynamics:
    - Small-world: Local clusters adopt together, then bridge to other clusters
    - Random: More uniform spread
    - Scale-free: Hubs can trigger massive cascades
    """
    N = G.number_of_nodes()
    adopted = [False] * N
    
    # Get node degrees to seed strategically
    degrees = dict(G.degree())
    nodes_by_degree = sorted(degrees.keys(), key=lambda x: degrees[x], reverse=True)
    
    # Seed some high-degree nodes (influencers) and some random nodes
    num_seeds = max(3, N // 20)  # 5% of network
    
    # Mix of high-degree and random seeds for realistic spread
    high_degree_seeds = nodes_by_degree[:num_seeds // 2]
    remaining_nodes = [n for n in range(N) if n not in high_degree_seeds]
    random_seeds = random.sample(remaining_nodes, num_seeds - len(high_degree_seeds))
    
    seeds = high_degree_seeds + random_seeds
    for seed in seeds:
        adopted[seed] = True
    
    timeline = []
    step = 0
    max_steps = 50
    prev_count = 0
    stall_count = 0
    
    while step < max_steps:
        adopter_count = sum(adopted)
        timeline.append({
            'step': step,
            'adopters': adopter_count,
            'percentage': round(adopter_count / N * 100, 2)
        })
        
        # Check for stalling (no change for 3 steps)
        if adopter_count == prev_count:
            stall_count += 1
            if stall_count >= 3:
                break
        else:
            stall_count = 0
        prev_count = adopter_count
        
        # Stop if everyone adopted
        if adopter_count >= N:
            break
            
        new_adopted = adopted.copy()
        
        for node in range(N):
            if not adopted[node]:
                neighbors = list(G.neighbors(node))
                if neighbors:
                    adopted_neighbors = sum(1 for n in neighbors if adopted[n])
                    fraction = adopted_neighbors / len(neighbors)
                    
                    if fraction >= threshold:
                        new_adopted[node] = True
        
        adopted = new_adopted
        step += 1
    
    return timeline

def simulate_cooperation(G, temptation=1.5, rounds=50):
    """Simulate cooperation evolution using Prisoner's Dilemma on networks.
    
    Each node plays Prisoner's Dilemma with neighbors.
    Payoff matrix:
    - Both cooperate: 1, 1
    - Both defect: 0, 0  
    - Cooperator vs Defector: 0, T (temptation)
    
    Nodes copy the strategy of their most successful neighbor.
    """
    N = G.number_of_nodes()
    
    # Initial strategies: random 50% cooperators
    # True = Cooperate, False = Defect
    strategy = [random.random() < 0.5 for _ in range(N)]
    
    timeline = []
    
    for round_num in range(rounds):
        # Count cooperators
        coop_count = sum(strategy)
        timeline.append({
            'round': round_num,
            'cooperators': coop_count,
            'defectors': N - coop_count,
            'coop_percentage': round(coop_count / N * 100, 2)
        })
        
        # Calculate payoffs for each node
        payoffs = [0.0] * N
        
        for node in range(N):
            for neighbor in G.neighbors(node):
                if strategy[node] and strategy[neighbor]:
                    # Both cooperate
                    payoffs[node] += 1
                elif strategy[node] and not strategy[neighbor]:
                    # I cooperate, they defect
                    payoffs[node] += 0
                elif not strategy[node] and strategy[neighbor]:
                    # I defect, they cooperate
                    payoffs[node] += temptation
                else:
                    # Both defect
                    payoffs[node] += 0
        
        # Update strategies - copy most successful neighbor
        new_strategy = strategy.copy()
        for node in range(N):
            neighbors = list(G.neighbors(node))
            if neighbors:
                # Find best performing neighbor (including self)
                candidates = neighbors + [node]
                best = max(candidates, key=lambda x: payoffs[x])
                new_strategy[node] = strategy[best]
        
        strategy = new_strategy
    
    return timeline

def simulate_transport(G, num_routes=100):
    """Simulate transport/routing efficiency on networks.
    
    Measures:
    - Average path length for random routes
    - Hub load distribution
    - Network efficiency
    """
    N = G.number_of_nodes()
    nodes = list(G.nodes())
    
    # Calculate betweenness centrality (hub importance)
    betweenness = nx.betweenness_centrality(G)
    
    # Simulate random routes
    path_lengths = []
    hub_usage = defaultdict(int)
    
    for _ in range(num_routes):
        source = random.choice(nodes)
        target = random.choice(nodes)
        if source != target:
            try:
                path = nx.shortest_path(G, source, target)
                path_lengths.append(len(path) - 1)
                for node in path:
                    hub_usage[node] += 1
            except nx.NetworkXNoPath:
                pass
    
    # Get top hubs
    top_hubs = sorted(hub_usage.items(), key=lambda x: x[1], reverse=True)[:10]
    
    # Calculate efficiency
    try:
        efficiency = nx.global_efficiency(G)
    except:
        efficiency = 0
    
    # Hub vulnerability - remove top hub and measure impact
    if top_hubs:
        top_hub = top_hubs[0][0]
        G_removed = G.copy()
        G_removed.remove_node(top_hub)
        try:
            efficiency_after = nx.global_efficiency(G_removed)
            vulnerability = round((efficiency - efficiency_after) / efficiency * 100, 2) if efficiency > 0 else 0
        except:
            vulnerability = 0
    else:
        vulnerability = 0
    
    return {
        'avg_path_length': round(sum(path_lengths) / len(path_lengths), 2) if path_lengths else 0,
        'efficiency': round(efficiency, 4),
        'vulnerability': vulnerability,
        'hub_distribution': [{'node': h[0], 'usage': h[1]} for h in top_hubs],
        'path_length_distribution': path_lengths[:50]  # Sample for visualization
    }

def get_3d_layout(G):
    """Generate 3D coordinates for network visualization."""
    N = G.number_of_nodes()
    
    # Use spring layout in 3D
    pos_3d = nx.spring_layout(G, dim=3, seed=42)
    
    nodes_3d = []
    for node in G.nodes():
        x, y, z = pos_3d[node]
        nodes_3d.append({
            'id': node,
            'x': float(x),
            'y': float(y),
            'z': float(z),
            'degree': G.degree(node)
        })
    
    edges_3d = []
    for edge in G.edges():
        source_pos = pos_3d[edge[0]]
        target_pos = pos_3d[edge[1]]
        edges_3d.append({
            'source': edge[0],
            'target': edge[1],
            'x0': float(source_pos[0]), 'y0': float(source_pos[1]), 'z0': float(source_pos[2]),
            'x1': float(target_pos[0]), 'y1': float(target_pos[1]), 'z1': float(target_pos[2])
        })
    
    return {'nodes': nodes_3d, 'edges': edges_3d}

@app.route('/')
def index():
    """Main dashboard page"""
    return render_template('dashboard.html')

@app.route('/api/networks')
def get_networks():
    """Get all network data"""
    load_or_generate_networks()
    
    result = {}
    for name, G in networks.items():
        result[name] = {
            'graph': network_to_json(G, name),
            'metrics': calculate_metrics(G)
        }
    
    return jsonify(result)

@app.route('/api/metrics')
def get_metrics():
    """Get comparative metrics"""
    load_or_generate_networks()
    
    metrics = {}
    for name, G in networks.items():
        metrics[name] = calculate_metrics(G)
    
    return jsonify(metrics)

@app.route('/api/simulate/disease', methods=['POST'])
def simulate_disease():
    """Run disease spread simulation"""
    data = request.json
    network_type = data.get('network', 'small_world')
    beta = float(data.get('beta', 0.3))
    gamma = float(data.get('gamma', 0.1))
    
    load_or_generate_networks()
    G = networks.get(network_type, networks['small_world'])
    
    timeline = simulate_sir_model(G, beta, gamma)
    
    return jsonify({
        'timeline': timeline,
        'network': network_type
    })

@app.route('/api/simulate/influence', methods=['POST'])
def simulate_influence():
    """Run social influence simulation"""
    data = request.json
    network_type = data.get('network', 'small_world')
    threshold = float(data.get('threshold', 0.3))
    
    load_or_generate_networks()
    G = networks.get(network_type, networks['small_world'])
    
    timeline = simulate_threshold_model(G, threshold)
    
    return jsonify({
        'timeline': timeline,
        'network': network_type
    })

@app.route('/api/simulate/cooperation', methods=['POST'])
def simulate_cooperation_route():
    """Run cooperation evolution simulation"""
    data = request.json
    network_type = data.get('network', 'small_world')
    temptation = float(data.get('temptation', 1.5))
    
    load_or_generate_networks()
    G = networks.get(network_type, networks['small_world'])
    
    timeline = simulate_cooperation(G, temptation)
    
    return jsonify({
        'timeline': timeline,
        'network': network_type
    })

@app.route('/api/simulate/transport', methods=['POST'])
def simulate_transport_route():
    """Run transport efficiency simulation"""
    data = request.json
    network_type = data.get('network', 'small_world')
    
    load_or_generate_networks()
    G = networks.get(network_type, networks['small_world'])
    
    result = simulate_transport(G)
    result['network'] = network_type
    
    return jsonify(result)

@app.route('/api/network3d/<network_type>')
def get_network_3d(network_type):
    """Get 3D layout for a network"""
    load_or_generate_networks()
    G = networks.get(network_type, networks['small_world'])
    
    layout_3d = get_3d_layout(G)
    
    return jsonify(layout_3d)

@app.route('/api/generate', methods=['POST'])
def generate_network():
    """Generate a new network with custom parameters"""
    data = request.json
    network_type = data.get('type', 'small_world')
    n = int(data.get('nodes', 50))
    
    if network_type == 'small_world':
        k = int(data.get('k', 4))
        p = float(data.get('p', 0.1))
        G = nx.watts_strogatz_graph(n, k, p)
    elif network_type == 'random':
        p = float(data.get('p', 0.08))
        G = nx.erdos_renyi_graph(n, p)
    elif network_type == 'scale_free':
        m = int(data.get('m', 2))
        G = nx.barabasi_albert_graph(n, m)
    else:
        return jsonify({'error': 'Invalid network type'}), 400
    
    networks[network_type] = G
    
    return jsonify({
        'graph': network_to_json(G, network_type),
        'metrics': calculate_metrics(G)
    })

if __name__ == '__main__':
    print("\n" + "="*60)
    print("🌐 Small-World Network Dashboard")
    print("="*60)
    print("\n📊 Starting web server...")
    print("🔗 Open your browser to: http://localhost:8080")
    print("⚠️  Press Ctrl+C to stop the server\n")
    
    # Create templates directory if it doesn't exist
    os.makedirs('templates', exist_ok=True)
    
    app.run(debug=True, host='0.0.0.0', port=8080)
