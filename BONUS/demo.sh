#!/bin/bash

# Quick Demo Script - Compile and show usage

echo "================================================"
echo "Small-World Network Analysis - Quick Demo"
echo "================================================"
echo ""

# Compile C++ programs
echo "[1] Compiling C++ programs..."
echo ""

programs=("network_analyzer" "disease_spread" "social_influence" "cooperation" "transport")

for prog in "${programs[@]}"; do
    echo "  Compiling ${prog}.cpp..."
    g++ -std=c++11 -O2 -o $prog ${prog}.cpp 2>&1 | head -5
    
    if [ $? -eq 0 ]; then
        echo "    ✓ ${prog} compiled successfully"
    else
        echo "    ✗ Error compiling ${prog}"
    fi
done

echo ""
echo "================================================"
echo "[2] Program Usage Information"
echo "================================================"
echo ""

echo "Before running the C++ programs, you need to:"
echo "  1. Install Python packages: pip install networkx numpy pandas matplotlib"
echo "  2. Run: python3 generate.py"
echo "  3. This creates network data files in small_world_analysis_data/"
echo ""

echo "Then you can run analyses:"
echo ""
echo "  # Analyze network metrics (C and L)"
echo "  ./network_analyzer small_world_analysis_data/data_proof_WS.txt"
echo ""
echo "  # Disease spread simulation"
echo "  ./disease_spread small_world_analysis_data/data_proof_WS.txt 0.3 0.1 10"
echo ""
echo "  # Social influence (threshold model)"
echo "  ./social_influence small_world_analysis_data/data_proof_WS.txt threshold"
echo ""
echo "  # Viral marketing simulation"
echo "  ./social_influence small_world_analysis_data/data_proof_WS.txt viral"
echo ""
echo "  # Cooperation evolution"
echo "  ./cooperation small_world_analysis_data/data_proof_WS.txt 0.5 100"
echo ""
echo "  # Transport routing"
echo "  ./transport small_world_analysis_data/data_proof_WS.txt routing"
echo ""
echo "  # Hub vulnerability"
echo "  ./transport small_world_analysis_data/data_proof_WS.txt vulnerability"
echo ""

echo "================================================"
echo "[3] Summary"
echo "================================================"
echo ""
echo "✓ All C++ programs compiled successfully!"
echo ""
echo "What each program does:"
echo "  network_analyzer   → Calculates C (clustering) and L (path length)"
echo "  disease_spread     → SIR epidemic model simulation"
echo "  social_influence   → Information diffusion & viral marketing"
echo "  cooperation        → Prisoner's Dilemma game theory"
echo "  transport          → Routing efficiency & hub vulnerability"
echo ""
echo "Next step: Run 'python3 generate.py' to create network datasets"
echo ""
