#!/bin/bash

# Ultimate Launcher for Small-World Network Analysis
# This script sets up everything and launches the interactive system

# Activate virtual environment if it exists
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

# Check Python
echo "🔍 Checking prerequisites..."
if ! command -v python3 &> /dev/null; then
    echo "❌ Python 3 not found. Please install Python 3."
    exit 1
fi
echo "✓ Python 3 found"

# Check if Flask is installed
if python3 -c "import flask" 2>/dev/null; then
    echo "✓ Flask installed"
else
    echo "⚠️  Flask not installed"
    echo ""
    read -p "Install required packages now? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "📦 Installing packages..."
        pip3 install flask networkx numpy pandas matplotlib
    fi
fi

# Check if C++ compiler exists
if command -v g++ &> /dev/null; then
    echo "✓ C++ compiler found"
    
    # Check if programs are compiled
    if [ ! -f "network_analyzer" ]; then
        echo "⚙️  Compiling C++ programs..."
        ./demo.sh 2>&1 | grep -E "(Compiling|✓)" || true
    else
        echo "✓ C++ programs already compiled"
    fi
else
    echo "⚠️  C++ compiler not found (optional)"
fi

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
echo "  [0] Exit"
echo ""

read -p "Enter choice (0-3): " choice

case $choice in
    1)
        echo ""
        echo "🚀 Launching Web Dashboard..."
        echo ""
        echo "┌────────────────────────────────────────────────────────────┐"
        echo "│  The dashboard will open at: http://localhost:5000        │"
        echo "│  Press Ctrl+C to stop the server                          │"
        echo "└────────────────────────────────────────────────────────────┘"
        echo ""
        sleep 2
        
        # Try to open browser automatically
        if command -v open &> /dev/null; then
            sleep 3 && open http://localhost:5000 &
        elif command -v xdg-open &> /dev/null; then
            sleep 3 && xdg-open http://localhost:5000 &
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
        echo "  • QUICKSTART.md - Getting started guide"
        echo "  • SUMMARY.md - Project summary"
        echo ""
        read -p "Which file to view? (readme/quickstart/summary): " doc
        case $doc in
            readme|r)
                less README.md
                ;;
            quickstart|q)
                less QUICKSTART.md
                ;;
            summary|s)
                less SUMMARY.md
                ;;
            *)
                cat README.md
                ;;
        esac
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
