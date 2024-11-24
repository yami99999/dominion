#pragma once
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GameLogger {
private:
    std::vector<std::string> logs;
    std::map<std::string, int> cardsBought;
    std::map<std::string, int> cardsPlayed;
    std::time_t startTime;

public:
    GameLogger() : startTime(std::time(nullptr)) {}

    // 日志记录方法
    void logAction(const std::string& playerName, const std::string& action);
    void recordCardBought(const std::string& cardName);
    void recordCardPlayed(const std::string& cardName);
    void logKingdomCardSelection(const std::vector<std::string>& cards);

    // JSON序列化方法
    json toJson() const;
    void fromJson(const json& j);

    // 获取器
    const std::vector<std::string>& getLogs() const { return logs; }
    const std::map<std::string, int>& getCardsBought() const { return cardsBought; }
    const std::map<std::string, int>& getCardsPlayed() const { return cardsPlayed; }
}; 