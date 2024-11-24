#include "GameLogger.h"
#include <iomanip>

void GameLogger::logAction(const std::string& playerName, const std::string& action) {
    std::time_t now = std::time(nullptr);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", localtime(&now));
    logs.push_back(std::string(timeStr) + " - " + playerName + ": " + action);
}

void GameLogger::recordCardBought(const std::string& cardName) {
    cardsBought[cardName]++;
}

void GameLogger::recordCardPlayed(const std::string& cardName) {
    cardsPlayed[cardName]++;
}

void GameLogger::logKingdomCardSelection(const std::vector<std::string>& cards) {
    std::string cardList;
    for (size_t i = 0; i < cards.size(); i++) {
        if (i > 0) cardList += ", ";
        cardList += cards[i];
    }
    logAction("System", "Carte Royaume au choix: " + cardList);
}

json GameLogger::toJson() const {
    json j;
    j["start_time"] = startTime;
    j["duration"] = std::time(nullptr) - startTime;
    j["logs"] = logs;
    
    json stats;
    stats["cards_bought"] = cardsBought;
    stats["cards_played"] = cardsPlayed;
    j["statistics"] = stats;
    
    return j;
}

void GameLogger::fromJson(const json& j) {
    startTime = j["start_time"];
    logs = j["logs"].get<std::vector<std::string>>();
    cardsBought = j["statistics"]["cards_bought"].get<std::map<std::string, int>>();
    cardsPlayed = j["statistics"]["cards_played"].get<std::map<std::string, int>>();
} 