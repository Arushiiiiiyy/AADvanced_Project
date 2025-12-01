#!/usr/bin/env python3
"""
Enhanced algorithm runner for AADvanced Project
- Supports both generated graphs and SNAP datasets
- Excludes slow algorithms (Girvan-Newman, Eigenvector on large graphs)
- Runs C++ implementations where available
- Comprehensive benchmarking and visualization
"""

import networkx as nx
import random
import os
import csv
import subprocess
import argparse
import matplotlib.pyplot as plt
import numpy as np
import time as time_module
import pandas as pd
from pathlib import Path

# ========== CONFIGURATION ==========

# Directory structure (updated to match your repo)
CODES_DIR = "codes"
DATA_DIR = "gen_tc"
OUTPUT_DIR = "output"
SNAP_DIR = os.path.join(DATA_DIR, "snap_datasets")

# Algorithms to EXCLUDE (too slow for large graphs)
EXCLUDE_ALGOS = [
    'girvan_newman',      # O(m²n) - extremely slow
    'eigenvector',        # May not converge on some graphs
]

# Timeout for each algorithm run (seconds)
TIMEOUT = 120  # 2 minutes max per algorithm

# ========== Graph Generation Functions (from original script) ==========

def add_personality_tags(G):
    """Add random node attributes (Interest, Extraversion)"""
    interests = ['Cricket', 'Books', 'Coding', 'Music', 'Travel', 'Art', 'Gaming']
    for node in G.nodes():
        G.nodes[node]['Interest'] = random.choice(interests)
        G.nodes[node]['Extraversion'] = round(random.random(), 2)
    return G

def save_graph_to_text_files(G, base_filename, data_dir):
    """Save graph as edge list + node CSV"""
    os.makedirs(data_dir, exist_ok=True)
    edgelist_file = os.path.join(data_dir, f"{base_filename}_edges.txt")
    nodes_file = os.path.join(data_dir, f"{base_filename}_nodes.csv")

    G_int = nx.convert_node_labels_to_integers(G, first_label=0)
    nx.write_edgelist(G_int, edgelist_file, data=False)

    with open(nodes_file, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerow(['Node_ID', 'Interest', 'Extraversion'])
        for node, attrs in G_int.nodes(data=True):
            writer. writerow([node, attrs. get('Interest', 'Unknown'), attrs.get('Extraversion', 0. 5)])

    return edgelist_file, nodes_file

def generate_all_graphs(n_nodes=1000, data_dir='gen_tc'):
    """Generate 4 types of synthetic graphs"""
    print("🔧 Generating synthetic graphs...")

    graphs = {}
    
    # Sparse Graph
    G_sparse = nx.gnp_random_graph(n_nodes, 0.001, seed=42)
    G_sparse = add_personality_tags(G_sparse)
    graphs["sparse"] = save_graph_to_text_files(G_sparse, "sparse_network", data_dir)[0]

    # Dense Graph
    G_dense = nx.gnp_random_graph(n_nodes, 0.1, seed=42)
    G_dense = add_personality_tags(G_dense)
    graphs["dense"] = save_graph_to_text_files(G_dense, "dense_network", data_dir)[0]

    # Scale-Free Graph
    G_scale_free = nx.barabasi_albert_graph(n_nodes, 3, seed=42)
    G_scale_free = add_personality_tags(G_scale_free)
    graphs["scale_free"] = save_graph_to_text_files(G_scale_free, "scale_free_network", data_dir)[0]

    # Small-World Graph
    G_small_world = nx.watts_strogatz_graph(n_nodes, 10, 0.05, seed=42)
    G_small_world = add_personality_tags(G_small_world)
    graphs["small_world"] = save_graph_to_text_files(G_small_world, "small_world_network", data_dir)[0]

    print("✅ Generated 4 synthetic graphs\n")
    return graphs

# ========== SNAP Dataset Discovery ==========

def discover_snap_datasets(snap_dir):
    """Find all . txt files in SNAP directory"""
    if not os.path.exists(snap_dir):
        return {}
    
    datasets = {}
    for file in os.listdir(snap_dir):
        if file.endswith('. txt'):
            name = file.replace('.txt', '').replace('-', '_'). lower()
            datasets[f"snap_{name}"] = os.path.join(snap_dir, file)
    
    return datasets

# ========== Algorithm Execution ==========

def get_graph_stats(edges_file):
    """Quick graph statistics"""
    try:
        G = nx.read_edgelist(edges_file, nodetype=int)
        return {
            'nodes': G.number_of_nodes(),
            'edges': G.number_of_edges(),
            'density': round(nx.density(G), 6)
        }
    except:
        return {'nodes': 0, 'edges': 0, 'density': 0}

def should_skip_algorithm(algo_name, num_edges):
    """Decide if algorithm should be skipped based on graph size"""
    
    # Always skip excluded algorithms
    if any(excl in algo_name for excl in EXCLUDE_ALGOS):
        return True, f"Excluded (too slow)"
    
    # Skip betweenness on very large graphs (>50k edges)
    if 'betweenness' in algo_name and num_edges > 50000:
        return True, f"Skipped (graph too large: {num_edges} edges)"
    
    # Skip closeness on large graphs (>100k edges)
    if 'closeness' in algo_name and num_edges > 100000:
        return True, f"Skipped (graph too large: {num_edges} edges)"
    
    return False, ""

def run_cpp_algorithm(exe_path, edges_file, output_csv, timeout=TIMEOUT):
    """Run a single C++ algorithm with timeout"""
    try:
        start = time_module.perf_counter()
        result = subprocess.run(
            [exe_path, edges_file, output_csv],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout,
            text=True
        )
        elapsed = time_module.perf_counter() - start
        
        if result.returncode == 0:
            return elapsed, "Success"
        else:
            return elapsed, f"Failed (exit code {result.returncode})"
    
    except subprocess.TimeoutExpired:
        return timeout, "Timeout"
    except FileNotFoundError:
        return 0, "Binary not found"
    except Exception as e:
        return 0, f"Error: {str(e)}"

def run_python_algorithm(algo_func, edges_file, timeout=TIMEOUT):
    """Run NetworkX algorithm with timeout"""
    try:
        G = nx.read_edgelist(edges_file, nodetype=int)
        
        start = time_module.perf_counter()
        result = algo_func(G)
        elapsed = time_module.perf_counter() - start
        
        if elapsed > timeout:
            return None, elapsed, "Timeout"
        
        return result, elapsed, "Success"
    
    except Exception as e:
        return None, 0, f"Error: {str(e)}"

def save_centrality_results(values, output_csv):
    """Save centrality dict to CSV"""
    try:
        with open(output_csv, 'w', newline='', encoding='utf-8') as f:
            f.write('node,value\n')
            for node in sorted(values.keys()):
                f.write(f"{node},{values[node]}\n")
        return True
    except:
        return False

# ========== Main Benchmark Runner ==========

def run_all_benchmarks(datasets, output_dir, use_cpp=True):
    """Run all algorithms on all datasets"""
    
    os.makedirs(output_dir, exist_ok=True)
    results_dir = os.path.join(output_dir, "results")
    os.makedirs(results_dir, exist_ok=True)
    
    # Define algorithms
    centrality_algos = {
        'degree': {
            'cpp': os.path.join(CODES_DIR, 'Centrality', 'degree_centrality'),
            'python': nx.degree_centrality,
        },
        'closeness': {
            'cpp': os.path.join(CODES_DIR, 'Centrality', 'closeness_centrality'),
            'python': nx. closeness_centrality,
        },
        'betweenness': {
            'cpp': os.path.join(CODES_DIR, 'Centrality', 'betweenness_centrality'),
            'python': nx.betweenness_centrality,
        },
        'pagerank': {
            'cpp': os.path.join(CODES_DIR, 'Centrality', 'pagerank'),
            'python': nx.pagerank,
        },
        # Eigenvector excluded by default
    }
    
    community_algos = {
        'label_propagation': {
            'cpp': os.path.join(CODES_DIR, 'community', 'label_propagation'),
            'python': None,  # NetworkX version requires different handling
        },
        # Girvan-Newman excluded by default
    }
    
    graph_algos = {
        'kosaraju': {
            'cpp': os.path.join(CODES_DIR, 'Basic algos', 'kosaraju'),
            'python': None,
        },
        'tarjan': {
            'cpp': os.path.join(CODES_DIR, 'Basic algos', 'tarjan'),
            'python': None,
        },
    }
    
    all_algos = {**centrality_algos, **community_algos, **graph_algos}
    
    # Results tracking
    benchmark_results = []
    
    print("\n" + "="*70)
    print("🚀 STARTING BENCHMARK")
    print("="*70 + "\n")
    
    # Run benchmarks
    for dataset_name, edges_file in datasets.items():
        
        if not os.path.exists(edges_file):
            print(f"⚠️  Skipping {dataset_name}: file not found")
            continue
        
        stats = get_graph_stats(edges_file)
        print(f"\n📊 Dataset: {dataset_name}")
        print(f"   Nodes: {stats['nodes']:,} | Edges: {stats['edges']:,} | Density: {stats['density']}")
        print("   " + "-"*60)
        
        for algo_name, algo_info in all_algos.items():
            
            # Check if should skip
            skip, reason = should_skip_algorithm(algo_name, stats['edges'])
            if skip:
                print(f"   ⏭️  {algo_name:20s} - {reason}")
                benchmark_results.append({
                    'dataset': dataset_name,
                    'algorithm': algo_name,
                    'nodes': stats['nodes'],
                    'edges': stats['edges'],
                    'runtime_ms': -1,
                    'status': reason,
                    'implementation': 'N/A'
                })
                continue
            
            # Try C++ first (if enabled and exists)
            output_csv = os.path.join(results_dir, f"{algo_name}_{dataset_name}.csv")
            time_file = os.path.join(results_dir, f"{algo_name}_{dataset_name}_time. txt")
            
            success = False
            impl_used = "Python"
            
            if use_cpp and algo_info. get('cpp'):
                cpp_exe = algo_info['cpp']
                if os.path.exists(cpp_exe) and os.access(cpp_exe, os.X_OK):
                    runtime, status = run_cpp_algorithm(cpp_exe, edges_file, output_csv)
                    impl_used = "C++"
                    
                    if status == "Success":
                        success = True
                        print(f"   ✅ {algo_name:20s} - {runtime*1000:.2f} ms (C++)")
                        
                        # Save timing
                        with open(time_file, 'w') as f:
                            f. write(f"{runtime}\n")
                        
                        benchmark_results.append({
                            'dataset': dataset_name,
                            'algorithm': algo_name,
                            'nodes': stats['nodes'],
                            'edges': stats['edges'],
                            'runtime_ms': round(runtime * 1000, 3),
                            'status': status,
                            'implementation': impl_used
                        })
                    else:
                        print(f"   ❌ {algo_name:20s} - {status} (C++), trying Python...")
            
            # Fallback to Python (if C++ failed or not available)
            if not success and algo_info.get('python'):
                values, runtime, status = run_python_algorithm(algo_info['python'], edges_file)
                impl_used = "Python"
                
                if status == "Success" and values:
                    save_centrality_results(values, output_csv)
                    
                    with open(time_file, 'w') as f:
                        f.write(f"{runtime}\n")
                    
                    print(f"   ✅ {algo_name:20s} - {runtime*1000:.2f} ms (Python)")
                    
                    benchmark_results.append({
                        'dataset': dataset_name,
                        'algorithm': algo_name,
                        'nodes': stats['nodes'],
                        'edges': stats['edges'],
                        'runtime_ms': round(runtime * 1000, 3),
                        'status': status,
                        'implementation': impl_used
                    })
                else:
                    print(f"   ❌ {algo_name:20s} - {status}")
                    benchmark_results.append({
                        'dataset': dataset_name,
                        'algorithm': algo_name,
                        'nodes': stats['nodes'],
                        'edges': stats['edges'],
                        'runtime_ms': -1,
                        'status': status,
                        'implementation': impl_used
                    })
    
    # Save results
    df = pd.DataFrame(benchmark_results)
    results_csv = os.path.join(output_dir, "benchmark_results. csv")
    df.to_csv(results_csv, index=False)
    
    print("\n" + "="*70)
    print(f"✅ BENCHMARK COMPLETE")
    print(f"📁 Results saved to: {results_csv}")
    print("="*70 + "\n")
    
    return df

# ========== Visualization ==========

def create_visualizations(df, output_dir):
    """Generate comparison plots"""
    
    plots_dir = os.path.join(output_dir, "plots")
    os.makedirs(plots_dir, exist_ok=True)
    
    # Filter successful runs
    df_success = df[df['status'] == 'Success']. copy()
    
    if df_success.empty:
        print("⚠️  No successful runs to plot")
        return
    
    # 1. Runtime comparison by algorithm
    plt.figure(figsize=(14, 6))
    
    for algo in df_success['algorithm'].unique():
        subset = df_success[df_success['algorithm'] == algo]
        plt.plot(subset['dataset'], subset['runtime_ms'], marker='o', label=algo, linewidth=2)
    
    plt. yscale('log')
    plt. ylabel('Runtime (ms, log scale)', fontsize=12)
    plt.xlabel('Dataset', fontsize=12)
    plt.title('Algorithm Performance Comparison Across Datasets', fontsize=14, fontweight='bold')
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    plt. xticks(rotation=45, ha='right')
    plt. grid(True, alpha=0. 3)
    plt.tight_layout()
    plt.savefig(os.path.join(plots_dir, 'runtime_comparison.png'), dpi=300)
    plt.close()
    print(f"📊 Saved: plots/runtime_comparison.png")
    
    # 2. Runtime vs Graph Size
    plt.figure(figsize=(10, 6))
    
    for algo in df_success['algorithm'].unique():
        subset = df_success[df_success['algorithm'] == algo]
        plt. scatter(subset['edges'], subset['runtime_ms'], label=algo, s=100, alpha=0.6)
    
    plt.xscale('log')
    plt. yscale('log')
    plt.xlabel('Number of Edges (log scale)', fontsize=12)
    plt.ylabel('Runtime (ms, log scale)', fontsize=12)
    plt.title('Runtime Scaling with Graph Size', fontsize=14, fontweight='bold')
    plt. legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(os. path.join(plots_dir, 'scaling_analysis.png'), dpi=300)
    plt.close()
    print(f"📊 Saved: plots/scaling_analysis.png")
    
    # 3. Implementation comparison (C++ vs Python)
    if 'implementation' in df_success. columns:
        impl_comparison = df_success. groupby(['algorithm', 'implementation'])['runtime_ms'].mean().unstack(fill_value=0)
        
        if not impl_comparison.empty:
            plt.figure(figsize=(10, 6))
            impl_comparison.plot(kind='bar', ax=plt.gca())
            plt.ylabel('Average Runtime (ms)', fontsize=12)
            plt.xlabel('Algorithm', fontsize=12)
            plt.title('C++ vs Python Implementation Comparison', fontsize=14, fontweight='bold')
            plt.xticks(rotation=45, ha='right')
            plt.legend(title='Implementation')
            plt.tight_layout()
            plt.savefig(os.path.join(plots_dir, 'implementation_comparison. png'), dpi=300)
            plt.close()
            print(f"📊 Saved: plots/implementation_comparison.png")
    
    print()

# ========== Main Function ==========

def main():
    parser = argparse.ArgumentParser(
        description="Run centrality experiments on generated + SNAP datasets",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run all benchmarks (generate synthetic + use SNAP datasets)
  python run_all_algos_enhanced.py
  
  # Skip generation, only use existing datasets
  python run_all_algos_enhanced.py --skip-generate
  
  # Use only Python implementations (no C++)
  python run_all_algos_enhanced.py --no-cpp
  
  # Use only SNAP datasets
  python run_all_algos_enhanced.py --snap-only
        """
    )
    
    parser.add_argument('--skip-generate', action='store_true',
                        help='Skip generating synthetic graphs')
    parser.add_argument('--snap-only', action='store_true',
                        help='Only run on SNAP datasets (skip synthetic)')
    parser.add_argument('--no-cpp', action='store_true',
                        help='Only use Python implementations (skip C++)')
    
    args = parser.parse_args()
    
    # Collect all datasets
    all_datasets = {}
    
    # 1. Generated graphs (unless snap-only)
    if not args.snap_only:
        if args.skip_generate:
            # Look for existing files
            for name in ["sparse", "dense", "scale_free", "small_world"]:
                path = os.path.join(DATA_DIR, f"{name}_network_edges.txt")
                if os. path.exists(path):
                    all_datasets[name] = path
        else:
            # Generate new graphs
            all_datasets. update(generate_all_graphs(n_nodes=1000, data_dir=DATA_DIR))
    
    # 2.  SNAP datasets
    snap_datasets = discover_snap_datasets(SNAP_DIR)
    if snap_datasets:
        print(f"📦 Found {len(snap_datasets)} SNAP datasets:")
        for name in snap_datasets. keys():
            print(f"   - {name}")
        print()
        all_datasets.update(snap_datasets)
    else:
        print("⚠️  No SNAP datasets found.  Run download_snap_datasets.sh first!")
        print()
    
    if not all_datasets:
        print("❌ No datasets available.  Exiting.")
        return
    
    # Build C++ algorithms
    if not args.no_cpp:
        print("🔨 Building C++ algorithms...")
        try:
            subprocess.run(['bash', 'build. sh'], cwd=os.path.join(CODES_DIR, 'Centrality'), check=False)
            subprocess.run(['bash', 'build.sh'], cwd=os. path.join(CODES_DIR, 'community'), check=False)
            print("✅ Build complete\n")
        except Exception as e:
            print(f"⚠️  Build failed: {e}\n")
    
    # Run benchmarks
    df = run_all_benchmarks(all_datasets, OUTPUT_DIR, use_cpp=not args.no_cpp)
    
    # Generate visualizations
    create_visualizations(df, OUTPUT_DIR)
    
    # Print summary
    print("📊 SUMMARY STATISTICS")
    print("="*70)
    print(df.groupby('status'). size())
    print("\n✅ Fastest algorithm per dataset:")
    
    df_success = df[df['status'] == 'Success']
    for dataset in df_success['dataset'].unique():
        subset = df_success[df_success['dataset'] == dataset]
        if not subset.empty:
            fastest = subset.nsmallest(1, 'runtime_ms').iloc[0]
            print(f"   {dataset:25s} → {fastest['algorithm']:20s} ({fastest['runtime_ms']:. 2f} ms)")
    
    print("\n🎉 All done!\n")

if __name__ == "__main__":
    main()
