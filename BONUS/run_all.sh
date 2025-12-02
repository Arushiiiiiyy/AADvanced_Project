#!/bin/bash

# Compilation and Execution Script for Small-World Network Analysis

echo "================================================"
echo "Small-World Phenomena: Real-World Simulations"
echo "================================================"
echo ""

# Color codes for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Step 1: Generate network data using Python
echo -e "${BLUE}[STEP 1]${NC} Generating network datasets..."
echo ""
python3 generate.py

if [ $? -ne 0 ]; then
    echo "Error: Python script failed. Make sure you have the required packages:"
    echo "  pip install networkx numpy pandas matplotlib"
    exit 1
fi

echo ""
echo -e "${GREEN}✓ Network data generated successfully!${NC}"
echo ""

# Step 2: Compile C++ programs
echo -e "${BLUE}[STEP 2]${NC} Compiling C++ programs..."
echo ""

programs=("network_analyzer" "disease_spread" "social_influence" "cooperation" "transport")

for prog in "${programs[@]}"; do
    echo "Compiling ${prog}.cpp..."
    g++ -std=c++11 -O2 -o $prog ${prog}.cpp
    
    if [ $? -ne 0 ]; then
        echo "Error compiling ${prog}.cpp"
        exit 1
    fi
done

echo ""
echo -e "${GREEN}✓ All programs compiled successfully!${NC}"
echo ""

# Step 3: Run analyses
echo -e "${BLUE}[STEP 3]${NC} Running small-world analysis on generated networks..."
echo ""

DATA_DIR="small_world_analysis_data"

if [ ! -d "$DATA_DIR" ]; then
    echo "Error: Data directory not found. Make sure generate.py ran successfully."
    exit 1
fi

# Network types to analyze
networks=("WS" "ER" "BA")
network_names=("Watts-Strogatz (Small-World)" "Erdős-Rényi (Random)" "Barabási-Albert (Scale-Free)")

echo "================================================"
echo "PART A: NETWORK METRICS ANALYSIS"
echo "================================================"

for i in "${!networks[@]}"; do
    net="${networks[$i]}"
    name="${network_names[$i]}"
    
    echo ""
    echo -e "${YELLOW}>>> Analyzing $name Network${NC}"
    ./network_analyzer "${DATA_DIR}/data_proof_${net}.txt"
done

echo ""
echo "================================================"
echo "PART B: REAL-WORLD SCENARIO SIMULATIONS"
echo "================================================"

# Use WS (small-world) network for scenarios
WS_FILE="${DATA_DIR}/data_proof_WS.txt"

echo ""
echo -e "${YELLOW}>>> Scenario 1: Disease Spread (SIR Model)${NC}"
./disease_spread "$WS_FILE" 0.3 0.1 10

echo ""
echo -e "${YELLOW}>>> Scenario 2: Social Influence (Threshold Model)${NC}"
./social_influence "$WS_FILE" threshold

echo ""
echo -e "${YELLOW}>>> Scenario 3: Viral Marketing${NC}"
./social_influence "$WS_FILE" viral

echo ""
echo -e "${YELLOW}>>> Scenario 4: Cooperation Evolution${NC}"
./cooperation "$WS_FILE" 0.5 100

echo ""
echo -e "${YELLOW}>>> Scenario 5: Transport Routing${NC}"
./transport "$WS_FILE" routing

echo ""
echo -e "${YELLOW}>>> Scenario 6: Hub Vulnerability${NC}"
./transport "$WS_FILE" vulnerability

echo ""
echo "================================================"
echo "COMPARISON: Small-World vs Random vs Scale-Free"
echo "================================================"

echo ""
echo -e "${YELLOW}>>> Disease Spread Comparison${NC}"
for i in "${!networks[@]}"; do
    net="${networks[$i]}"
    name="${network_names[$i]}"
    echo ""
    echo "--- $name ---"
    ./disease_spread "${DATA_DIR}/data_proof_${net}.txt" 0.3 0.1 5
done

echo ""
echo "================================================"
echo -e "${GREEN}✓ All analyses complete!${NC}"
echo "================================================"
echo ""
echo "Generated files:"
echo "  - Network data: ${DATA_DIR}/*.txt"
echo "  - Plots: ${DATA_DIR}/*.png"
echo "  - Executables: network_analyzer, disease_spread, social_influence, cooperation, transport"
echo ""
echo "To run individual simulations:"
echo "  ./network_analyzer ${DATA_DIR}/data_proof_WS.txt"
echo "  ./disease_spread ${DATA_DIR}/data_proof_WS.txt 0.3 0.1 10"
echo "  ./social_influence ${DATA_DIR}/data_proof_WS.txt threshold"
echo "  ./cooperation ${DATA_DIR}/data_proof_WS.txt 0.5 100"
echo "  ./transport ${DATA_DIR}/data_proof_WS.txt routing"
echo ""
