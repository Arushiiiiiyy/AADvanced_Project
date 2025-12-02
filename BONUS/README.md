# Small-World Network Phenomena: Real-World Applications

A comprehensive analysis suite for exploring small-world network phenomena through real-world simulations including disease spread, social influence, cooperation evolution, and transport efficiency.

## 📋 Overview

This project demonstrates the **small-world phenomenon** discovered by Duncan Watts and Steven Strogatz (1998):

> **Small-world networks** combine high local clustering (like regular networks) with short path lengths (like random networks), making them ideal for efficient information transmission while maintaining community structure.

### Key Properties
- **High Clustering Coefficient (C)**: Strong local communities
- **Low Average Path Length (L)**: Fast global connectivity
- **Real-world relevance**: Social networks, neural networks, power grids, internet

## 📁 Project Structure

```
BONUS/
├── generate.py              # Python script to generate network datasets
├── network_analyzer.cpp     # C++ program to analyze network metrics (C, L)
├── disease_spread.cpp       # Disease epidemic simulation (SIR model)
├── social_influence.cpp     # Information diffusion & viral marketing
├── cooperation.cpp          # Evolution of cooperation (game theory)
├── transport.cpp            # Transport efficiency & hub vulnerability
├── BA.cpp                   # Barabási-Albert generator (reference)
├── ER.cpp                   # Erdős-Rényi generator (reference)
├── Watts-Strogatz.cpp       # Watts-Strogatz generator (reference)
├── run_all.sh              # Master script to run everything
└── README.md               # This file
```

## 🚀 Quick Start

### Prerequisites

**Python packages:**
```bash
pip install networkx numpy pandas matplotlib
```

**C++ compiler:**
- macOS: `g++` (install via Xcode Command Line Tools)
- Linux: `g++`
- Windows: MinGW or MSVC

### Run Everything

```bash
chmod +x run_all.sh
./run_all.sh
```

This will:
1. Generate three network types (WS, ER, BA)
2. Export networks as adjacency lists
3. Compile all C++ programs
4. Run comprehensive analysis
5. Create visualization plots

## 📊 What Does `generate.py` Do?

### Purpose
`generate.py` is a Python script that generates three fundamental network topologies and exports them for C++ analysis.

### Generated Networks

1. **Watts-Strogatz (WS)** - Small-World Network
   - Parameters: N=1000 nodes, K=6 neighbors, p=0.1 rewiring
   - Properties: High C, Low L
   - Real-world analogy: Social networks, neural networks

2. **Erdős-Rényi (ER)** - Random Network
   - Parameters: N=1000 nodes, p=K/(N-1) edge probability
   - Properties: Low C, Low L
   - Real-world analogy: Baseline for comparison

3. **Barabási-Albert (BA)** - Scale-Free Network
   - Parameters: N=1000 nodes, m=3 edges per new node
   - Properties: Power-law degree distribution
   - Real-world analogy: Internet, citation networks

### Output Files

The script creates:
- `small_world_analysis_data/data_proof_WS.txt` - WS network adjacency list
- `small_world_analysis_data/data_proof_ER.txt` - ER network adjacency list
- `small_world_analysis_data/data_proof_BA.txt` - BA network adjacency list
- `small_world_analysis_data/1_metrics_comparison_bar.png` - C and L comparison
- `small_world_analysis_data/2_ws_rewiring_sweep_plot.png` - Small-world transition

### File Format

Each `.txt` file has the format:
```
N                          # Total number of nodes
0: neighbor1 neighbor2 ... # Node 0's adjacency list
1: neighbor1 neighbor2 ... # Node 1's adjacency list
...
```

## 🔄 Can the C++ Algorithms Run on Generated Data?

### Original C++ Files (BA.cpp, ER.cpp, Watts-Strogatz.cpp)

**NO** - These are **network generators**, not analyzers:
- They create networks from scratch using algorithms
- They don't read input files
- They lack main() functions for execution
- They're reference implementations

### New C++ Programs (YES!)

I've created **5 new C++ programs** that:
- ✅ **Read** the adjacency list files from `generate.py`
- ✅ **Analyze** network properties (C, L, degree distribution)
- ✅ **Simulate** real-world phenomena

## 🧪 Real-World Scenario Simulations

### 1. Disease Spread (`disease_spread.cpp`)

**Model:** SIR (Susceptible-Infected-Recovered) epidemic

**Key Findings:**
- Small-world networks enable **rapid disease transmission**
- Short paths (low L) → fast spread across the network
- High clustering (high C) → local outbreak amplification
- Few infected individuals can trigger large epidemics

**Usage:**
```bash
./disease_spread small_world_analysis_data/data_proof_WS.txt 0.3 0.1 10
#                <network file>                               <β>  <γ> <simulations>
# β = infection probability, γ = recovery probability
```

**Real-world examples:** COVID-19, flu, computer viruses

---

### 2. Social Influence (`social_influence.cpp`)

**Models:** 
- Threshold Model: Adoption based on peer pressure
- Viral Marketing: Targeting influencers

**Key Findings:**
- Small-world networks are **ideal for viral campaigns**
- Targeting hubs (high-degree nodes) maximizes reach
- Lower adoption thresholds → larger cascades
- Few seeds can trigger global cascades via shortcuts

**Usage:**
```bash
# Threshold model
./social_influence small_world_analysis_data/data_proof_WS.txt threshold

# Viral marketing
./social_influence small_world_analysis_data/data_proof_WS.txt viral
```

**Real-world examples:** Twitter trends, product adoption, political movements

---

### 3. Cooperation Evolution (`cooperation.cpp`)

**Model:** Evolutionary Prisoner's Dilemma

**Key Findings:**
- Small-world networks **support cooperation** better than random networks
- High clustering → cooperators form protective clusters
- Shortcuts → successful strategies spread quickly
- Network structure affects evolutionary outcomes

**Usage:**
```bash
./cooperation small_world_analysis_data/data_proof_WS.txt 0.5 100
#            <network file>                               <init> <gens>
# init = initial cooperation probability, gens = generations
```

**Real-world examples:** Altruism, trust networks, collaboration

---

### 4. Transport Efficiency (`transport.cpp`)

**Models:**
- Packet routing with congestion
- Hub vulnerability analysis

**Key Findings:**
- Small-world networks: **Efficient BUT vulnerable**
- Short paths → fast delivery
- BUT: Shortcuts create congestion at hubs
- Hub failures → network fragmentation

**Usage:**
```bash
# Routing simulation
./transport small_world_analysis_data/data_proof_WS.txt routing

# Vulnerability analysis
./transport small_world_analysis_data/data_proof_WS.txt vulnerability
```

**Real-world examples:** Air traffic, internet routing, supply chains

---

### 5. Network Analysis (`network_analyzer.cpp`)

**Metrics:**
- Average Clustering Coefficient (C)
- Average Shortest Path Length (L)
- Degree distribution

**Usage:**
```bash
./network_analyzer small_world_analysis_data/data_proof_WS.txt
```

**Output:**
- Network properties (nodes, edges, degrees)
- C and L values
- Small-world criteria check

## 📈 Expected Results

### Network Metrics Comparison

| Network Type    | Clustering (C) | Path Length (L) | Small-World? |
|----------------|---------------|-----------------|--------------|
| **Watts-Strogatz** | **HIGH** (≈0.4) | **LOW** (≈5)    | **YES ✓**    |
| Erdős-Rényi    | LOW (≈0.006)  | LOW (≈4.5)      | NO ✗         |
| Barabási-Albert| MED (≈0.01)   | LOW (≈3.5)      | NO ✗         |

### Disease Spread

| Network Type    | Infected (%) | Peak Time | Duration |
|----------------|-------------|-----------|----------|
| **Small-World** | **60-80%**  | **Early** | **Fast** |
| Random         | 50-70%      | Early     | Fast     |
| Scale-Free     | 70-90%      | Very Early| Very Fast|

### Social Influence

| Strategy       | Reach (%) | ROI    | Cost  |
|---------------|-----------|--------|-------|
| Random seeds  | 20-40%    | 2-4x   | HIGH  |
| **Hub targeting** | **60-80%** | **10-20x** | **LOW** |

## 🔬 Scientific Significance

### Why Small-World Networks Matter

1. **Biology**: Neural networks, protein interactions, metabolic pathways
2. **Sociology**: Six degrees of separation, social movements
3. **Technology**: Internet topology, power grids
4. **Epidemiology**: Disease outbreak patterns
5. **Economics**: Trade networks, financial contagion

### Key Insights

- **Efficiency + Robustness Trade-off**: Small-world networks are efficient but vulnerable to hub failures
- **Phase Transition**: Small amount of rewiring (p ≈ 0.01-0.1) creates small-world properties
- **Universality**: Many real networks self-organize into small-world structure

## 🎯 Key Takeaways

### Summary: Can C++ Algos Run on Generated Data?

| File | Type | Reads `generate.py` output? | Purpose |
|------|------|---------------------------|---------|
| `BA.cpp` | Generator | ❌ NO | Reference implementation |
| `ER.cpp` | Generator | ❌ NO | Reference implementation |
| `Watts-Strogatz.cpp` | Generator | ❌ NO | Reference implementation |
| `network_analyzer.cpp` | **Analyzer** | ✅ **YES** | **Calculate C, L** |
| `disease_spread.cpp` | **Simulator** | ✅ **YES** | **SIR epidemic** |
| `social_influence.cpp` | **Simulator** | ✅ **YES** | **Diffusion models** |
| `cooperation.cpp` | **Simulator** | ✅ **YES** | **Game theory** |
| `transport.cpp` | **Simulator** | ✅ **YES** | **Routing & congestion** |

### What `generate.py` Does

1. ✅ Generates WS, ER, BA networks using NetworkX
2. ✅ Calculates clustering coefficient (C) and path length (L)
3. ✅ Exports networks as `.txt` adjacency lists for C++
4. ✅ Creates visualization plots
5. ✅ Proves small-world phenomenon mathematically

### The Complete Pipeline

```
generate.py → Network Data Files → C++ Programs → Real-World Simulations → Insights
            (.txt adjacency lists)  (analyzers &    (disease, social,     (scientific
                                     simulators)      cooperation, etc.)    findings)
```

## 🔧 Advanced Usage

### Individual Analyses

```bash
# Compare all three networks
for net in WS ER BA; do
    echo "=== $net Network ==="
    ./network_analyzer small_world_analysis_data/data_proof_${net}.txt
done

# Disease spread with different parameters
./disease_spread <network> 0.1 0.1 10  # Low infection
./disease_spread <network> 0.5 0.1 10  # High infection

# Cooperation with different initial conditions
./cooperation <network> 0.2 100  # Few initial cooperators
./cooperation <network> 0.8 100  # Many initial cooperators
```

### Modify Network Parameters

Edit `generate.py`:
```python
N_PROOF = 1000      # Change network size
K_PROOF = 6         # Change average degree
P_WS = 0.1          # Change rewiring probability
```

## 📚 References

1. Watts, D. J., & Strogatz, S. H. (1998). *Collective dynamics of 'small-world' networks.* Nature, 393(6684), 440-442.
2. Barabási, A. L., & Albert, R. (1999). *Emergence of scaling in random networks.* Science, 286(5439), 509-512.
3. Newman, M. E. (2003). *The structure and function of complex networks.* SIAM review, 45(2), 167-256.

## 📝 License

This project is for educational purposes.

## 🤝 Contributing

Feel free to extend this project with:
- Additional real-world scenarios (traffic flow, opinion dynamics, etc.)
- More network types (geographical networks, hierarchical, etc.)
- Visualization of simulation dynamics
- Parameter optimization studies

---

**Author:** Network Science Explorer  
**Date:** December 2025  
**Course:** Network Science / Complex Systems (Bonus Assignment)
