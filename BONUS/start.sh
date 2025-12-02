#!/bin/bash

# ============================================================
# Ultimate Launcher for Small-World Network Analysis
# This script runs setup first (if needed) then launches the system
# ============================================================

# Get the directory where the script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Color codes
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# ============================================================
# STEP 0: Run setup.sh first (if needed)
# ============================================================
SETUP_MARKER=".setup_complete"

if [ ! -f "$SETUP_MARKER" ] || [ ! -f "network_analyzer" ] || [ ! -d "venv" ]; then
    echo ""
    echo "🔧 First-time setup required. Running setup.sh..."
    echo ""
    
    # Make setup.sh executable if it isn't
    chmod +x setup.sh 2>/dev/null
    
    # Run setup
    ./setup.sh
    
    if [ $? -ne 0 ]; then
        echo "❌ Setup failed. Please check errors above."
        exit 1
    fi
    
    echo ""
    echo "════════════════════════════════════════════════════════════════════"
    echo ""
fi

# Activate virtual environment
if [ -d "venv" ]; then
    source venv/bin/activate
fi

clear

cat << "EOF"
╔══════════════════════════════════════════════════════════════════════╗
║                                                                      ║
║           🌐 SMALL-WORLD NETWORK PHENOMENA ANALYZER 🌐               ║
║                                                                      ║
║         Real-World Simulations & Interactive Visualizations         ║
║                                                                      ║
╚══════════════════════════════════════════════════════════════════════╝
EOF

echo ""
echo "Welcome! This system allows you to explore small-world networks through:"
echo "  • Disease Spread Simulations 🦠"
echo "  • Social Influence Modeling 📱"
echo "  • Cooperation Evolution 🤝"
echo "  • Transport Analysis 🚗"
echo "  • Interactive Web Dashboard 🌐"
echo ""

# Quick status check
echo "📋 System Status:"
echo -e "  ${GREEN}✓${NC} Python environment ready"
[ -f "network_analyzer" ] && echo -e "  ${GREEN}✓${NC} C++ programs compiled" || echo -e "  ${YELLOW}⚠${NC} C++ programs not compiled"
[ -d "small_world_analysis_data" ] && echo -e "  ${GREEN}✓${NC} Network data available" || echo -e "  ${YELLOW}⚠${NC} Network data not generated"
echo ""

echo "════════════════════════════════════════════════════════════════════"
echo "                        LAUNCH OPTIONS"
echo "════════════════════════════════════════════════════════════════════"
echo ""
echo "Choose how you'd like to start:"
echo ""
echo "  [1] 🌐 Web Dashboard (Recommended!) - Visual & Interactive"
echo "  [2] 📋 Terminal Menu - Command-line Interface"
echo "  [3] 📚 View Documentation"
echo "  [4] 🔧 Re-run Setup (force recompile)"
echo "  [0] Exit"
echo ""

read -p "Enter choice (0-4): " choice

case $choice in
    1)
        echo ""
        echo "🚀 Launching Web Dashboard..."
        echo ""
        
        # Kill any existing process on port 8080
        fuser -k 8080/tcp 2>/dev/null
        
        echo "┌────────────────────────────────────────────────────────────┐"
        echo "│  The dashboard will open at: http://localhost:8080        │"
        echo "│  Press Ctrl+C to stop the server                          │"
        echo "└────────────────────────────────────────────────────────────┘"
        echo ""
        sleep 2
        
        # Try to open browser automatically
        if command -v xdg-open &> /dev/null; then
            sleep 3 && xdg-open http://localhost:8080 &
        elif command -v open &> /dev/null; then
            sleep 3 && open http://localhost:8080 &
        fi
        
        python3 web_dashboard.py
        ;;
    2)
        echo ""
        echo "📋 Launching Terminal Menu..."
        sleep 1
        python3 menu.py
        ;;
    3)
        echo ""
        echo "📚 Documentation Files:"
        echo "  • README.md - Complete documentation"
        echo ""
        if [ -f "README.md" ]; then
            read -p "View README.md? (y/n): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                less README.md 2>/dev/null || cat README.md
            fi
        else
            echo "No documentation files found."
        fi
        ;;
    4)
        echo ""
        echo "🔧 Re-running setup..."
        ./setup.sh --force
        ;;
    0)
        echo ""
        echo "👋 Goodbye!"
        exit 0
        ;;
    *)
        echo ""
        echo "❌ Invalid choice"
        exit 1
        ;;
esac

echo ""
echo "════════════════════════════════════════════════════════════════════"
echo "                     Thank you for using"
echo "           🌐 Small-World Network Phenomena Analyzer 🌐"
echo "════════════════════════════════════════════════════════════════════"
echo ""
