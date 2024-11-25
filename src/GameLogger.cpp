#include "GameLogger.h"
#include <iomanip>

// Fonction : Enregistrer les actions du joueur -utilisé pour enregistrer les actions du joueur
//  作用: 记录玩家动作 - 用于记录玩家动作
void GameLogger::logAction(const std::string &playerName, const std::string &action)
{
    std::time_t now = std::time(nullptr);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", localtime(&now));
    logs.push_back(std::string(timeStr) + " - " + playerName + ": " + action);
}

// Fonction : Carte pour enregistrer les achats -Carte utilisée pour enregistrer les achats
//  作用: 记录购买的卡片 - 用于记录购买的卡片
void GameLogger::recordCardBought(const std::string &cardName)
{
    cardsBought[cardName]++;
}

// Fonction : Enregistrer les cartes utilisées -Utilisé pour enregistrer les cartes utilisées
//  作用: 记录使用过的卡片 - 用于记录使用过的卡片
void GameLogger::recordCardPlayed(const std::string &cardName)
{
    cardsPlayed[cardName]++;
}

// Fonction : Enregistrer la carte du Royaume sélectionnée -Utilisé pour enregistrer la carte du Royaume sélectionnée
//  作用: 记录选择的王国卡 - 用于记录选择的王国卡
void GameLogger::logKingdomCardSelection(const std::vector<std::string> &cards)
{
    std::string cardList;
    for (size_t i = 0; i < cards.size(); i++)
    {
        if (i > 0)
            cardList += ", ";
        cardList += cards[i];
    }
    logAction("System", "Carte Royaume au choix: " + cardList);
}

// Fonction : Convertir les journaux au format JSON -Utilisé pour convertir les journaux au format JSON
//  作用: 将日志转换为JSON格式 - 用于将日志转换为JSON格式
json GameLogger::toJson() const
{
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

// Fonction : Restaurer les logs depuis le format JSON -Utilisé pour restaurer les logs depuis le format JSON
//  作用: 从JSON格式中恢复日志 - 用于从JSON格式中恢复日志
void GameLogger::fromJson(const json &j)
{
    startTime = j["start_time"];
    logs = j["logs"].get<std::vector<std::string>>();
    cardsBought = j["statistics"]["cards_bought"].get<std::map<std::string, int>>();
    cardsPlayed = j["statistics"]["cards_played"].get<std::map<std::string, int>>();
}