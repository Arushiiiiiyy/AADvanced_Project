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
from collections import defaultdict

app = Flask(__name__)

# Global storage for generated networks
networks = {}
simulation_results = {}

def load_or_generate_networks():
    """Load or generate small-world networks"""
    global networks
    
    if not networks:
        # Generate small networks for visualization
        networks['small_world'] = nx.watts_strogatz_graph(50, 4, 0.1)
        networks['random'] = nx.erdos_renyi_graph(50, 4/49)
        networks['scale_free'] = nx.barabasi_albert_graph(50, 2)
    
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

def simulate_sir_model(G, beta=0.3, gamma=0.1, steps=50):
    """Simulate disease spread (SIR model)"""
    N = G.number_of_nodes()
    
    # States: 0=Susceptible, 1=Infected, 2=Recovered
    state = [0] * N
    
    # Infect patient zero
    patient_zero = random.randint(0, N-1)
    state[patient_zero] = 1
    
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

def simulate_threshold_model(G, threshold=0.3, initial_adopters=3):
    """Simulate social influence (threshold model)"""
    N = G.number_of_nodes()
    adopted = [False] * N
    
    # Random initial adopters
    seeds = random.sample(range(N), initial_adopters)
    for seed in seeds:
        adopted[seed] = True
    
    timeline = []
    changed = True
    step = 0
    
    while changed and step < 50:
        adopter_count = sum(adopted)
        timeline.append({
            'step': step,
            'adopters': adopter_count,
            'percentage': round(adopter_count / N * 100, 2)
        })
        
        changed = False
        new_adopted = adopted.copy()
        
        for node in range(N):
            if not adopted[node]:
                neighbors = list(G.neighbors(node))
                if neighbors:
                    adopted_neighbors = sum(adopted[n] for n in neighbors)
                    fraction = adopted_neighbors / len(neighbors)
                    
                    if fraction >= threshold:
                        new_adopted[node] = True
                        changed = True
        
        adopted = new_adopted
        step += 1
    
    return timeline

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
