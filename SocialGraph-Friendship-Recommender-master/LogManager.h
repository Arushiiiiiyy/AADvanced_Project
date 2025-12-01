#pragma once
#include <vector>
#include <string>

struct LogEvent {
    std::string type;
    int nodeID;
    int fromID;      // Tracks the "Parent" node so we can light the edge
    double value;    // NEW: For scores (PageRank) or labels (Communities)
};

class LogManager {
public:
    inline static std::vector<LogEvent> events;
    inline static std::string algorithmName = "";
    inline static std::string algorithmDescription = "";
    inline static int targetUserID = -1;

    static void clear() { 
        events.clear();
        algorithmName = "";
        algorithmDescription = "";
        targetUserID = -1;
    }

    // Set algorithm metadata
    static void setAlgorithm(std::string name, std::string description, int userID = -1) {
        algorithmName = name;
        algorithmDescription = description;
        targetUserID = userID;
    }

    // Updated log function with optional 'fromID' and 'value'
    static void log(std::string type, int nodeID, int fromID = -1, double value = 0.0) {
        events.push_back({type, nodeID, fromID, value});
    }
};