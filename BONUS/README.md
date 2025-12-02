# 🌐 Small-World Network Phenomena Analyzer

A comprehensive interactive analysis suite for exploring small-world network phenomena through real-world simulations including disease spread, social influence, cooperation evolution, and transport efficiency.

![Python](https://img.shields.io/badge/Python-3.8+-blue.svg)
![C++](https://img.shields.io/badge/C++-17-orange.svg)
![Flask](https://img.shields.io/badge/Flask-Web_Dashboard-green.svg)

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Quick Start](#-quick-start)
- [Installation](#-installation)
- [Usage](#-usage)
- [Web Dashboard](#-web-dashboard)
- [Command Line Tools](#-command-line-tools)
- [Project Structure](#-project-structure)
- [Network Types](#-network-types)
- [Simulations Explained](#-simulations-explained)

---

## 🔬 Overview

This project demonstrates the **small-world phenomenon** discovered by Duncan Watts and Steven Strogatz (1998):

> **Small-world networks** combine high local clustering (like regular networks) with short path lengths (like random networks), making them ideal for efficient information transmission while maintaining community structure.

### Key Properties
| Property | Small-World | Random | Scale-Free |
|----------|-------------|--------|------------|
| Clustering (C) | **High** | Low | Medium |
| Path Length (L) | **Low** | Low | Very Low |
| Degree Distribution | Normal | Poisson | Power-law |
| Hub Vulnerability | Low | Low | **High** |

---

## ✨ Features

### 🌐 Interactive Web Dashboard
- Real-time network visualizations (2D & 3D)
- Interactive simulations with adjustable parameters
- Side-by-side comparison of all network types
- Beautiful charts powered by Plotly.js and D3.js

### 🧪 Simulations
1. **🦠 Disease Spread** - SIR epidemic model
2. **📱 Social Influence** - Information diffusion & viral marketing
3. **🤝 Cooperation Evolution** - Prisoner's Dilemma game theory
4. **🚗 Transport Efficiency** - Routing & hub vulnerability analysis

### 📊 Analysis Tools
- Network metrics calculation (Clustering Coefficient, Path Length)
- Comparative analysis across network types
- Data visualization and export

---

## 🚀 Quick Start

### One-Command Setup & Launch

```bash
./start.sh
```

This will:
1. ✅ Check prerequisites (Python, g++)
2. ✅ Create virtual environment
3. ✅ Install Python packages (Flask, NetworkX, NumPy, Pandas, Matplotlib)
4. ✅ Compile all C++ programs
5. ✅ Generate network datasets
6. ✅ Launch the interactive menu

---

## 📦 Installation

### Prerequisites

| Requirement | Version | Check Command |
|-------------|---------|---------------|
| Python | 3.8+ | `python3 --version` |
| pip | Latest | `pip3 --version` |
| g++ | C++17 support | `g++ --version` |

### Manual Installation

```bash
# 1. Clone or navigate to the project
cd /path/to/BONUS

# 2. Run setup script
./setup.sh

# 3. Activate virtual environment (for future sessions)
source venv/bin/activate
```

### Setup Script Options

| Command | Description |
|---------|-------------|
| `./setup.sh` | Normal setup (skips if already done) |
| `./setup.sh --force` | Force re-setup (recompile everything) |
| `./setup.sh --quiet` | Quiet mode (minimal output) |
| `./setup.sh --help` | Show help message |

---

## 💻 Usage

### Option 1: Web Dashboard (Recommended) 🌐

```bash
# Launch the web dashboard
python3 web_dashboard.py

# Or use the start script
./start.sh
# Then select option [1] Web Dashboard
```

Open your browser to: **http://localhost:8080**

### Option 2: Terminal Menu 📋

```bash
python3 menu.py
```

### Option 3: Direct Command Line ⌨️

See [Command Line Tools](#-command-line-tools) section below.

---

## 🌐 Web Dashboard

### Tabs Overview

| Tab | Icon | Description |
|-----|------|-------------|
| **Overview** | 📊 | Network visualizations with metrics (C, L) for all three network types |
| **Disease Spread** | 🦠 | SIR model simulation with adjustable β (infection) and γ (recovery) rates |
| **Social Influence** | 📱 | Threshold model for information diffusion |
| **Cooperation** | 🤝 | Prisoner's Dilemma evolution with adjustable temptation parameter |
| **Transport** | 🚗 | Routing efficiency, hub usage, and vulnerability analysis |
| **3D View** | 🌐 | Interactive 3D network visualization (rotate, zoom, pan) |
| **Comparison** | 📈 | Dynamic comparison of all simulations across network types |

### Dashboard Controls

#### Disease Spread Parameters
| Parameter | Range | Description |
|-----------|-------|-------------|
| Network | WS/ER/BA | Select network type |
| Infection Rate (β) | 0.0 - 1.0 | Probability of infection per contact |
| Recovery Rate (γ) | 0.0 - 1.0 | Probability of recovery per time step |

#### Social Influence Parameters
| Parameter | Range | Description |
|-----------|-------|-------------|
| Adoption Threshold | 0.0 - 1.0 | Fraction of neighbors needed to adopt |

#### Cooperation Parameters
| Parameter | Range | Description |
|-----------|-------|-------------|
| Temptation (T) | 1.0 - 2.0 | Payoff for defecting against cooperator |

#### 3D View Controls
| Action | Control |
|--------|---------|
| Rotate | Click and drag |
| Zoom | Scroll wheel |
| Pan | Right-click and drag |

---

## 🔧 Command Line Tools

### Network Analyzer
Calculates Clustering Coefficient (C) and Average Path Length (L).

```bash
./network_analyzer <network_file>
```

**Examples:**
```bash
./network_analyzer small_world_analysis_data/data_proof_WS.txt
./network_analyzer small_world_analysis_data/data_proof_ER.txt
./network_analyzer small_world_analysis_data/data_proof_BA.txt
```

### Disease Spread Simulation
Simulates SIR epidemic model.

```bash
./disease_spread <network_file> <beta> <gamma> <initial_infected>
```

| Parameter | Description |
|-----------|-------------|
| `network_file` | Path to network data file |
| `beta` | Infection rate (0.0 - 1.0) |
| `gamma` | Recovery rate (0.0 - 1.0) |
| `initial_infected` | Number of initially infected nodes |

**Example:**
```bash
./disease_spread small_world_analysis_data/data_proof_WS.txt 0.3 0.1 5
```

### Social Influence
Simulates information diffusion.

```bash
./social_influence <network_file> <mode>
```

| Mode | Description |
|------|-------------|
| `threshold` | Threshold-based adoption model |
| `viral` | Viral marketing simulation |

**Examples:**
```bash
./social_influence small_world_analysis_data/data_proof_WS.txt threshold
./social_influence small_world_analysis_data/data_proof_WS.txt viral
```

### Cooperation Evolution
Simulates Prisoner's Dilemma game theory.

```bash
./cooperation <network_file> <temptation> <rounds>
```

| Parameter | Description |
|-----------|-------------|
| `temptation` | Payoff for defecting (> 1.0 makes defection tempting) |
| `rounds` | Number of game rounds to simulate |

**Example:**
```bash
./cooperation small_world_analysis_data/data_proof_WS.txt 1.5 100
```

### Transport Analysis
Analyzes routing efficiency and hub vulnerability.

```bash
./transport <network_file> <mode>
```

| Mode | Description |
|------|-------------|
| `routing` | Routing efficiency analysis |
| `vulnerability` | Hub vulnerability analysis |

**Examples:**
```bash
./transport small_world_analysis_data/data_proof_WS.txt routing
./transport small_world_analysis_data/data_proof_WS.txt vulnerability
```

---

## 📁 Project Structure

```
BONUS/
├── 🚀 Launcher Scripts
│   ├── start.sh              # Main launcher (setup + menu)
│   ├── setup.sh              # Environment setup & compilation
│   └── run_all.sh            # Run all analyses
│
├── 🐍 Python Files
│   ├── web_dashboard.py      # Flask web server & API
│   ├── menu.py               # Terminal menu interface
│   ├── generate.py           # Network dataset generator
│   └── create_test_data.py   # Test data generator
│
├── ⚡ C++ Programs
│   ├── network_analyzer.cpp  # Metrics calculation (C, L)
│   ├── disease_spread.cpp    # SIR epidemic model
│   ├── social_influence.cpp  # Information diffusion
│   ├── cooperation.cpp       # Game theory simulation
│   ├── transport.cpp         # Routing & vulnerability
│   ├── Watts-Strogatz.cpp    # WS network generator
│   ├── ER.cpp                # Erdős-Rényi generator
│   └── BA.cpp                # Barabási-Albert generator
│
├── 🌐 Web Dashboard
│   └── templates/
│       └── dashboard.html    # Dashboard UI (HTML/CSS/JS)
│
├── 📊 Data Files
│   └── small_world_analysis_data/
│       ├── data_proof_WS.txt # Watts-Strogatz network
│       ├── data_proof_ER.txt # Erdős-Rényi network
│       └── data_proof_BA.txt # Barabási-Albert network
│
└── 📖 Documentation
    └── README.md             # This file
```

---

## 🔗 Network Types

### 1. Watts-Strogatz (Small-World) 🌐

| Property | Value |
|----------|-------|
| Clustering (C) | **High** |
| Path Length (L) | **Low** |
| Real-world examples | Social networks, neural networks, power grids |
| Key insight | "Six degrees of separation" |

### 2. Erdős-Rényi (Random) 🎲

| Property | Value |
|----------|-------|
| Clustering (C) | Low |
| Path Length (L) | Low |
| Real-world examples | Random connections, baseline model |
| Key insight | Phase transition at p = 1/n |

### 3. Barabási-Albert (Scale-Free) ⚡

| Property | Value |
|----------|-------|
| Clustering (C) | Medium |
| Path Length (L) | Very Low |
| Real-world examples | Internet, citation networks, airline routes |
| Key insight | "Rich get richer" (preferential attachment) |

---

## 🧪 Simulations Explained

### 🦠 Disease Spread (SIR Model)

Models epidemic spreading with three states:

| State | Symbol | Description |
|-------|--------|-------------|
| Susceptible | S | Can be infected |
| Infected | I | Can spread disease |
| Recovered | R | Immune |

**Key finding**: Small-world networks spread diseases faster than random networks due to local clustering combined with shortcuts.

### 📱 Social Influence (Threshold Model)

Models information/behavior adoption:
- Nodes adopt when fraction of neighbors ≥ threshold
- Simulates viral marketing, trend adoption

**Key finding**: Scale-free networks enable faster viral spread through hub nodes.

### 🤝 Cooperation (Prisoner's Dilemma)

Models evolution of cooperation:

| Outcome | Result |
|---------|--------|
| Both Cooperate | Both get reward (1, 1) |
| Both Defect | Both get punishment (0, 0) |
| One Defects | Defector gets temptation (T, 0) |

**Key finding**: Small-world clustering helps sustain cooperation by forming cooperative clusters.

### 🚗 Transport (Routing Efficiency)

Analyzes network navigation:
- Average path length for routing
- Hub identification and usage
- Vulnerability to hub removal

**Key finding**: Scale-free networks are efficient but vulnerable - removing hubs severely impacts connectivity.

---

## 🛠️ Troubleshooting

### Port Already in Use
```bash
# Kill process on port 8080
fuser -k 8080/tcp

# Then restart
python3 web_dashboard.py
```

### C++ Compilation Errors
```bash
# Force recompile
./setup.sh --force

# Or manually compile
g++ -std=c++17 -O2 -o network_analyzer network_analyzer.cpp
```

### Missing Python Packages
```bash
source venv/bin/activate
pip install flask networkx numpy pandas matplotlib
```

### Virtual Environment Not Found
```bash
python3 -m venv venv
source venv/bin/activate
pip install flask networkx numpy pandas matplotlib
```

---

## 📚 References

1. Watts, D. J., & Strogatz, S. H. (1998). *Collective dynamics of 'small-world' networks*. Nature, 393(6684), 440-442.

2. Barabási, A. L., & Albert, R. (1999). *Emergence of scaling in random networks*. Science, 286(5439), 509-512.

3. Erdős, P., & Rényi, A. (1959). *On random graphs*. Publicationes Mathematicae, 6, 290-297.

---

## 👨‍💻 Quick Reference Card

| Task | Command |
|------|---------|
| **Start everything** | `./start.sh` |
| **Web dashboard only** | `python3 web_dashboard.py` |
| **Terminal menu** | `python3 menu.py` |
| **Force recompile** | `./setup.sh --force` |
| **Kill server** | `fuser -k 8080/tcp` |
| **Activate venv** | `source venv/bin/activate` |
| **Run all analyses** | `./run_all.sh` |

---

**Built with ❤️ using Python, C++, Flask, D3.js, and Plotly**
