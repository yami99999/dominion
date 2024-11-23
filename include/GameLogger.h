#pragma once
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GameLogger {
private:
    std::vector<std::string> logs;
    std::map<std::string, int> cardsBought;  // 统计购买的卡片
    std::map<std::string, int> cardsPlayed;  // 统计使用的卡片
    std::time_t startTime;
    
public:
    GameLogger() : startTime(std::time(nullptr)) {}
    
    void logAction(const std::string& playerName, const std::string& action) const {
        std::time_t now = std::time(nullptr);
        char timeStr[20];
        strftime(timeStr, sizeof(timeStr), "%H:%M:%S", localtime(&now));
        
        std::string logEntry = std::string(timeStr) + " - " + playerName + ": " + action;
        const_cast<GameLogger*>(this)->logs.push_back(logEntry);
    }
    
    void recordCardBought(const std::string& cardName) {
        cardsBought[cardName]++;
    }
    
    void recordCardPlayed(const std::string& cardName) {
        cardsPlayed[cardName]++;
    }
    
    json getJson() const {
        json j;
        j["start_time"] = startTime;
        j["duration"] = std::time(nullptr) - startTime;
        j["logs"] = logs;
        j["statistics"]["cards_bought"] = cardsBought;
        j["statistics"]["cards_played"] = cardsPlayed;
        return j;
    }
    
    void loadFromJson(const json& j) {
        try {
            startTime = j["start_time"];
            logs = j["logs"].get<std::vector<std::string>>();
            cardsBought = j["statistics"]["cards_bought"].get<std::map<std::string, int>>();
            cardsPlayed = j["statistics"]["cards_played"].get<std::map<std::string, int>>();
        } catch (const json::exception& e) {
            throw std::runtime_error("Échec du chargement des données du journal: " + std::string(e.what())); // 加载日志失败
        }
    }
    
    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename);
        if (out) {
            json j = getJson();
            out << std::setw(4) << j << std::endl;
        }
    }
    
    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename);
        if (!in) {
            throw std::runtime_error("Impossible d'ouvrir le fichier journal: " + filename); // 无法打开日志文件:
        }
        
        try {
            json j = json::parse(in);
            loadFromJson(j);
        } catch (const json::exception& e) {
            throw std::runtime_error("Échec de l'analyse du fichier journal: " + std::string(e.what())); // 解析日志文件失败
        }
    }
    
    json getStatistics() const {
        json stats;
        stats["game_duration"]["seconds"] = std::time(nullptr) - startTime;
        stats["cards_bought"] = cardsBought;
        stats["cards_played"] = cardsPlayed;
        stats["total_actions"] = logs.size();
        return stats;
    }
    
    std::vector<std::string> getRecentLogs(int count = 5) const {
        std::vector<std::string> recent;
        auto start = logs.size() > count ? logs.end() - count : logs.begin();
        recent.assign(start, logs.end());
        return recent;
    }
    
    const std::vector<std::string>& getLogs() const { return logs; }
    
    void clear() {
        logs.clear();
        cardsBought.clear();
        cardsPlayed.clear();
        startTime = std::time(nullptr);
    }
}; 