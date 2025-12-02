#!/bin/bash

# ============================================================
# Setup Script for Small-World Network Analysis Project
# This script sets up the complete environment, compiles C++
# programs, and runs all necessary initialization files
# ============================================================

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Get the directory where the script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Flag to check if this is called from start.sh (quiet mode)
QUIET_MODE=false
FORCE_SETUP=false

# Parse arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --quiet|-q) QUIET_MODE=true ;;
        --force|-f) FORCE_SETUP=true ;;
        --help|-h) 
            echo "Usage: ./setup.sh [OPTIONS]"
            echo "Options:"
            echo "  -q, --quiet    Run in quiet mode (minimal output)"
            echo "  -f, --force    Force re-setup even if already done"
            echo "  -h, --help     Show this help message"
            exit 0
            ;;
        *) echo "Unknown parameter: $1"; exit 1 ;;
    esac
    shift
done

# Function to print messages based on mode
print_msg() {
    if [ "$QUIET_MODE" = false ]; then
        echo -e "$1"
    fi
}

print_header() {
    if [ "$QUIET_MODE" = false ]; then
        echo ""
        echo -e "${CYAN}╔══════════════════════════════════════════════════════════════════════╗${NC}"
        echo -e "${CYAN}║                                                                      ║${NC}"
        echo -e "${CYAN}║       🚀 SMALL-WORLD NETWORK ANALYSIS - SETUP SCRIPT 🚀              ║${NC}"
        echo -e "${CYAN}║                                                                      ║${NC}"
        echo -e "${CYAN}╚══════════════════════════════════════════════════════════════════════╝${NC}"
        echo ""
    fi
}

# Check if setup has already been completed
SETUP_MARKER=".setup_complete"
check_setup_needed() {
    if [ "$FORCE_SETUP" = true ]; then
        return 0  # Setup needed
    fi
    
    # Check if marker exists and all required files are present
    if [ -f "$SETUP_MARKER" ] && \
       [ -f "network_analyzer" ] && \
       [ -f "disease_spread" ] && \
       [ -f "social_influence" ] && \
       [ -d "venv" ] && \
       [ -d "small_world_analysis_data" ]; then
        return 1  # Setup not needed
    fi
    return 0  # Setup needed
}

# ============================================================
# MAIN SETUP FUNCTION
# ============================================================
run_setup() {
    print_header

    # ============================================================
    # STEP 1: Check System Prerequisites
    # ============================================================
    print_msg "${BLUE}[STEP 1/6]${NC} Checking system prerequisites..."
    print_msg ""

    # Check Python3
    if command -v python3 &> /dev/null; then
        PYTHON_VERSION=$(python3 --version 2>&1)
        print_msg "  ${GREEN}✓${NC} Python3 found: $PYTHON_VERSION"
    else
        echo -e "  ${RED}✗${NC} Python3 not found. Please install Python 3."
        echo "    Ubuntu/Debian: sudo apt install python3 python3-pip python3-venv"
        exit 1
    fi

    # Check pip
    if command -v pip3 &> /dev/null; then
        print_msg "  ${GREEN}✓${NC} pip3 found"
    else
        print_msg "  ${YELLOW}⚠${NC} pip3 not found. Attempting to install..."
        sudo apt install python3-pip -y || {
            echo -e "  ${RED}✗${NC} Failed to install pip3. Please install manually."
            exit 1
        }
    fi

    # Check g++ compiler
    if command -v g++ &> /dev/null; then
        GCC_VERSION=$(g++ --version | head -n 1)
        print_msg "  ${GREEN}✓${NC} g++ found: $GCC_VERSION"
    else
        print_msg "  ${YELLOW}⚠${NC} g++ not found. Attempting to install..."
        sudo apt install g++ build-essential -y || {
            echo -e "  ${RED}✗${NC} Failed to install g++. Please install manually."
            echo "    Ubuntu/Debian: sudo apt install build-essential"
            exit 1
        }
    fi

    print_msg ""

    # ============================================================
    # STEP 2: Create and Activate Virtual Environment
    # ============================================================
    print_msg "${BLUE}[STEP 2/6]${NC} Setting up Python virtual environment..."
    print_msg ""

    VENV_DIR="venv"

    if [ -d "$VENV_DIR" ]; then
        print_msg "  ${GREEN}✓${NC} Virtual environment already exists"
    else
        print_msg "  Creating virtual environment..."
        python3 -m venv $VENV_DIR
        print_msg "  ${GREEN}✓${NC} Virtual environment created"
    fi

    # Activate virtual environment
    source $VENV_DIR/bin/activate
    print_msg "  ${GREEN}✓${NC} Virtual environment activated"
    print_msg ""

    # ============================================================
    # STEP 3: Install Python Dependencies
    # ============================================================
    print_msg "${BLUE}[STEP 3/6]${NC} Installing Python dependencies..."
    print_msg ""

    # Upgrade pip first
    pip install --upgrade pip > /dev/null 2>&1
    print_msg "  ${GREEN}✓${NC} pip upgraded"

    # Install required packages
    PACKAGES="flask networkx numpy pandas matplotlib"

    for package in $PACKAGES; do
        if [ "$QUIET_MODE" = false ]; then
            echo -n "  Installing $package..."
        fi
        pip install $package > /dev/null 2>&1
        print_msg " ${GREEN}✓${NC}"
    done

    print_msg ""
    print_msg "  ${GREEN}✓${NC} All Python dependencies installed"
    print_msg ""

    # ============================================================
    # STEP 4: Compile C++ Programs with C++17
    # ============================================================
    print_msg "${BLUE}[STEP 4/6]${NC} Compiling C++ programs..."
    print_msg ""

    # Function to compile a C++ file with fallback standards
    compile_cpp() {
        local src="$1"
        local out="$2"
        local optional="$3"
        
        if [ ! -f "$src" ]; then
            if [ "$optional" != "true" ]; then
                print_msg "  ${YELLOW}⚠${NC} $src not found, skipping..."
            fi
            return 1
        fi
        
        if [ "$QUIET_MODE" = false ]; then
            echo -n "  Compiling $src → $out..."
        fi
        
        # Try C++17 first, then C++14, then C++11
        if g++ -std=c++17 -O2 -o "$out" "$src" 2>/dev/null; then
            print_msg " ${GREEN}✓${NC}"
            return 0
        elif g++ -std=c++14 -O2 -o "$out" "$src" 2>/dev/null; then
            print_msg " ${GREEN}✓${NC} (C++14)"
            return 0
        elif g++ -std=c++11 -O2 -o "$out" "$src" 2>/dev/null; then
            print_msg " ${GREEN}✓${NC} (C++11)"
            return 0
        else
            if [ "$optional" = "true" ]; then
                print_msg " ${YELLOW}⚠${NC} (Optional, skipped)"
            else
                print_msg " ${RED}✗${NC} (Compilation failed)"
            fi
            return 1
        fi
    }

    # Main analysis programs (required)
    compile_cpp "network_analyzer.cpp" "network_analyzer"
    compile_cpp "disease_spread.cpp" "disease_spread"
    compile_cpp "social_influence.cpp" "social_influence"
    compile_cpp "cooperation.cpp" "cooperation"
    compile_cpp "transport.cpp" "transport"

    # Network generators (optional)
    compile_cpp "Watts-Strogatz.cpp" "Watts-Strogatz" "true"
    compile_cpp "ER.cpp" "ER" "true"
    compile_cpp "BA.cpp" "BA" "true"

    # Menu (optional)
    compile_cpp "menu.cpp" "menu_cpp" "true"

    print_msg ""

    # ============================================================
    # STEP 5: Generate Network Data
    # ============================================================
    print_msg "${BLUE}[STEP 5/6]${NC} Generating network data..."
    print_msg ""

    # Create data directory if not exists
    mkdir -p small_world_analysis_data

    if [ -f "generate.py" ]; then
        print_msg "  Running generate.py (this may take a moment)..."
        if python3 generate.py 2>/dev/null; then
            print_msg "  ${GREEN}✓${NC} Network data generated successfully"
        else
            print_msg "  ${YELLOW}⚠${NC} generate.py had some issues, continuing..."
        fi
    else
        print_msg "  ${YELLOW}⚠${NC} generate.py not found, skipping data generation"
    fi

    # Run create_test_data.py if exists
    if [ -f "create_test_data.py" ]; then
        print_msg "  Running create_test_data.py..."
        python3 create_test_data.py 2>/dev/null && \
            print_msg "  ${GREEN}✓${NC} Test data created" || \
            print_msg "  ${YELLOW}⚠${NC} Test data generation skipped"
    fi

    print_msg ""

    # ============================================================
    # STEP 6: Make Scripts Executable & Finalize
    # ============================================================
    print_msg "${BLUE}[STEP 6/6]${NC} Finalizing setup..."
    print_msg ""

    # Make all shell scripts executable
    for script in start.sh demo.sh run_all.sh setup.sh; do
        if [ -f "$script" ]; then
            chmod +x "$script"
            print_msg "  ${GREEN}✓${NC} Made $script executable"
        fi
    done

    # Make compiled binaries executable
    for prog in network_analyzer disease_spread social_influence cooperation transport Watts-Strogatz ER BA menu_cpp; do
        if [ -f "$prog" ]; then
            chmod +x "$prog"
        fi
    done

    # Create setup marker file
    echo "Setup completed on $(date)" > "$SETUP_MARKER"
    print_msg "  ${GREEN}✓${NC} Setup marker created"

    print_msg ""

    # ============================================================
    # SETUP COMPLETE
    # ============================================================
    if [ "$QUIET_MODE" = false ]; then
        echo -e "${CYAN}╔══════════════════════════════════════════════════════════════════════╗${NC}"
        echo -e "${CYAN}║                    ✅ SETUP COMPLETE!                                ║${NC}"
        echo -e "${CYAN}╚══════════════════════════════════════════════════════════════════════╝${NC}"
        echo ""
        echo -e "${GREEN}Your environment is ready! Here's what you can do now:${NC}"
        echo ""
        echo "  📊 Launch Web Dashboard (Recommended):"
        echo -e "     ${YELLOW}python3 web_dashboard.py${NC}"
        echo "     Then open: http://localhost:8080"
        echo ""
        echo "  📋 Use Terminal Menu:"
        echo -e "     ${YELLOW}python3 menu.py${NC}"
        echo ""
        echo "  🚀 Quick Start:"
        echo -e "     ${YELLOW}./start.sh${NC}"
        echo ""
        echo "  🔬 Run Network Analyzer:"
        echo -e "     ${YELLOW}./network_analyzer small_world_analysis_data/data_proof_WS.txt${NC}"
        echo ""
        echo -e "${BLUE}Note:${NC} To activate the virtual environment in future sessions, run:"
        echo -e "     ${YELLOW}source venv/bin/activate${NC}"
        echo ""
    fi
}

# ============================================================
# MAIN EXECUTION
# ============================================================

if check_setup_needed; then
    if [ "$QUIET_MODE" = true ]; then
        echo "⚙️  Running first-time setup (this may take a moment)..."
    fi
    run_setup
    exit 0
else
    if [ "$QUIET_MODE" = false ]; then
        echo -e "${GREEN}✓${NC} Setup already complete. Use --force to re-run setup."
    fi
    # Still activate venv if it exists
    if [ -d "venv" ]; then
        source venv/bin/activate
    fi
    exit 0
fi
