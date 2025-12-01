#!/usr/bin/env python3
"""
Facebook SNAP Dataset Analysis Script
Automatically downloads SNAP Facebook dataset and runs all available algorithms
"""

import os
import subprocess
import time
import urllib.request
import gzip
import shutil
import psutil
import csv
from pathlib import Path

# Project paths
PROJECT_ROOT = Path(__file__).parent
CODES_DIR = PROJECT_ROOT / "codes"
OUTPUT_DIR = PROJECT_ROOT / "facebook_results"
DATASET_PATH = PROJECT_ROOT / "facebook_combined.txt"

# Algorithm configurations
CENTRALITY_ALGOS = {
    'degree_centrality': 'Degree Centrality',
    'closeness_centrality': 'Closeness Centrality', 
    'betweenness_centrality': 'Betweenness Centrality',
    'eigenvector_centrality': 'Eigenvector Centrality',
    'pagerank': 'PageRank'
}

COMMUNITY_ALGOS = {
    'label_propagation': 'Label Propagation',
    'girwan_newman': 'Girvan-Newman'
}

SHORTEST_PATH_ALGOS = {
    'bellmann_ford': 'Bellman-Ford',
    'djikstra_edge': 'Dijkstra'
}

SCC_ALGOS = {
    'kosaraju': 'Kosaraju SCC',
    'tarjan': 'Tarjan SCC'
}

def print_header(text):
    """Print formatted header"""
    print("\n" + "="*60)
    print(f"🚀 {text}")
    print("="*60)

def print_step(step, text):
    """Print formatted step"""
    print(f"\n📋 STEP {step}: {text}")
    print("-" * 40)

def download_facebook_dataset():
    """Download and extract Facebook dataset if not present"""
    if DATASET_PATH.exists():
        print("✅ Facebook dataset already exists")
        return True
    
    print("📥 Downloading SNAP Facebook dataset...")
    url = "http://snap.stanford.edu/data/facebook_combined.txt.gz"
    gz_path = PROJECT_ROOT / "facebook_combined.txt.gz"
    
    try:
        # Download
        urllib.request.urlretrieve(url, gz_path)
        print(f"✅ Downloaded: {gz_path}")
        
        # Extract
        with gzip.open(gz_path, 'rb') as f_in:
            with open(DATASET_PATH, 'wb') as f_out:
                shutil.copyfileobj(f_in, f_out)
        
        # Clean up
        gz_path.unlink()
        print(f"✅ Extracted: {DATASET_PATH}")
        
        # Verify
        lines = sum(1 for _ in open(DATASET_PATH))
        print(f"📊 Dataset: {lines:,} edges")
        return True
        
    except Exception as e:
        print(f"❌ Download failed: {e}")
        return False

def build_algorithms():
    """Build C++ algorithms"""
    print("🔧 Building C++ algorithms...")
    
    # Build centrality algorithms
    centrality_dir = CODES_DIR / "Centrality"
    if (centrality_dir / "build.sh").exists():
        try:
            result = subprocess.run(
                ["bash", "build.sh"], 
                cwd=centrality_dir, 
                capture_output=True, 
                text=True
            )
            if result.returncode == 0:
                print("✅ Centrality algorithms built")
            else:
                print("⚠️ Centrality build warnings (might still work)")
        except Exception as e:
            print(f"⚠️ Centrality build failed: {e}")
    
    # Build community algorithms  
    community_dir = CODES_DIR / "community"
    if (community_dir / "build.sh").exists():
        try:
            result = subprocess.run(
                ["bash", "build.sh"],
                cwd=community_dir,
                capture_output=True,
                text=True
            )
            if result.returncode == 0:
                print("✅ Community algorithms built")
            else:
                print("⚠️ Community build warnings (might still work)")
        except Exception as e:
            print(f"⚠️ Community build failed: {e}")

def run_algorithm_with_source(exe_path, dataset_path, output_dir, algo_name, source_node=0):
    """Run shortest path algorithm with source node parameter"""
    if not exe_path.exists():
        print(f"❌ {algo_name}: executable not found")
        return None
    
    try:
        process = psutil.Process()
        baseline_memory = process.memory_info().rss / (1024 * 1024)
        
        start_time = time.perf_counter()
        
        # Run with source node parameter
        result = subprocess.run(
            [str(exe_path), str(source_node), str(dataset_path)],
            capture_output=True,
            text=True,
            timeout=60  # 1 minute timeout
        )
        
        end_time = time.perf_counter()
        peak_memory = process.memory_info().rss / (1024 * 1024)
        memory_used = max(0, peak_memory - baseline_memory)
        runtime = end_time - start_time
        
        if result.returncode == 0:
            # Count output files or lines in stdout
            output_lines = len(result.stdout.splitlines()) if result.stdout else 0
            print(f"✅ {algo_name}: {runtime:.3f}s | {memory_used:.2f}MB | {output_lines} output lines")
            
            return {
                'algorithm': algo_name,
                'runtime_seconds': runtime,
                'memory_mb': memory_used,
                'output_lines': output_lines,
                'status': 'success'
            }
        else:
            print(f"❌ {algo_name}: failed")
            return None
            
    except subprocess.TimeoutExpired:
        print(f"⏰ {algo_name}: timeout")
        return None
    except Exception as e:
        print(f"❌ {algo_name}: error - {e}")
        return None

def run_algorithm_simple(exe_path, dataset_path, output_dir, algo_name):
    """Run algorithm that only needs input file (like SCC algorithms)"""
    if not exe_path.exists():
        print(f"❌ {algo_name}: executable not found")
        return None
    
    try:
        process = psutil.Process()
        baseline_memory = process.memory_info().rss / (1024 * 1024)
        
        start_time = time.perf_counter()
        
        # Run with just the dataset
        result = subprocess.run(
            [str(exe_path), str(dataset_path)],
            capture_output=True,
            text=True,
            timeout=60  # 1 minute timeout
        )
        
        end_time = time.perf_counter()
        peak_memory = process.memory_info().rss / (1024 * 1024)
        memory_used = max(0, peak_memory - baseline_memory)
        runtime = end_time - start_time
        
        if result.returncode == 0:
            # Count lines in stdout
            output_lines = len(result.stdout.splitlines()) if result.stdout else 0
            print(f"✅ {algo_name}: {runtime:.3f}s | {memory_used:.2f}MB | {output_lines} output lines")
            
            return {
                'algorithm': algo_name,
                'runtime_seconds': runtime,
                'memory_mb': memory_used,
                'output_lines': output_lines,
                'status': 'success'
            }
        else:
            print(f"❌ {algo_name}: failed")
            return None
            
    except subprocess.TimeoutExpired:
        print(f"⏰ {algo_name}: timeout")
        return None
    except Exception as e:
        print(f"❌ {algo_name}: error - {e}")
        return None

def run_label_propagation(exe_path, dataset_path, output_dir, algo_name):
    """Run label propagation algorithm (special case - creates community_output.txt)"""
    if not exe_path.exists():
        print(f"❌ {algo_name}: executable not found")
        return None
    
    try:
        process = psutil.Process()
        baseline_memory = process.memory_info().rss / (1024 * 1024)
        
        start_time = time.perf_counter()
        
        # Change to community directory since it creates community_output.txt there
        community_dir = exe_path.parent
        result = subprocess.run(
            [str(exe_path), str(dataset_path)],
            cwd=community_dir,
            capture_output=True,
            text=True,
            timeout=300  # 5 minute timeout
        )
        
        end_time = time.perf_counter()
        peak_memory = process.memory_info().rss / (1024 * 1024)
        memory_used = max(0, peak_memory - baseline_memory)
        runtime = end_time - start_time
        
        # Check if community_output.txt was created
        community_output = community_dir / "community_output.txt"
        if result.returncode == 0 and community_output.exists():
            lines = sum(1 for _ in open(community_output))
            print(f"✅ {algo_name}: {runtime:.3f}s | {memory_used:.2f}MB | {lines} communities")
            
            # Copy to expected location
            dest_file = output_dir / "fb_label_propagation_communities.txt"
            import shutil
            shutil.copy2(community_output, dest_file)
            
            return {
                'algorithm': algo_name,
                'runtime_seconds': runtime,
                'memory_mb': memory_used,
                'output_lines': lines,
                'status': 'success'
            }
        else:
            print(f"❌ {algo_name}: failed")
            if result.stderr:
                print(f"   Error: {result.stderr[:100]}")
            return None
            
    except subprocess.TimeoutExpired:
        print(f"⏰ {algo_name}: timeout (>5min)")
        return None
    except Exception as e:
        print(f"❌ {algo_name}: error - {e}")
        return None

def run_algorithm(exe_path, dataset_path, output_dir, algo_name):
    """Run a single algorithm and return performance metrics"""
    if not exe_path.exists():
        print(f"❌ {algo_name}: executable not found")
        return None
    
    output_file = output_dir / f"fb_{Path(exe_path).stem}.csv"
    
    try:
        # Get baseline memory
        process = psutil.Process()
        baseline_memory = process.memory_info().rss / (1024 * 1024)  # MB
        
        # Start timing
        start_time = time.perf_counter()
        
        # Run algorithm
        result = subprocess.run(
            [str(exe_path), str(dataset_path), str(output_file)],
            capture_output=True,
            text=True,
            timeout=300  # 5 minute timeout
        )
        
        # End timing
        end_time = time.perf_counter()
        
        # Get peak memory (approximate)
        peak_memory = process.memory_info().rss / (1024 * 1024)  # MB
        memory_used = max(0, peak_memory - baseline_memory)
        
        runtime = end_time - start_time
        
        if result.returncode == 0 and output_file.exists():
            lines = sum(1 for _ in open(output_file))
            print(f"✅ {algo_name}: {runtime:.3f}s | {memory_used:.2f}MB | {lines:,} results")
            
            return {
                'algorithm': algo_name,
                'runtime_seconds': runtime,
                'memory_mb': memory_used,
                'output_lines': lines,
                'status': 'success'
            }
        else:
            print(f"❌ {algo_name}: failed")
            if result.stderr:
                print(f"   Error: {result.stderr[:100]}")
            return {
                'algorithm': algo_name,
                'runtime_seconds': None,
                'memory_mb': None,
                'output_lines': 0,
                'status': 'failed'
            }
            
    except subprocess.TimeoutExpired:
        print(f"⏰ {algo_name}: timeout (>5min)")
        return {
            'algorithm': algo_name,
            'runtime_seconds': None,
            'memory_mb': None,
            'output_lines': 0,
            'status': 'timeout'
        }
    except Exception as e:
        print(f"❌ {algo_name}: error - {e}")
        return {
            'algorithm': algo_name,
            'runtime_seconds': None,
            'memory_mb': None,
            'output_lines': 0,
            'status': f'error: {str(e)}'
        }

def analyze_facebook_dataset():
    """Run all algorithms on Facebook dataset"""
    if not DATASET_PATH.exists():
        print("❌ Dataset not found")
        return []
    
    # Create output directory
    OUTPUT_DIR.mkdir(exist_ok=True)
    
    all_results = []
    
    print_step(1, "Centrality Algorithms")
    centrality_dir = CODES_DIR / "Centrality"
    
    for algo_key, algo_name in CENTRALITY_ALGOS.items():
        exe_path = centrality_dir / algo_key
        result = run_algorithm(exe_path, DATASET_PATH, OUTPUT_DIR, algo_name)
        if result:
            all_results.append(result)
    
    print_step(2, "Community Detection Algorithms") 
    community_dir = CODES_DIR / "community"
    
    for algo_key, algo_name in COMMUNITY_ALGOS.items():
        exe_path = community_dir / algo_key
        if algo_key == 'girwan_newman':
            print(f"⏭️ {algo_name}: skipped (too slow for large graphs)")
            continue
        if algo_key == 'label_propagation':
            # Special handling for label propagation (creates community_output.txt)
            result = run_label_propagation(exe_path, DATASET_PATH, OUTPUT_DIR, algo_name)
        else:
            result = run_algorithm(exe_path, DATASET_PATH, OUTPUT_DIR, algo_name)
        if result:
            all_results.append(result)
    
    print_step(3, "Shortest Path Algorithms")
    basic_algos_dir = CODES_DIR / "Basic algos"
    
    for algo_key, algo_name in SHORTEST_PATH_ALGOS.items():
        # Compile if needed
        exe_path = PROJECT_ROOT / algo_key
        cpp_file = basic_algos_dir / f"{algo_key}.cpp"
        
        if cpp_file.exists():
            try:
                print(f"🔧 Compiling {algo_name}...")
                subprocess.run(
                    ["g++", "-O2", "-std=c++17", str(cpp_file), "-o", str(exe_path)],
                    check=True, capture_output=True
                )
                
                # These algorithms need source node parameter (use node 0)
                result = run_algorithm_with_source(exe_path, DATASET_PATH, OUTPUT_DIR, algo_name, source_node=0)
                if result:
                    all_results.append(result)
                    
            except subprocess.CalledProcessError as e:
                print(f"❌ {algo_name}: compilation failed")
    
    print_step(4, "Strongly Connected Components")
    
    for algo_key, algo_name in SCC_ALGOS.items():
        # Compile if needed  
        exe_path = PROJECT_ROOT / algo_key
        cpp_file = basic_algos_dir / f"{algo_key}.cpp"
        
        if cpp_file.exists():
            try:
                print(f"🔧 Compiling {algo_name}...")
                subprocess.run(
                    ["g++", "-O2", "-std=c++17", str(cpp_file), "-o", str(exe_path)],
                    check=True, capture_output=True
                )
                
                # These algorithms just need the input file
                result = run_algorithm_simple(exe_path, DATASET_PATH, OUTPUT_DIR, algo_name)
                if result:
                    all_results.append(result)
                    
            except subprocess.CalledProcessError as e:
                print(f"❌ {algo_name}: compilation failed")
    
    return all_results

def generate_report(results):
    """Generate analysis report and performance CSV"""
    report_file = OUTPUT_DIR / "facebook_analysis_report.txt"
    performance_csv = OUTPUT_DIR / "algorithm_performance.csv"
    
    # Get dataset stats
    edges = sum(1 for _ in open(DATASET_PATH))
    nodes = len(set(
        node for line in open(DATASET_PATH) 
        for node in line.strip().split()
    ))
    
    # Save performance data to CSV
    successful_results = [r for r in results if r['status'] == 'success']
    
    with open(performance_csv, 'w', newline='') as f:
        if successful_results:
            writer = csv.DictWriter(f, fieldnames=['algorithm', 'runtime_seconds', 'memory_mb', 'output_lines', 'status'])
            writer.writeheader()
            for result in results:
                writer.writerow(result)
    
    print(f"📊 Performance CSV saved: {performance_csv}")
    
    # Generate text report
    with open(report_file, 'w') as f:
        f.write("="*70 + "\n")
        f.write("FACEBOOK NETWORK ANALYSIS REPORT\n")
        f.write(f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write("="*70 + "\n\n")
        
        f.write("DATASET INFORMATION\n")
        f.write("-" * 20 + "\n")
        f.write(f"Source: SNAP Facebook Social Circles\n")
        f.write(f"Nodes: {nodes:,}\n")
        f.write(f"Edges: {edges:,}\n")
        f.write(f"Avg Degree: {2*edges/nodes:.2f}\n")
        f.write(f"Density: {edges/(nodes*(nodes-1)/2):.6f}\n\n")
        
        if successful_results:
            f.write("ALGORITHM PERFORMANCE COMPARISON\n")
            f.write("-" * 35 + "\n")
            f.write(f"{'Algorithm':<25} {'Time(s)':<10} {'Memory(MB)':<12} {'Status':<10}\n")
            f.write("-" * 70 + "\n")
            
            # Sort by runtime for comparison
            sorted_results = sorted(successful_results, key=lambda x: x['runtime_seconds'])
            
            for result in sorted_results:
                f.write(f"{result['algorithm']:<25} ")
                f.write(f"{result['runtime_seconds']:>7.3f}   ")
                f.write(f"{result['memory_mb']:>8.2f}    ")
                f.write(f"{result['status']:<10}\n")
            
            f.write("\n")
            
            # Performance insights
            f.write("PERFORMANCE INSIGHTS\n")
            f.write("-" * 20 + "\n")
            fastest = min(sorted_results, key=lambda x: x['runtime_seconds'])
            slowest = max(sorted_results, key=lambda x: x['runtime_seconds'])
            lowest_memory = min(sorted_results, key=lambda x: x['memory_mb'])
            highest_memory = max(sorted_results, key=lambda x: x['memory_mb'])
            
            f.write(f"⚡ Fastest Algorithm: {fastest['algorithm']} ({fastest['runtime_seconds']:.3f}s)\n")
            f.write(f"🐌 Slowest Algorithm: {slowest['algorithm']} ({slowest['runtime_seconds']:.3f}s)\n")
            f.write(f"💾 Lowest Memory: {lowest_memory['algorithm']} ({lowest_memory['memory_mb']:.2f}MB)\n")
            f.write(f"🔥 Highest Memory: {highest_memory['algorithm']} ({highest_memory['memory_mb']:.2f}MB)\n")
            
            speedup = slowest['runtime_seconds'] / fastest['runtime_seconds']
            f.write(f"📈 Speed Difference: {speedup:.1f}x faster (fastest vs slowest)\n\n")
        
        # Failed algorithms
        failed_results = [r for r in results if r['status'] != 'success']
        if failed_results:
            f.write("FAILED ALGORITHMS\n")
            f.write("-" * 17 + "\n")
            for result in failed_results:
                f.write(f"{result['algorithm']:<25}: {result['status']}\n")
            f.write("\n")
        
        f.write("OUTPUT FILES\n")
        f.write("-" * 12 + "\n")
        for file in sorted(OUTPUT_DIR.glob("fb_*.csv")):
            size = file.stat().st_size / 1024  # KB
            f.write(f"{file.name:<30}: {size:>8.1f} KB\n")
    
    print(f"📊 Report saved: {report_file}")

def main():
    """Main execution function"""
    print_header("FACEBOOK SNAP DATASET ANALYSIS")
    
    print_step(1, "Dataset Preparation")
    if not download_facebook_dataset():
        print("❌ Failed to prepare dataset")
        return
    
    print_step(2, "Algorithm Compilation")
    build_algorithms()
    
    print_step(3, "Algorithm Execution")  
    results = analyze_facebook_dataset()
    
    print_step(4, "Report Generation")
    generate_report(results)
    
    print_header("ANALYSIS COMPLETE")
    print(f"📁 Results directory: {OUTPUT_DIR}")
    
    successful_results = [r for r in results if r['status'] == 'success']
    print(f"📊 Successful algorithms: {len(successful_results)}")
    print(f"❌ Failed algorithms: {len(results) - len(successful_results)}")
    
    if successful_results:
        fastest = min(successful_results, key=lambda x: x['runtime_seconds'])
        slowest = max(successful_results, key=lambda x: x['runtime_seconds'])
        lowest_mem = min(successful_results, key=lambda x: x['memory_mb'])
        highest_mem = max(successful_results, key=lambda x: x['memory_mb'])
        
        print(f"\n⚡ Fastest: {fastest['algorithm']} ({fastest['runtime_seconds']:.3f}s)")
        print(f"🐌 Slowest: {slowest['algorithm']} ({slowest['runtime_seconds']:.3f}s)")
        print(f"💾 Lowest Memory: {lowest_mem['algorithm']} ({lowest_mem['memory_mb']:.2f}MB)")
        print(f"🔥 Highest Memory: {highest_mem['algorithm']} ({highest_mem['memory_mb']:.2f}MB)")
    
    print("\n💡 Quick commands:")
    print(f"   cat {OUTPUT_DIR}/facebook_analysis_report.txt")
    print(f"   cat {OUTPUT_DIR}/algorithm_performance.csv")
    print(f"   ls -lh {OUTPUT_DIR}/")

if __name__ == "__main__":
    main()