# AADvanced_Project

A comprehensive graph algorithms analysis project for the Stanford SNAP Facebook dataset. This project implements and benchmarks 10 different graph algorithms including centrality measures, community detection, shortest path, and strongly connected components algorithms.

## 📋 Prerequisites

- **C++ Compiler**: g++ with C++17 support
- **Python 3.x**: For data processing and visualization
- **Internet Connection**: To download the Facebook dataset

## 🚀 Complete Setup and Execution Guide

### Step 1: Clone and Navigate to Project
```bash
git clone https://github.com/Arushiiiiiyy/AADvanced_Project.git
cd AADvanced_Project
```

### Step 2: Install Python Dependencies
```bash
# Install required Python packages
pip3 install networkx matplotlib pandas numpy scipy psutil seaborn
```

### Step 3: Download Facebook Dataset
```bash
# Download Stanford SNAP Facebook dataset
wget http://snap.stanford.edu/data/facebook_combined.txt.gz

# Extract the dataset
gunzip facebook_combined.txt.gz

# Verify dataset is downloaded (should show ~1.8MB file)
ls -la facebook_combined.txt
```

### Step 4: Compile All Algorithms
```bash
# Make compilation script executable and run it
chmod +x test_facebook_optimized.sh
./test_facebook_optimized.sh
```

**Expected Output**: You should see successful compilation messages for all algorithms without errors.

### Step 5: Run Complete Algorithm Analysis
```bash
# Execute all algorithms with performance monitoring
python3 run_facebook_analysis.py
```

**This will:**
- Run 10 different graph algorithms on the Facebook dataset
- Measure execution time and memory usage for each algorithm
- Generate CSV outputs with results
- Create performance reports
- Organize all outputs in the `facebook_results/` directory

### Step 6: Generate Visualization Plots
```bash
# Create comprehensive visualizations
python3 generate_plots.py
```

**This will generate:**
- Centrality distribution plots
- Algorithm performance comparisons
- Community analysis visualizations
- Network sample visualization
- All plots saved in the `plots/` directory as PNG files

## 📊 Algorithm Coverage

### Centrality Algorithms (5)
1. **Degree Centrality** - Node connectivity measure
2. **Closeness Centrality** - Average distance to all other nodes
3. **Betweenness Centrality** - Nodes that lie on shortest paths
4. **Eigenvector Centrality** - Influence based on connections to important nodes
5. **PageRank** - Google's page ranking algorithm

### Community Detection (1)
6. **Label Propagation** - Community detection algorithm

### Shortest Path Algorithms (2)
7. **Bellman-Ford** - Single-source shortest path with negative weights
8. **Dijkstra** - Single-source shortest path for non-negative weights

### Strongly Connected Components (2)
9. **Kosaraju's Algorithm** - SCC detection using DFS
10. **Tarjan's Algorithm** - SCC detection using single DFS

## 📁 Output Structure

After running the algorithms, your project will have:

```
AADvanced_Project/
├── facebook_results/          # All algorithm outputs
│   ├── fb_*_centrality.csv   # Centrality results
│   ├── fb_*_output_*.txt     # Detailed algorithm outputs
│   ├── fb_*_time.txt         # Individual runtime reports
│   └── algorithm_performance.csv # Complete performance summary
├── plots/                     # Generated visualizations
│   ├── centrality_*.png      # Centrality analysis plots
│   ├── performance_*.png     # Performance comparison plots
│   └── community_*.png       # Community analysis plots
└── facebook_combined.txt      # Original dataset
```

## 🔍 Understanding the Results

### Performance Data
- **algorithm_performance.csv**: Complete runtime and memory usage for all algorithms
- Individual **_time.txt** files: Detailed execution reports per algorithm

### Centrality Results
- **CSV files**: Node ID and corresponding centrality values
- **Rankings**: Nodes sorted by centrality importance

### Community Detection
- **fb_lpa_communities.txt**: Communities found by Label Propagation Algorithm
- Each line represents a community with member node IDs

### Shortest Path Results
- **Output files**: Distance matrices and path information from source node 0

## 🛠 Troubleshooting

### Compilation Errors
```bash
# If compilation fails, check C++17 support
g++ --version

# Update compilation standard if needed
sed -i 's/-std=c++11/-std=c++17/g' test_facebook_optimized.sh
```

### Dataset Issues
```bash
# If download fails, manual download:
curl -O http://snap.stanford.edu/data/facebook_combined.txt.gz
gunzip facebook_combined.txt.gz
```

### Python Package Issues
```bash
# If packages missing, install individually:
pip3 install networkx
pip3 install matplotlib
pip3 install pandas
pip3 install psutil
pip3 install seaborn
```

## 📈 Dataset Information

**Stanford SNAP Facebook Dataset:**
- **Nodes**: 4,039 (Facebook users)
- **Edges**: 88,234 (Friend connections)
- **Type**: Undirected social network
- **Source**: Stanford Network Analysis Project

## 🎯 Expected Runtime

- **Small algorithms** (Degree, Closeness): < 1 second
- **Medium algorithms** (PageRank, Dijkstra): 1-5 seconds  
- **Large algorithms** (Betweenness, Bellman-Ford): 5-30 seconds
- **Total runtime**: ~2-3 minutes for all algorithms

## 📝 Notes

- All outputs are automatically organized in `facebook_results/` directory
- Performance monitoring includes both time and memory usage
- Visualization script creates publication-ready plots
- All algorithms are optimized for the Facebook dataset structure


