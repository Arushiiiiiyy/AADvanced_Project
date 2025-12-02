// Interactive Menu System for Small-World Network Analysis
// Provides a user-friendly interface for running all simulations

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <sys/stat.h>

using namespace std;

// ANSI color codes
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

bool fileExists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

bool directoryExists(const string& dirname) {
    struct stat buffer;
    return (stat(dirname.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printHeader() {
    cout << CYAN << BOLD;
    cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    cout << "║                                                                ║\n";
    cout << "║     SMALL-WORLD NETWORK PHENOMENA: REAL-WORLD SIMULATIONS     ║\n";
    cout << "║                                                                ║\n";
    cout << "║        Explore disease spread, social influence, and more!    ║\n";
    cout << "║                                                                ║\n";
    cout << "╚════════════════════════════════════════════════════════════════╝\n";
    cout << RESET << "\n";
}

void printMenu() {
    cout << BOLD << "\n━━━━━━━━━━━━━━━━━━━━━ MAIN MENU ━━━━━━━━━━━━━━━━━━━━━\n" << RESET;
    cout << "\n" << YELLOW << "📊 DATA GENERATION" << RESET << "\n";
    cout << "  1. Generate Network Datasets (Python)\n";
    cout << "  2. Check Data Status\n";
    
    cout << "\n" << GREEN << "📈 NETWORK ANALYSIS" << RESET << "\n";
    cout << "  3. Analyze Network Metrics (C & L)\n";
    cout << "  4. Compare All Network Types\n";
    
    cout << "\n" << RED << "🦠 REAL-WORLD SIMULATIONS" << RESET << "\n";
    cout << "  5. Disease Spread (Epidemic Model)\n";
    cout << "  6. Social Influence & Viral Marketing\n";
    cout << "  7. Cooperation Evolution (Game Theory)\n";
    cout << "  8. Transport & Routing Efficiency\n";
    cout << "  9. Hub Vulnerability Analysis\n";
    
    cout << "\n" << MAGENTA << "🌐 WEB DASHBOARD" << RESET << "\n";
    cout << "  10. Launch Web Dashboard (Interactive)\n";
    cout << "  11. Generate HTML Report\n";
    
    cout << "\n" << BLUE << "📚 HELP & INFO" << RESET << "\n";
    cout << "  12. About Small-World Networks\n";
    cout << "  13. View README\n";
    
    cout << "\n" << "  0. Exit\n";
    cout << "\n" << BOLD << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n" << RESET;
}

string selectNetworkType() {
    cout << "\n" << YELLOW << "Select Network Type:" << RESET << "\n";
    cout << "  1. Watts-Strogatz (Small-World)\n";
    cout << "  2. Erdős-Rényi (Random)\n";
    cout << "  3. Barabási-Albert (Scale-Free)\n";
    cout << "\nChoice [1-3]: ";
    
    int choice;
    cin >> choice;
    
    switch(choice) {
        case 1: return "small_world_analysis_data/data_proof_WS.txt";
        case 2: return "small_world_analysis_data/data_proof_ER.txt";
        case 3: return "small_world_analysis_data/data_proof_BA.txt";
        default: return "small_world_analysis_data/data_proof_WS.txt";
    }
}

void generateData() {
    cout << "\n" << CYAN << "═══ Generating Network Datasets ═══" << RESET << "\n\n";
    cout << "Running Python script to generate networks...\n\n";
    
    int result = system("python3 generate.py");
    
    if (result == 0) {
        cout << "\n" << GREEN << "✓ Data generated successfully!" << RESET << "\n";
    } else {
        cout << "\n" << RED << "✗ Error: Please install required packages:" << RESET << "\n";
        cout << "  pip install networkx numpy pandas matplotlib\n";
    }
    
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void checkDataStatus() {
    cout << "\n" << CYAN << "═══ Data Status Check ═══" << RESET << "\n\n";
    
    bool dirExists = directoryExists("small_world_analysis_data");
    
    if (!dirExists) {
        cout << RED << "✗ Data directory not found!" << RESET << "\n";
        cout << "  Please run option 1 to generate data.\n";
    } else {
        cout << GREEN << "✓ Data directory exists" << RESET << "\n\n";
        
        vector<string> files = {"data_proof_WS.txt", "data_proof_ER.txt", "data_proof_BA.txt"};
        vector<string> names = {"Watts-Strogatz (WS)", "Erdős-Rényi (ER)", "Barabási-Albert (BA)"};
        
        for (size_t i = 0; i < files.size(); i++) {
            string path = "small_world_analysis_data/" + files[i];
            if (fileExists(path)) {
                cout << "  " << GREEN << "✓" << RESET << " " << names[i] << ": " << path << "\n";
            } else {
                cout << "  " << RED << "✗" << RESET << " " << names[i] << ": NOT FOUND\n";
            }
        }
    }
    
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void analyzeMetrics() {
    string file = selectNetworkType();
    
    if (!fileExists(file)) {
        cout << RED << "\n✗ Error: Data file not found!" << RESET << "\n";
        cout << "Please generate data first (option 1).\n";
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }
    
    cout << "\n" << CYAN << "═══ Running Network Analysis ═══" << RESET << "\n\n";
    string cmd = "./network_analyzer " + file;
    system(cmd.c_str());
    
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void compareNetworks() {
    cout << "\n" << CYAN << "═══ Comparing All Network Types ═══" << RESET << "\n\n";
    
    vector<string> files = {"data_proof_WS.txt", "data_proof_ER.txt", "data_proof_BA.txt"};
    vector<string> names = {"WATTS-STROGATZ (Small-World)", "ERDŐS-RÉNYI (Random)", "BARABÁSI-ALBERT (Scale-Free)"};
    
    for (size_t i = 0; i < files.size(); i++) {
        string path = "small_world_analysis_data/" + files[i];
        if (fileExists(path)) {
            cout << YELLOW << "\n▶ " << names[i] << RESET << "\n";
            string cmd = "./network_analyzer " + path;
            system(cmd.c_str());
        }
    }
    
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void diseaseSpread() {
    string file = selectNetworkType();
    
    if (!fileExists(file)) {
        cout << RED << "\n✗ Error: Data file not found!" << RESET << "\n";
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }
    
    cout << "\n" << CYAN << "═══ Disease Spread Simulation ═══" << RESET << "\n\n";
    cout << "Parameters:\n";
    cout << "  Infection probability (0.0-1.0) [default 0.3]: ";
    
    string input;
    cin.ignore();
    getline(cin, input);
    double infect = input.empty() ? 0.3 : stod(input);
    
    cout << "  Recovery probability (0.0-1.0) [default 0.1]: ";
    getline(cin, input);
    double recover = input.empty() ? 0.1 : stod(input);
    
    cout << "  Number of simulations [default 10]: ";
    getline(cin, input);
    int sims = input.empty() ? 10 : stoi(input);
    
    cout << "\n";
    stringstream cmd;
    cmd << "./disease_spread " << file << " " << infect << " " << recover << " " << sims;
    system(cmd.str().c_str());
    
    cout << "\nPress Enter to continue...";
    cin.get();
}

void socialInfluence() {
    string file = selectNetworkType();
    
    if (!fileExists(file)) {
        cout << RED << "\n✗ Error: Data file not found!" << RESET << "\n";
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }
    
    cout << "\n" << CYAN << "═══ Social Influence Simulation ═══" << RESET << "\n\n";
    cout << "Select mode:\n";
    cout << "  1. Threshold Model (peer pressure)\n";
    cout << "  2. Viral Marketing (influencer targeting)\n";
    cout << "Choice [1-2]: ";
    
    int choice;
    cin >> choice;
    
    string mode = (choice == 2) ? "viral" : "threshold";
    
    cout << "\n";
    string cmd = "./social_influence " + file + " " + mode;
    system(cmd.c_str());
    
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void cooperation() {
    string file = selectNetworkType();
    
    if (!fileExists(file)) {
        cout << RED << "\n✗ Error: Data file not found!" << RESET << "\n";
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }
    
    cout << "\n" << CYAN << "═══ Cooperation Evolution ═══" << RESET << "\n\n";
    cout << "Initial cooperation probability (0.0-1.0) [default 0.5]: ";
    
    string input;
    cin.ignore();
    getline(cin, input);
    double prob = input.empty() ? 0.5 : stod(input);
    
    cout << "Number of generations [default 100]: ";
    getline(cin, input);
    int gens = input.empty() ? 100 : stoi(input);
    
    cout << "\n";
    stringstream cmd;
    cmd << "./cooperation " << file << " " << prob << " " << gens;
    system(cmd.str().c_str());
    
    cout << "\nPress Enter to continue...";
    cin.get();
}

void transport() {
    string file = selectNetworkType();
    
    if (!fileExists(file)) {
        cout << RED << "\n✗ Error: Data file not found!" << RESET << "\n";
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }
    
    cout << "\n" << CYAN << "═══ Transport Efficiency ═══" << RESET << "\n\n";
    string cmd = "./transport " + file + " routing";
    system(cmd.c_str());
    
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void vulnerability() {
    string file = selectNetworkType();
    
    if (!fileExists(file)) {
        cout << RED << "\n✗ Error: Data file not found!" << RESET << "\n";
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
        return;
    }
    
    cout << "\n" << CYAN << "═══ Hub Vulnerability Analysis ═══" << RESET << "\n\n";
    string cmd = "./transport " + file + " vulnerability";
    system(cmd.c_str());
    
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void launchDashboard() {
    cout << "\n" << CYAN << "═══ Launching Web Dashboard ═══" << RESET << "\n\n";
    cout << "Starting web server...\n\n";
    
    if (fileExists("dashboard.html")) {
        cout << GREEN << "✓ Opening dashboard in browser..." << RESET << "\n\n";
        #ifdef __APPLE__
            system("open dashboard.html");
        #elif __linux__
            system("xdg-open dashboard.html");
        #else
            system("start dashboard.html");
        #endif
        
        cout << "Dashboard opened in your default browser.\n";
        cout << "You can also manually open: dashboard.html\n";
    } else {
        cout << YELLOW << "! Dashboard file not found. Generating now..." << RESET << "\n";
        system("python3 create_dashboard.py");
    }
    
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void generateReport() {
    cout << "\n" << CYAN << "═══ Generating HTML Report ═══" << RESET << "\n\n";
    cout << "Creating comprehensive analysis report...\n\n";
    
    system("python3 create_dashboard.py");
    
    cout << "\n" << GREEN << "✓ Report generated: dashboard.html" << RESET << "\n";
    cout << "Open it in your browser to view interactive visualizations.\n";
    
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void aboutSmallWorld() {
    clearScreen();
    cout << CYAN << BOLD;
    cout << "\n╔════════════════════════════════════════════════════════════════╗\n";
    cout << "║              ABOUT SMALL-WORLD NETWORKS                        ║\n";
    cout << "╚════════════════════════════════════════════════════════════════╝\n";
    cout << RESET << "\n";
    
    cout << YELLOW << "What are Small-World Networks?" << RESET << "\n";
    cout << "Small-world networks are characterized by:\n";
    cout << "  • " << GREEN << "High Clustering (C)" << RESET << " - Strong local communities\n";
    cout << "  • " << GREEN << "Short Path Lengths (L)" << RESET << " - Fast global connectivity\n\n";
    
    cout << YELLOW << "Real-World Examples:" << RESET << "\n";
    cout << "  🧠 Neural networks in the brain\n";
    cout << "  👥 Social networks (6 degrees of separation)\n";
    cout << "  🌐 The Internet and World Wide Web\n";
    cout << "  ⚡ Power grids\n";
    cout << "  🦠 Disease transmission networks\n\n";
    
    cout << YELLOW << "Why Do They Matter?" << RESET << "\n";
    cout << "  • Enable " << GREEN << "efficient information spread" << RESET << "\n";
    cout << "  • Balance " << BLUE << "local + global connectivity" << RESET << "\n";
    cout << "  • Explain " << MAGENTA << "rapid epidemic spread" << RESET << "\n";
    cout << "  • Guide " << CYAN << "viral marketing strategies" << RESET << "\n\n";
    
    cout << YELLOW << "Key Discovery:" << RESET << "\n";
    cout << "Watts & Strogatz (1998) showed that just a few random\n";
    cout << "\"shortcuts\" in a regular network create small-world properties!\n\n";
    
    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

void viewReadme() {
    clearScreen();
    cout << CYAN << "═══ README Content ═══" << RESET << "\n\n";
    system("cat README.md | head -100");
    cout << "\n\n" << YELLOW << "(Scroll up to see full content)" << RESET << "\n";
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

int main() {
    int choice;
    
    while (true) {
        clearScreen();
        printHeader();
        printMenu();
        
        cout << "\n" << BOLD << "Enter your choice: " << RESET;
        cin >> choice;
        
        switch (choice) {
            case 0:
                cout << "\n" << GREEN << "Thank you for exploring Small-World Networks! 👋\n" << RESET;
                return 0;
                
            case 1:
                generateData();
                break;
                
            case 2:
                checkDataStatus();
                break;
                
            case 3:
                analyzeMetrics();
                break;
                
            case 4:
                compareNetworks();
                break;
                
            case 5:
                diseaseSpread();
                break;
                
            case 6:
                socialInfluence();
                break;
                
            case 7:
                cooperation();
                break;
                
            case 8:
                transport();
                break;
                
            case 9:
                vulnerability();
                break;
                
            case 10:
                launchDashboard();
                break;
                
            case 11:
                generateReport();
                break;
                
            case 12:
                aboutSmallWorld();
                break;
                
            case 13:
                viewReadme();
                break;
                
            default:
                cout << "\n" << RED << "Invalid choice! Please try again." << RESET << "\n";
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
        }
    }
    
    return 0;
}
