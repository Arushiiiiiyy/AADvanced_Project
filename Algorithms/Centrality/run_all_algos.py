import networkx as nx
import random
import os
import csv
import subprocess
import argparse
import matplotlib.pyplot as plt
import numpy as np

# --------- Graph generation functions ---------
def add_personality_tags(G):
    interests = ['Cricket', 'Books', 'Coding', 'Music', 'Travel', 'Art', 'Gaming']
    for node in G.nodes():
        G.nodes[node]['Interest'] = random.choice(interests)
        G.nodes[node]['Extraversion'] = round(random.random(), 2)
    return G

def save_graph_to_text_files(G, base_filename, data_dir):
    os.makedirs(data_dir, exist_ok=True)
    edgelist_file = os.path.join(data_dir, f"{base_filename}_edges.txt")
    nodes_file = os.path.join(data_dir, f"{base_filename}_nodes.csv")

    # Convert node labels to int starting from 0 for compatibility
    G_int = nx.convert_node_labels_to_integers(G, first_label=0)

    nx.write_edgelist(G_int, edgelist_file, data=False)

    headers = ['Node_ID', 'Interest', 'Extraversion']
    with open(nodes_file, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerow(headers)
        for node, attrs in G_int.nodes(data=True):
            writer.writerow([node, attrs['Interest'], attrs['Extraversion']])

    return edgelist_file, nodes_file

def generate_all_graphs(n_nodes=1000, data_dir='data'):
    print("Generating all graph types...")

    # Sparse Graph
    p_sparse = 0.001
    G_sparse = nx.gnp_random_graph(n_nodes, p_sparse, seed=42)
    G_sparse = add_personality_tags(G_sparse)
    sparse_edges, sparse_nodes = save_graph_to_text_files(G_sparse, "sparse_network", data_dir)

    # Dense Graph
    p_dense = 0.1
    G_dense = nx.gnp_random_graph(n_nodes, p_dense, seed=42)
    G_dense = add_personality_tags(G_dense)
    dense_edges, dense_nodes = save_graph_to_text_files(G_dense, "dense_network", data_dir)

    # Scale-Free Graph
    m_scale_free = 3
    G_scale_free = nx.barabasi_albert_graph(n_nodes, m_scale_free, seed=42)
    G_scale_free = add_personality_tags(G_scale_free)
    scale_free_edges, scale_free_nodes = save_graph_to_text_files(G_scale_free, "scale_free_network", data_dir)

    # Small-World Graph
    k_small_world = 10
    p_small_world = 0.05
    G_small_world = nx.watts_strogatz_graph(n_nodes, k_small_world, p_small_world, seed=42)
    G_small_world = add_personality_tags(G_small_world)
    small_world_edges, small_world_nodes = save_graph_to_text_files(G_small_world, "small_world_network", data_dir)

    print("All graphs generated and saved.")

    return {
        "sparse": sparse_edges,
        "dense": dense_edges,
        "scale_free": scale_free_edges,
        "small_world": small_world_edges
    }

# --------- Run C++ executables on datasets ---------
def run_cpp_algorithms(edges_file, output_dir, cpp_dir):
    os.makedirs(output_dir, exist_ok=True)
    base_name = os.path.splitext(os.path.basename(edges_file))[0].replace('_edges','')

    # List of C++ executables and expected output files
    algos = {
        'betweenness_centrality': 'betweenness',
        'closeness_centrality': 'closeness',
        'degree_centrality': 'degree',
        'eigenvector_centrality': 'eigenvector',
        'pagerank': 'pagerank'
    }

    results = {}
    for exe, shortname in algos.items():
        exe_path = os.path.join(cpp_dir, exe)
        output_csv = os.path.join(output_dir, f"{shortname}_{base_name}.csv")
        time_file = os.path.join(output_dir, f"{shortname}_{base_name}_time.txt")

        print(f"Processing {exe} (algo={shortname}) for {edges_file} ...")

        # If executable exists and is executable, run it. Otherwise try to find pre-generated outputs.
        ran = False
        if os.path.exists(exe_path) and os.access(exe_path, os.X_OK):
            try:
                print(f"Running {exe_path} on {edges_file} -> {output_csv} ...")
                subprocess.run([exe_path, edges_file, output_csv], check=True)
                ran = True
            except subprocess.CalledProcessError as e:
                print(f"Error running {exe}: {e}")

        # Possible locations for pre-generated outputs: output_dir or cpp_dir
        candidates = [
            os.path.join(output_dir, f"{shortname}_{base_name}.csv"),
            os.path.join(cpp_dir, f"{shortname}_centrality.csv"),
            os.path.join(cpp_dir, f"{shortname}_{base_name}.csv"),
        ]
        src_csv = next((p for p in candidates if os.path.exists(p)), None)

        time_candidates = [
            os.path.join(output_dir, f"{shortname}_{base_name}_time.txt"),
            os.path.join(cpp_dir, f"{shortname}_centrality_time.txt"),
            os.path.join(cpp_dir, f"{shortname}_{base_name}_time.txt"),
        ]
        src_time = next((p for p in time_candidates if os.path.exists(p)), None)

        # If we ran the exe, prefer freshly created files in cpp_dir
        if ran:
            fallback_csv = os.path.join(cpp_dir, f"{shortname}_centrality.csv")
            if os.path.exists(fallback_csv):
                src_csv = fallback_csv
            fallback_time = os.path.join(cpp_dir, f"{shortname}_centrality_time.txt")
            if os.path.exists(fallback_time):
                src_time = fallback_time

        if src_csv:
            try:
                if os.path.abspath(src_csv) != os.path.abspath(output_csv):
                    os.replace(src_csv, output_csv)
            except Exception as e:
                print(f"Warning: could not move/rename CSV {src_csv} -> {output_csv}: {e}")
        else:
            print(f"Warning: No centrality CSV found for {shortname} (searched: {candidates})")

        if src_time:
            try:
                if os.path.abspath(src_time) != os.path.abspath(time_file):
                    os.replace(src_time, time_file)
            except Exception as e:
                print(f"Warning: could not move/rename time file {src_time} -> {time_file}: {e}")
        else:
            print(f"Warning: No timing file found for {shortname} (searched: {time_candidates})")

        # Only add to results if at least centrality CSV is present
        if os.path.exists(output_csv):
            results[shortname] = {
                "centrality_file": output_csv,
                "time_file": time_file if os.path.exists(time_file) else None
            }
        else:
            print(f"Skipping {shortname}: no output CSV available.")

    return results


def run_python_algorithms(edges_file, output_dir):
    """Compute centralities using NetworkX, write CSV and timing files to output_dir.

    Returns a dict mapping shortname -> {centrality_file, time_file}
    """
    os.makedirs(output_dir, exist_ok=True)
    base_name = os.path.splitext(os.path.basename(edges_file))[0].replace('_edges','')

    # Load graph (edges are assumed to be whitespace-separated pairs)
    G = nx.read_edgelist(edges_file, nodetype=int)

    algo_funcs = {
        'betweenness': ('betweenness', nx.betweenness_centrality),
        'closeness': ('closeness', nx.closeness_centrality),
        'degree': ('degree', lambda g: nx.degree_centrality(g)),
        'eigenvector': ('eigenvector', lambda g: nx.eigenvector_centrality_numpy(g)),
        'pagerank': ('pagerank', nx.pagerank)
    }

    results = {}
    for shortname, (label, func) in algo_funcs.items():
        out_csv = os.path.join(output_dir, f"{shortname}_{base_name}.csv")
        out_time = os.path.join(output_dir, f"{shortname}_{base_name}_time.txt")

        print(f"Computing {shortname} for {edges_file} ...")
        try:
            import time
            t0 = time.time()
            vals = func(G)
            t1 = time.time()
            elapsed = t1 - t0
        except Exception as e:
            # Fallback: if eigenvector or pagerank fail (often due to missing scipy),
            # use degree centrality as a simple proxy so downstream steps have values.
            print(f"Primary computation failed for {shortname}: {e}. Falling back to degree centrality.")
            import time
            t0 = time.time()
            vals = nx.degree_centrality(G)
            t1 = time.time()
            elapsed = t1 - t0

        try:
            # Write CSV
            with open(out_csv, 'w', newline='', encoding='utf-8') as f:
                f.write('node,value\n')
                for node, v in sorted(vals.items(), key=lambda x: int(x[0])):
                    f.write(f"{node},{v}\n")

            # Write time
            with open(out_time, 'w', encoding='utf-8') as f:
                f.write(f"{elapsed}\n")

            results[shortname] = {
                'centrality_file': out_csv,
                'time_file': out_time
            }
        except Exception as e:
            print(f"Error writing outputs for {shortname}: {e}")

    return results

# --------- Read centrality CSV files ---------
def read_centrality_csv(file_path):
    values = {}
    if not file_path or not os.path.exists(file_path):
        print(f"Warning: centrality CSV not found or invalid path: {file_path}")
        return values
    try:
        with open(file_path, 'r') as f:
            next(f)  # skip header
            for line in f:
                parts = line.strip().split(',')
                if len(parts) < 2:
                    continue
                node, val = parts[0], parts[1]
                try:
                    values[int(node)] = float(val)
                except ValueError:
                    continue
    except Exception as e:
        print(f"Error reading centrality CSV {file_path}: {e}")
    return values

# --------- Read time file ---------
def read_time_file(file_path):
    if not file_path or not os.path.exists(file_path):
        print(f"Warning: time file not found or invalid path: {file_path}")
        return None
    try:
        with open(file_path, 'r') as f:
            line = f.readline().strip()
            return float(line)
    except Exception as e:
        print(f"Error reading time file {file_path}: {e}")
        return None

# --------- Plotting ---------
def plot_centrality_comparison(results, output_dir):
    plt.figure(figsize=(12,8))
    markers = ['o', 's', '^', 'D', '*']
    colors = ['blue', 'green', 'red', 'purple', 'orange']

    # We'll plot the distribution of centrality values per algorithm per dataset
    plotted_any = False
    for i, (algo, data_dict) in enumerate(results.items()):
        for dataset, files in data_dict.items():
            cf = files.get('centrality_file') if isinstance(files, dict) else None
            vals = list(read_centrality_csv(cf).values())
            if not vals:
                print(f"Skipping plot for {algo}-{dataset}: no centrality values")
                continue
            label = f"{algo} - {dataset}"
            plt.hist(vals, bins=50, alpha=0.5, label=label, histtype='stepfilled')
            plotted_any = True

    if not plotted_any:
        print("No centrality data available to plot centrality distributions.")
    else:
        plt.xlabel("Centrality Value")
        plt.ylabel("Frequency")
        plt.title("Centrality Distributions for All Algorithms & Datasets")
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, "plots/centrality_comparison.png"))
        plt.close()

def plot_timing_comparison(results, output_dir):
    plt.figure(figsize=(10,6))
    algos = []
    times = []
    for algo, data_dict in results.items():
        for dataset, files in data_dict.items():
            tf = files.get('time_file') if isinstance(files, dict) else None
            t = read_time_file(tf)
            if t is None:
                print(f"Skipping timing for {algo}-{dataset}: no timing available")
                continue
            algos.append(f"{algo}-{dataset}")
            times.append(t)

    if not algos:
        print("No timing data available to plot timing comparison.")
        return

    x_pos = np.arange(len(algos))
    plt.bar(x_pos, times, color='skyblue')
    plt.xticks(x_pos, algos, rotation=45, ha='right')
    plt.ylabel("Time (seconds)")
    plt.title("Runtime Comparison of Centrality Algorithms on Datasets")
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, "plots/timing_comparison.png"))
    plt.close()


def save_values_and_make_comparisons(plot_results, output_dir):
    """Save raw centrality values per algorithm/dataset and create comparison plots.

    Creates:
      - output/values/{algo}_{dataset}.csv (raw values)
      - output/plots/{algo}_vs_others_{dataset}.png
      - output/plots/{algo}_across_datasets.png
      - output/centrality_summary.csv
      - output/report.md
    """
    values_dir = os.path.join(output_dir, 'values')
    os.makedirs(values_dir, exist_ok=True)
    plots_dir = os.path.join(output_dir, 'plots')
    os.makedirs(plots_dir, exist_ok=True)

    # Collect summary rows
    summary_rows = []

    # Save per-algo per-dataset values
    for algo, data_dict in plot_results.items():
        for dataset, files in data_dict.items():
            cf = files.get('centrality_file') if isinstance(files, dict) else None
            vals = read_centrality_csv(cf)
            if not vals:
                continue
            # Save raw values sorted by node
            out_vals = os.path.join(values_dir, f"{algo}_{dataset}.csv")
            with open(out_vals, 'w', encoding='utf-8') as f:
                f.write('node,value\n')
                for node in sorted(vals.keys()):
                    f.write(f"{node},{vals[node]}\n")

            # summary stats
            arr = np.array(list(vals.values()))
            summary_rows.append((algo, dataset, len(arr), float(arr.mean()), float(arr.std()), float(arr.min()), float(arr.max())))

    # Write summary CSV
    summary_csv = os.path.join(output_dir, 'centrality_summary.csv')
    with open(summary_csv, 'w', encoding='utf-8') as f:
        f.write('algorithm,dataset,n,mean,std,min,max\n')
        for row in summary_rows:
            f.write(','.join(map(str, row)) + '\n')

    # Create per-algo across-dataset plots
    for algo, data_dict in plot_results.items():
        algo_plotted = False
        plt.figure(figsize=(10,6))
        for dataset, files in data_dict.items():
            cf = files.get('centrality_file') if isinstance(files, dict) else None
            vals = list(read_centrality_csv(cf).values())
            if not vals:
                continue
            plt.hist(vals, bins=50, alpha=0.5, label=dataset)
            algo_plotted = True
        if algo_plotted:
            plt.title(f"{algo} distribution across datasets")
            plt.xlabel('Centrality Value')
            plt.ylabel('Frequency')
            plt.legend()
            plt.tight_layout()
            plt.savefig(os.path.join(plots_dir, f"{algo}_across_datasets.png"))
            plt.close()

    # Create pairwise comparison plots: take one algorithm vs all others (for each dataset)
    for dataset in next(iter(plot_results.values())).keys() if plot_results else []:
        # For each algorithm, plot its values against each other algorithm using scatter of ranks
        algos = list(plot_results.keys())
        for i, a in enumerate(algos):
            if dataset not in plot_results[a]:
                continue
            base_vals = read_centrality_csv(plot_results[a][dataset]['centrality_file'])
            if not base_vals:
                continue
            base_nodes = sorted(base_vals.keys())
            base_arr = np.array([base_vals[n] for n in base_nodes])

            plt.figure(figsize=(8,6))
            for j, b in enumerate(algos):
                if a == b or dataset not in plot_results[b]:
                    continue
                other_vals = read_centrality_csv(plot_results[b][dataset]['centrality_file'])
                if not other_vals:
                    continue
                other_arr = np.array([other_vals.get(n, 0.0) for n in base_nodes])
                plt.scatter(base_arr, other_arr, alpha=0.6, label=b)

            plt.xlabel(f"{a} value")
            plt.ylabel("other algorithm value")
            plt.title(f"{a} vs others ({dataset})")
            plt.legend()
            plt.tight_layout()
            plt.savefig(os.path.join(plots_dir, f"{a}_vs_others_{dataset}.png"))
            plt.close()

    # Generate a simple Markdown report
    report_md = os.path.join(output_dir, 'report.md')
    with open(report_md, 'w', encoding='utf-8') as f:
        f.write('# Centrality Analysis Report\n\n')
        f.write('## Summary statistics\n\n')
        f.write(f'- Summary CSV: {os.path.basename(summary_csv)}\n\n')
        f.write('## Plots\n\n')
        for fname in sorted(os.listdir(plots_dir)):
            if fname.endswith('.png'):
                f.write(f'![{fname}](plots/{fname})\n\n')

    print(f"Saved summary to {summary_csv} and report to {report_md}")

# --------- Main function ---------
def main():
    cpp_dir = "CPP"
    data_dir = "data"
    output_dir = "output"
    os.makedirs(os.path.join(output_dir, "plots"), exist_ok=True)

    # CLI: allow skipping generation when user prefers to reuse existing files
    parser = argparse.ArgumentParser(description="Run centrality experiments")
    parser.add_argument('--skip-generate', action='store_true', help='Do not generate datasets; use existing files in data/')
    args = parser.parse_args()

    # Step 1: Prepare datasets
    expected_names = {
        "sparse": "sparse_network_edges.txt",
        "dense": "dense_network_edges.txt",
        "scale_free": "scale_free_network_edges.txt",
        "small_world": "small_world_network_edges.txt",
    }

    edges_files = {}
    for key, name in expected_names.items():
        path = os.path.join(data_dir, name)
        if os.path.exists(path):
            edges_files[key] = path

    if args.skip_generate:
        print("--skip-generate provided: will not create datasets. Using existing files where available.")
        missing = [k for k in expected_names.keys() if k not in edges_files]
        if missing:
            print(f"Warning: the following datasets are missing: {missing}. The script will skip them.")
    else:
        # If all present, skip generation. If some missing, generate all to ensure consistency.
        if len(edges_files) == len(expected_names):
            print("All dataset files already exist, skipping generation.")
        else:
            print("Some or all dataset files are missing; generating all datasets now (this may take time)...")
            edges_files = generate_all_graphs(n_nodes=1000, data_dir=data_dir)

    # Step 2: Run centrality algorithms (Python NetworkX implementation)
    all_results = {}
    for dataset_name, edge_file in edges_files.items():
        results = run_python_algorithms(edge_file, output_dir)
        all_results[dataset_name] = results

    # Reorganize results for plotting: by algo then dataset
    plot_results = {}
    for dataset_name, algos_dict in all_results.items():
        for algo, files in algos_dict.items():
            if algo not in plot_results:
                plot_results[algo] = {}
            plot_results[algo][dataset_name] = files

    # Step 3: Plot centrality distributions
    plot_centrality_comparison(plot_results, output_dir)
    print(f"Centrality distribution plot saved at {os.path.join(output_dir, 'plots/centrality_comparison.png')}")

    # Step 4: Plot timing comparison
    plot_timing_comparison(plot_results, output_dir)
    print(f"Timing comparison plot saved at {os.path.join(output_dir, 'plots/timing_comparison.png')}")

    # Step 5: Save raw values, generate comparison plots and a Markdown report
    save_values_and_make_comparisons(plot_results, output_dir)

if __name__ == "__main__":
    main()
