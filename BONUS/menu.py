#!/usr/bin/env python3
"""
Interactive Terminal Menu for Small-World Network Analysis
"""

import os
import sys
import subprocess
import time

# ANSI color codes
class Colors:
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'

def clear_screen():
    os.system('clear' if os.name == 'posix' else 'cls')

def print_header():
    clear_screen()
    print(f"{Colors.CYAN}{Colors.BOLD}")
    print("╔══════════════════════════════════════════════════════════════╗")
    print("║       SMALL-WORLD NETWORK PHENOMENA ANALYZER                 ║")
    print("║       Real-World Simulations & Visualizations                ║")
    print("╚══════════════════════════════════════════════════════════════╝")
    print(f"{Colors.END}\n")

def print_menu():
    print(f"{Colors.BOLD}📊 MAIN MENU{Colors.END}\n")
    print(f"{Colors.GREEN}[1]{Colors.END} 🌐 Generate Network Datasets (Python)")
    print(f"{Colors.GREEN}[2]{Colors.END} 🔬 Analyze Network Metrics (C, L)")
    print(f"{Colors.GREEN}[3]{Colors.END} 🦠 Disease Spread Simulation")
    print(f"{Colors.GREEN}[4]{Colors.END} 📱 Social Influence & Viral Marketing")
    print(f"{Colors.GREEN}[5]{Colors.END} 🤝 Cooperation Evolution (Game Theory)")
    print(f"{Colors.GREEN}[6]{Colors.END} 🚗 Transport & Navigation Efficiency")
    print(f"{Colors.GREEN}[7]{Colors.END} 📊 Compare All Network Types")
    print(f"{Colors.GREEN}[8]{Colors.END} 🌐 Launch Web Dashboard (Recommended!)")
    print(f"{Colors.GREEN}[9]{Colors.END} 📖 View README Documentation")
    print(f"{Colors.RED}[0]{Colors.END} 🚪 Exit\n")

def run_command(cmd, description):
    print(f"\n{Colors.YELLOW}➜ {description}{Colors.END}")
    print(f"{Colors.CYAN}Command: {cmd}{Colors.END}\n")
    result = subprocess.run(cmd, shell=True)
    print(f"\n{Colors.BLUE}{'='*60}{Colors.END}")
    input(f"\n{Colors.BOLD}Press Enter to continue...{Colors.END}")
    return result.returncode == 0

def generate_networks():
    print_header()
    print(f"{Colors.BOLD}🌐 NETWORK GENERATION{Colors.END}\n")
    print("This will generate three network types:")
    print("  • Watts-Strogatz (Small-World)")
    print("  • Erdős-Rényi (Random)")
    print("  • Barabási-Albert (Scale-Free)")
    print("\nNote: Requires Python packages: networkx, numpy, pandas, matplotlib")
    print(f"{Colors.YELLOW}Install with: pip install networkx numpy pandas matplotlib{Colors.END}\n")
    
    proceed = input("Continue? (y/n): ").strip().lower()
    if proceed == 'y':
        run_command("python3 generate.py", "Generating networks...")

def analyze_networks():
    print_header()
    print(f"{Colors.BOLD}🔬 NETWORK ANALYSIS{Colors.END}\n")
    print("Select network to analyze:")
    print("[1] Watts-Strogatz (Small-World)")
    print("[2] Erdős-Rényi (Random)")
    print("[3] Barabási-Albert (Scale-Free)")
    print("[4] All Networks")
    
    choice = input("\nEnter choice (1-4): ").strip()
    
    data_dir = "small_world_analysis_data"
    networks = {
        '1': ('data_proof_WS.txt', 'Watts-Strogatz'),
        '2': ('data_proof_ER.txt', 'Erdős-Rényi'),
        '3': ('data_proof_BA.txt', 'Barabási-Albert')
    }
    
    if choice in networks:
        file, name = networks[choice]
        cmd = f"./network_analyzer {data_dir}/{file}"
        run_command(cmd, f"Analyzing {name} network...")
    elif choice == '4':
        for key in ['1', '2', '3']:
            file, name = networks[key]
            cmd = f"./network_analyzer {data_dir}/{file}"
            run_command(cmd, f"Analyzing {name} network...")

def disease_simulation():
    print_header()
    print(f"{Colors.BOLD}🦠 DISEASE SPREAD SIMULATION{Colors.END}\n")
    print("SIR Model: Susceptible → Infected → Recovered")
    print("\nSelect network:")
    print("[1] Small-World (WS) - Fast spread")
    print("[2] Random (ER) - Moderate spread")
    print("[3] Scale-Free (BA) - Very fast spread")
    
    choice = input("\nEnter choice (1-3): ").strip()
    
    networks = {
        '1': 'data_proof_WS.txt',
        '2': 'data_proof_ER.txt',
        '3': 'data_proof_BA.txt'
    }
    
    if choice in networks:
        print(f"\n{Colors.CYAN}Parameters:{Colors.END}")
        infection = input("Infection probability (default 0.3): ").strip() or "0.3"
        recovery = input("Recovery probability (default 0.1): ").strip() or "0.1"
        sims = input("Number of simulations (default 10): ").strip() or "10"
        
        cmd = f"./disease_spread small_world_analysis_data/{networks[choice]} {infection} {recovery} {sims}"
        run_command(cmd, "Running disease spread simulation...")

def social_influence():
    print_header()
    print(f"{Colors.BOLD}📱 SOCIAL INFLUENCE SIMULATION{Colors.END}\n")
    print("Select simulation mode:")
    print("[1] Threshold Model - Peer pressure adoption")
    print("[2] Viral Marketing - Influencer targeting")
    
    mode_choice = input("\nEnter choice (1-2): ").strip()
    
    mode = 'threshold' if mode_choice == '1' else 'viral'
    
    print("\nSelect network:")
    print("[1] Small-World (WS)")
    print("[2] Random (ER)")
    print("[3] Scale-Free (BA)")
    
    net_choice = input("\nEnter choice (1-3): ").strip()
    
    networks = {
        '1': 'data_proof_WS.txt',
        '2': 'data_proof_ER.txt',
        '3': 'data_proof_BA.txt'
    }
    
    if net_choice in networks:
        cmd = f"./social_influence small_world_analysis_data/{networks[net_choice]} {mode}"
        run_command(cmd, f"Running {mode} simulation...")

def cooperation_simulation():
    print_header()
    print(f"{Colors.BOLD}🤝 COOPERATION EVOLUTION{Colors.END}\n")
    print("Prisoner's Dilemma on Networks")
    print("\nSelect network:")
    print("[1] Small-World (WS) - High cooperation")
    print("[2] Random (ER) - Low cooperation")
    print("[3] Scale-Free (BA) - Variable cooperation")
    
    choice = input("\nEnter choice (1-3): ").strip()
    
    networks = {
        '1': 'data_proof_WS.txt',
        '2': 'data_proof_ER.txt',
        '3': 'data_proof_BA.txt'
    }
    
    if choice in networks:
        print(f"\n{Colors.CYAN}Parameters:{Colors.END}")
        init_coop = input("Initial cooperation rate (default 0.5): ").strip() or "0.5"
        gens = input("Generations (default 100): ").strip() or "100"
        
        cmd = f"./cooperation small_world_analysis_data/{networks[choice]} {init_coop} {gens}"
        run_command(cmd, "Running cooperation evolution...")

def transport_simulation():
    print_header()
    print(f"{Colors.BOLD}🚗 TRANSPORT SIMULATION{Colors.END}\n")
    print("Select analysis mode:")
    print("[1] Routing Efficiency - Packet delivery")
    print("[2] Hub Vulnerability - Critical node analysis")
    
    mode_choice = input("\nEnter choice (1-2): ").strip()
    
    mode = 'routing' if mode_choice == '1' else 'vulnerability'
    
    print("\nSelect network:")
    print("[1] Small-World (WS)")
    print("[2] Random (ER)")
    print("[3] Scale-Free (BA)")
    
    net_choice = input("\nEnter choice (1-3): ").strip()
    
    networks = {
        '1': 'data_proof_WS.txt',
        '2': 'data_proof_ER.txt',
        '3': 'data_proof_BA.txt'
    }
    
    if net_choice in networks:
        cmd = f"./transport small_world_analysis_data/{networks[net_choice]} {mode}"
        run_command(cmd, f"Running {mode} analysis...")

def compare_all():
    print_header()
    print(f"{Colors.BOLD}📊 COMPARING ALL NETWORK TYPES{Colors.END}\n")
    print("This will run disease spread on all three networks for comparison.\n")
    
    proceed = input("Continue? (y/n): ").strip().lower()
    if proceed == 'y':
        data_dir = "small_world_analysis_data"
        networks = [
            ('data_proof_WS.txt', 'Watts-Strogatz (Small-World)'),
            ('data_proof_ER.txt', 'Erdős-Rényi (Random)'),
            ('data_proof_BA.txt', 'Barabási-Albert (Scale-Free)')
        ]
        
        for file, name in networks:
            print(f"\n{Colors.YELLOW}{'='*60}{Colors.END}")
            print(f"{Colors.BOLD}Network: {name}{Colors.END}")
            print(f"{Colors.YELLOW}{'='*60}{Colors.END}\n")
            cmd = f"./disease_spread {data_dir}/{file} 0.3 0.1 5"
            subprocess.run(cmd, shell=True)
            time.sleep(2)
        
        input(f"\n{Colors.BOLD}Press Enter to continue...{Colors.END}")

def launch_dashboard():
    print_header()
    print(f"{Colors.BOLD}🌐 LAUNCHING WEB DASHBOARD{Colors.END}\n")
    print("Starting Flask web server...")
    print(f"\n{Colors.GREEN}The dashboard will open in your browser at: http://localhost:8080{Colors.END}")
    print(f"{Colors.YELLOW}Press Ctrl+C to stop the server{Colors.END}\n")
    
    time.sleep(2)
    
    try:
        subprocess.run("python3 web_dashboard.py", shell=True)
    except KeyboardInterrupt:
        print(f"\n\n{Colors.RED}Server stopped.{Colors.END}")
        input(f"\n{Colors.BOLD}Press Enter to continue...{Colors.END}")

def view_readme():
    print_header()
    run_command("cat README.md | less", "Viewing README documentation...")

def check_prerequisites():
    """Check if necessary files and programs exist"""
    issues = []
    
    # Check for compiled programs
    programs = ['network_analyzer', 'disease_spread', 'social_influence', 'cooperation', 'transport']
    for prog in programs:
        if not os.path.exists(prog):
            issues.append(f"Missing compiled program: {prog}")
    
    # Check for data directory
    if not os.path.exists('small_world_analysis_data'):
        issues.append("Data directory not found. Run option [1] to generate networks first.")
    
    return issues

def main():
    while True:
        print_header()
        
        # Check prerequisites
        issues = check_prerequisites()
        if issues:
            print(f"{Colors.YELLOW}⚠️  Setup Required:{Colors.END}")
            for issue in issues:
                print(f"   • {issue}")
            print(f"\n{Colors.CYAN}To compile programs, run: ./demo.sh{Colors.END}")
            print(f"{Colors.CYAN}To generate data, select option [1]{Colors.END}\n")
        
        print_menu()
        
        choice = input(f"{Colors.BOLD}Enter your choice (0-9): {Colors.END}").strip()
        
        if choice == '0':
            print_header()
            print(f"{Colors.GREEN}Thank you for using Small-World Network Analyzer!{Colors.END}\n")
            sys.exit(0)
        elif choice == '1':
            generate_networks()
        elif choice == '2':
            analyze_networks()
        elif choice == '3':
            disease_simulation()
        elif choice == '4':
            social_influence()
        elif choice == '5':
            cooperation_simulation()
        elif choice == '6':
            transport_simulation()
        elif choice == '7':
            compare_all()
        elif choice == '8':
            launch_dashboard()
        elif choice == '9':
            view_readme()
        else:
            print(f"\n{Colors.RED}Invalid choice. Please try again.{Colors.END}")
            time.sleep(1)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print(f"\n\n{Colors.RED}Program interrupted by user.{Colors.END}")
        sys.exit(0)
