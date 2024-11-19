#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <map>

class GameLogger {
private:
    std::vector<std::string> logs;
    std::map<std::string, int> cardsBought;  // 统计购买的卡片
    std::map<std::string, int> cardsPlayed;  // 统计使用的卡片
    std::time_t startTime;
    
public:
    GameLogger() : startTime(std::time(nullptr)) {}
    
    void logAction(const std::string& playerName, const std::string& action) {
        std::time_t now = std::time(nullptr);
        char timeStr[20];
        strftime(timeStr, sizeof(timeStr), "%H:%M:%S", localtime(&now));
        
        std::string logEntry = std::string(timeStr) + " - " + playerName + ": " + action;
        logs.push_back(logEntry);
    }
    
    void recordCardBought(const std::string& cardName) {
        cardsBought[cardName]++;
    }
    
    void recordCardPlayed(const std::string& cardName) {
        cardsPlayed[cardName]++;
    }
    
    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename);
        if (!out) return;
        
        // 写入游戏时长
        std::time_t duration = std::time(nullptr) - startTime;
        out << "游戏时长: " << duration / 60 << "分" << duration % 60 << "秒\n\n";
        
        // 写入游戏日志
        out << "=== 游戏日志 ===\n";
        for (const auto& log : logs) {
            out << log << "\n";
        }
        
        // 写入统计信息
        out << "\n=== 统计信息 ===\n";
        out << "购买的卡片:\n";
        for (const auto& [card, count] : cardsBought) {
            out << card << ": " << count << "张\n";
        }
        
        out << "\n使用的卡片:\n";
        for (const auto& [card, count] : cardsPlayed) {
            out << card << ": " << count << "次\n";
        }
    }
    
    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename);
        if (!in) {
            throw std::runtime_error("无法打开日志文件");
        }
        
        logs.clear();
        cardsBought.clear();
        cardsPlayed.clear();
        
        std::string line;
        while (std::getline(in, line)) {
            if (line.find(" - ") != std::string::npos) {
                logs.push_back(line);
            }
        }
    }
    
    const std::vector<std::string>& getLogs() const { return logs; }
}; 