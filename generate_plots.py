#!/usr/bin/env python3
"""
Facebook Network Analysis - Plot Generation Script
Generates comprehensive visualizations from algorithm outputs
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from pathlib import Path
import networkx as nx
from collections import Counter

# Set style for better-looking plots
plt.style.use('seaborn-v0_8')
sns.set_palette("husl")

# Directories
RESULTS_DIR = Path("facebook_results")
PLOTS_DIR = Path("plots")
PLOTS_DIR.mkdir(exist_ok=True)

def load_centrality_data():
    """Load all centrality algorithm results"""
    centrality_files = {
        'degree': RESULTS_DIR / 'fb_degree_centrality.csv',
        'closeness': RESULTS_DIR / 'fb_closeness_centrality.csv', 
        'betweenness': RESULTS_DIR / 'fb_betweenness_centrality.csv',
        'eigenvector': RESULTS_DIR / 'fb_eigenvector_centrality.csv',
        'pagerank': RESULTS_DIR / 'fb_pagerank.csv'
    }
    
    data = {}
    for name, file_path in centrality_files.items():
        if file_path.exists():
            df = pd.read_csv(file_path)
            data[name] = df
            print(f"✅ Loaded {name}: {len(df)} nodes")
        else:
            print(f"❌ Missing: {file_path}")
    
    return data

def load_performance_data():
    """Load algorithm performance metrics"""
    perf_file = RESULTS_DIR / 'algorithm_performance.csv'
    if perf_file.exists():
        df = pd.read_csv(perf_file)
        print(f"✅ Loaded performance data: {len(df)} algorithms")
        return df
    else:
        print(f"❌ Performance file not found: {perf_file}")
        return None

def load_community_data():
    """Load community detection results"""
    community_file = RESULTS_DIR / 'fb_label_propagation_communities.txt'
    if community_file.exists():
        communities = []
        with open(community_file, 'r') as f:
            for i, line in enumerate(f):
                nodes = list(map(int, line.strip().split()))
                communities.append({'community_id': i, 'size': len(nodes), 'nodes': nodes})
        
        print(f"✅ Loaded {len(communities)} communities")
        return communities
    else:
        print(f"❌ Community file not found: {community_file}")
        return None

def plot_centrality_distributions(centrality_data):
    """Plot distributions of centrality values"""
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    axes = axes.flatten()
    
    colors = ['skyblue', 'lightcoral', 'lightgreen', 'plum', 'orange']
    
    for i, (name, df) in enumerate(centrality_data.items()):
        ax = axes[i]
        
        values = df.iloc[:, 1].values  # Second column contains values
        
        # Histogram
        ax.hist(values, bins=50, alpha=0.7, color=colors[i], edgecolor='black')
        ax.set_title(f'{name.title()} Centrality Distribution', fontsize=12, fontweight='bold')
        ax.set_xlabel('Centrality Value')
        ax.set_ylabel('Frequency')
        ax.grid(True, alpha=0.3)
        
        # Add statistics
        mean_val = np.mean(values)
        std_val = np.std(values)
        ax.axvline(mean_val, color='red', linestyle='--', alpha=0.8, label=f'Mean: {mean_val:.4f}')
        ax.legend()
    
    # Remove empty subplot
    if len(centrality_data) < 6:
        fig.delaxes(axes[5])
    
    plt.tight_layout()
    plt.savefig(PLOTS_DIR / 'centrality_distributions.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("📊 Saved: plots/centrality_distributions.png")

def plot_centrality_rankings(centrality_data):
    """Plot top nodes for each centrality measure"""
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    axes = axes.flatten()
    
    for i, (name, df) in enumerate(centrality_data.items()):
        ax = axes[i]
        
        # Get top 20 nodes
        top_nodes = df.nlargest(20, df.columns[1])
        
        bars = ax.bar(range(len(top_nodes)), top_nodes.iloc[:, 1], 
                     color=plt.cm.viridis(np.linspace(0, 1, len(top_nodes))))
        
        ax.set_title(f'Top 20 Nodes - {name.title()} Centrality', fontsize=12, fontweight='bold')
        ax.set_xlabel('Node Rank')
        ax.set_ylabel('Centrality Value')
        ax.set_xticks(range(0, len(top_nodes), 5))
        ax.set_xticklabels(range(1, len(top_nodes)+1, 5))
        
        # Annotate top 3
        for j in range(min(3, len(top_nodes))):
            height = bars[j].get_height()
            node_id = top_nodes.iloc[j, 0]
            ax.annotate(f'Node {node_id}', 
                       xy=(j, height), 
                       xytext=(0, 3), 
                       textcoords="offset points",
                       ha='center', va='bottom', fontsize=8)
    
    # Remove empty subplot
    if len(centrality_data) < 6:
        fig.delaxes(axes[5])
    
    plt.tight_layout()
    plt.savefig(PLOTS_DIR / 'centrality_rankings.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("📊 Saved: plots/centrality_rankings.png")

def plot_centrality_correlation(centrality_data):
    """Plot correlation between different centrality measures"""
    if len(centrality_data) < 2:
        print("⚠️ Need at least 2 centrality measures for correlation")
        return
    
    # Combine all centrality data
    combined_df = pd.DataFrame()
    for name, df in centrality_data.items():
        combined_df[name] = df.iloc[:, 1].values
    
    # Correlation matrix
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))
    
    # Heatmap
    corr_matrix = combined_df.corr()
    sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0, ax=ax1,
                square=True, fmt='.3f')
    ax1.set_title('Centrality Measures Correlation Matrix', fontsize=14, fontweight='bold')
    
    # Scatter plot of top 2 correlated measures
    if len(centrality_data) >= 2:
        names = list(centrality_data.keys())
        x_data = combined_df[names[0]]
        y_data = combined_df[names[1]]
        
        ax2.scatter(x_data, y_data, alpha=0.6, s=20)
        ax2.set_xlabel(f'{names[0].title()} Centrality')
        ax2.set_ylabel(f'{names[1].title()} Centrality')
        ax2.set_title(f'{names[0].title()} vs {names[1].title()}', fontsize=14, fontweight='bold')
        ax2.grid(True, alpha=0.3)
        
        # Add correlation coefficient
        correlation = np.corrcoef(x_data, y_data)[0, 1]
        ax2.text(0.05, 0.95, f'Correlation: {correlation:.3f}', 
                transform=ax2.transAxes, fontsize=12, 
                bbox=dict(boxstyle="round,pad=0.3", facecolor="yellow", alpha=0.7))
    
    plt.tight_layout()
    plt.savefig(PLOTS_DIR / 'centrality_correlation.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("📊 Saved: plots/centrality_correlation.png")

def plot_performance_comparison(performance_df):
    """Plot algorithm performance comparison"""
    if performance_df is None or performance_df.empty:
        print("⚠️ No performance data available")
        return
    
    successful_algos = performance_df[performance_df['status'] == 'success'].copy()
    if successful_algos.empty:
        print("⚠️ No successful algorithms to plot")
        return
    
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
    
    # Runtime comparison
    runtime_data = successful_algos.sort_values('runtime_seconds')
    bars1 = ax1.barh(runtime_data['algorithm'], runtime_data['runtime_seconds'], 
                     color=plt.cm.plasma(np.linspace(0, 1, len(runtime_data))))
    ax1.set_xlabel('Runtime (seconds)')
    ax1.set_title('Algorithm Runtime Comparison', fontweight='bold')
    ax1.grid(True, alpha=0.3)
    
    # Add runtime labels
    for i, (_, row) in enumerate(runtime_data.iterrows()):
        ax1.text(row['runtime_seconds'] + 0.01, i, f'{row["runtime_seconds"]:.3f}s', 
                va='center', fontsize=9)
    
    # Memory usage comparison
    memory_data = successful_algos.sort_values('memory_mb')
    bars2 = ax2.barh(memory_data['algorithm'], memory_data['memory_mb'],
                     color=plt.cm.viridis(np.linspace(0, 1, len(memory_data))))
    ax2.set_xlabel('Memory Usage (MB)')
    ax2.set_title('Algorithm Memory Usage', fontweight='bold')
    ax2.grid(True, alpha=0.3)
    
    # Runtime vs Memory scatter
    ax3.scatter(successful_algos['runtime_seconds'], successful_algos['memory_mb'], 
               s=100, alpha=0.7, c=range(len(successful_algos)), cmap='tab10')
    
    for _, row in successful_algos.iterrows():
        ax3.annotate(row['algorithm'], 
                    (row['runtime_seconds'], row['memory_mb']),
                    xytext=(5, 5), textcoords='offset points', fontsize=8)
    
    ax3.set_xlabel('Runtime (seconds)')
    ax3.set_ylabel('Memory Usage (MB)')
    ax3.set_title('Runtime vs Memory Usage', fontweight='bold')
    ax3.grid(True, alpha=0.3)
    
    # Algorithm category breakdown
    categories = {
        'Centrality': ['Degree Centrality', 'Closeness Centrality', 'Betweenness Centrality', 
                      'Eigenvector Centrality', 'PageRank'],
        'Community': ['Label Propagation'],
        'Shortest Path': ['Bellman-Ford', 'Dijkstra'],
        'SCC': ['Kosaraju SCC', 'Tarjan SCC']
    }
    
    category_times = {}
    for category, algos in categories.items():
        times = []
        for algo in algos:
            matching = successful_algos[successful_algos['algorithm'] == algo]
            if not matching.empty:
                times.append(matching.iloc[0]['runtime_seconds'])
        if times:
            category_times[category] = np.mean(times)
    
    if category_times:
        ax4.pie(category_times.values(), labels=category_times.keys(), autopct='%1.1f%%',
               startangle=90, colors=plt.cm.Set3(np.linspace(0, 1, len(category_times))))
        ax4.set_title('Runtime Distribution by Algorithm Type', fontweight='bold')
    
    plt.tight_layout()
    plt.savefig(PLOTS_DIR / 'performance_comparison.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("📊 Saved: plots/performance_comparison.png")

def plot_community_analysis(communities):
    """Plot community detection analysis"""
    if not communities:
        print("⚠️ No community data available")
        return
    
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
    
    # Community size distribution
    sizes = [c['size'] for c in communities]
    
    ax1.hist(sizes, bins=20, alpha=0.7, color='skyblue', edgecolor='black')
    ax1.set_xlabel('Community Size')
    ax1.set_ylabel('Number of Communities')
    ax1.set_title('Community Size Distribution', fontweight='bold')
    ax1.grid(True, alpha=0.3)
    ax1.axvline(np.mean(sizes), color='red', linestyle='--', label=f'Mean: {np.mean(sizes):.1f}')
    ax1.legend()
    
    # Top 10 largest communities
    sorted_communities = sorted(communities, key=lambda x: x['size'], reverse=True)[:10]
    community_ids = [f"C{c['community_id']}" for c in sorted_communities]
    community_sizes = [c['size'] for c in sorted_communities]
    
    bars = ax2.bar(community_ids, community_sizes, 
                   color=plt.cm.tab10(np.linspace(0, 1, len(community_sizes))))
    ax2.set_xlabel('Community ID')
    ax2.set_ylabel('Community Size')
    ax2.set_title('Top 10 Largest Communities', fontweight='bold')
    ax2.tick_params(axis='x', rotation=45)
    
    # Add size labels on bars
    for bar, size in zip(bars, community_sizes):
        ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.5,
                str(size), ha='center', va='bottom', fontsize=9)
    
    # Community size categories
    size_categories = {
        'Small (1-10)': len([s for s in sizes if 1 <= s <= 10]),
        'Medium (11-50)': len([s for s in sizes if 11 <= s <= 50]),
        'Large (51-100)': len([s for s in sizes if 51 <= s <= 100]),
        'Very Large (100+)': len([s for s in sizes if s > 100])
    }
    
    # Remove empty categories
    size_categories = {k: v for k, v in size_categories.items() if v > 0}
    
    ax3.pie(size_categories.values(), labels=size_categories.keys(), autopct='%1.1f%%',
           startangle=90, colors=plt.cm.Pastel1(np.linspace(0, 1, len(size_categories))))
    ax3.set_title('Community Size Categories', fontweight='bold')
    
    # Statistics summary
    stats_text = f"""Community Statistics:
    
Total Communities: {len(communities)}
Average Size: {np.mean(sizes):.1f}
Largest Community: {max(sizes)} nodes
Smallest Community: {min(sizes)} nodes
Std Deviation: {np.std(sizes):.1f}
    
Coverage: {sum(sizes):,} nodes
"""
    
    ax4.text(0.1, 0.9, stats_text, transform=ax4.transAxes, fontsize=11,
            verticalalignment='top', bbox=dict(boxstyle="round,pad=0.5", 
            facecolor="lightblue", alpha=0.7))
    ax4.set_xlim(0, 1)
    ax4.set_ylim(0, 1) 
    ax4.axis('off')
    ax4.set_title('Community Analysis Summary', fontweight='bold')
    
    plt.tight_layout()
    plt.savefig(PLOTS_DIR / 'community_analysis.png', dpi=300, bbox_inches='tight')
    plt.close()
    print("📊 Saved: plots/community_analysis.png")

def create_network_sample_visualization(centrality_data):
    """Create a sample network visualization with top nodes highlighted"""
    try:
        # Load original network (sample for visualization)
        facebook_file = Path("facebook_combined.txt")
        if not facebook_file.exists():
            print("⚠️ Facebook dataset not found for network visualization")
            return
        
        # Load a sample of the network (first 500 edges for performance)
        edges = []
        with open(facebook_file, 'r') as f:
            for i, line in enumerate(f):
                if i >= 500:  # Limit for visualization
                    break
                u, v = map(int, line.strip().split())
                edges.append((u, v))
        
        G = nx.Graph()
        G.add_edges_from(edges)
        
        # Get the largest connected component
        largest_cc = max(nx.connected_components(G), key=len)
        G_sample = G.subgraph(largest_cc).copy()
        
        if len(G_sample.nodes()) < 10:
            print("⚠️ Sample network too small for visualization")
            return
        
        fig, ax = plt.subplots(1, 1, figsize=(12, 10))
        
        # Position nodes using spring layout
        pos = nx.spring_layout(G_sample, k=1, iterations=50)
        
        # Color nodes based on degree centrality if available
        if 'degree' in centrality_data:
            degree_df = centrality_data['degree']
            node_colors = []
            for node in G_sample.nodes():
                if node in degree_df['node'].values:
                    degree_val = degree_df[degree_df['node'] == node]['degree'].iloc[0]
                    node_colors.append(degree_val)
                else:
                    node_colors.append(0)
        else:
            node_colors = [G_sample.degree(node) for node in G_sample.nodes()]
        
        # Draw the network
        nx.draw(G_sample, pos, 
               node_color=node_colors,
               node_size=[v*10 for v in node_colors],
               cmap=plt.cm.plasma,
               alpha=0.8,
               edge_color='gray',
               edge_alpha=0.5,
               width=0.5,
               ax=ax)
        
        ax.set_title('Facebook Network Sample\n(Node size/color = Degree Centrality)', 
                    fontsize=14, fontweight='bold')
        
        # Add colorbar
        sm = plt.cm.ScalarMappable(cmap=plt.cm.plasma, 
                                  norm=plt.Normalize(vmin=min(node_colors), 
                                                   vmax=max(node_colors)))
        sm.set_array([])
        cbar = plt.colorbar(sm, ax=ax, shrink=0.6)
        cbar.set_label('Degree Centrality', rotation=270, labelpad=20)
        
        plt.tight_layout()
        plt.savefig(PLOTS_DIR / 'network_sample_visualization.png', dpi=300, bbox_inches='tight')
        plt.close()
        print("📊 Saved: plots/network_sample_visualization.png")
        
    except Exception as e:
        print(f"⚠️ Network visualization failed: {e}")

def main():
    """Generate all plots"""
    print("🎨 Generating Facebook Network Analysis Plots...")
    print("="*50)
    
    # Load data
    centrality_data = load_centrality_data()
    performance_df = load_performance_data()
    communities = load_community_data()
    
    print("\n📊 Generating Visualizations...")
    print("-" * 30)
    
    # Generate plots
    if centrality_data:
        plot_centrality_distributions(centrality_data)
        plot_centrality_rankings(centrality_data)
        plot_centrality_correlation(centrality_data)
        create_network_sample_visualization(centrality_data)
    
    if performance_df is not None:
        plot_performance_comparison(performance_df)
    
    if communities:
        plot_community_analysis(communities)
    
    print("\n🎉 Plot generation complete!")
    print(f"📁 All plots saved in: {PLOTS_DIR}")
    print("\nGenerated plots:")
    for plot_file in sorted(PLOTS_DIR.glob("*.png")):
        print(f"   • {plot_file.name}")

if __name__ == "__main__":
    main()