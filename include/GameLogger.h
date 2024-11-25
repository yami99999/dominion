#pragma once
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GameLogger
{
private:
    // Conteneur pour stocker les journaux
    //  存储日志的容器
    std::vector<std::string> logs;
    // Conteneur pour stocker les cartes achetées
    //  存储购买的卡片的容器
    std::map<std::string, int> cardsBought;
    // Conteneur pour stocker les cartes utilisées
    //  存储使用过的卡片的容器
    std::map<std::string, int> cardsPlayed;
    // Variable pour enregistrer l'heure de début du jeu
    //  记录游戏开始时间的变量
    std::time_t startTime;

public:
    GameLogger() : startTime(std::time(nullptr)) {}

    // méthode de journalisation
    // Fonction : Enregistrer les actions du joueur -utilisé pour enregistrer les actions du joueur
    //  日志记录方法
    // 作用: 记录玩家动作 - 用于记录玩家动作
    void logAction(const std::string &playerName, const std::string &action);
    // Fonction : Carte pour enregistrer les achats -Carte utilisée pour enregistrer les achats
    //  作用: 记录购买的卡片 - 用于记录购买的卡片
    void recordCardBought(const std::string &cardName);
    // Fonction : Enregistrer les cartes utilisées -Utilisé pour enregistrer les cartes utilisées
    //  作用: 记录使用过的卡片 - 用于记录使用过的卡片
    void recordCardPlayed(const std::string &cardName);
    // Fonction : Enregistrer la carte du Royaume sélectionnée -Utilisé pour enregistrer la carte du Royaume sélectionnée
    //  作用: 记录选择的王国卡 - 用于记录选择的王国卡
    void logKingdomCardSelection(const std::vector<std::string> &cards);

    // Méthode de sérialisation JSON
    // Fonction : Convertir les journaux au format JSON -Utilisé pour convertir les journaux au format JSON
    //  JSON序列化方法
    // 作用: 将日志转换为JSON格式 - 用于将日志转换为JSON格式
    json toJson() const;
    // Fonction : Restaurer les logs depuis le format JSON -Utilisé pour restaurer les logs depuis le format JSON
    //  作用: 从JSON格式中恢复日志 - 用于从JSON格式中恢复日志
    void fromJson(const json &j);

    // getteur
    // Fonction : Obtenir le journal -utilisé pour obtenir le journal
    //  获取器
    // 作用: 获取日志 - 用于获取日志
    const std::vector<std::string> &getLogs() const { return logs; }
    // Fonction : Récupérer la carte achetée -Utilisé pour récupérer la carte achetée
    //  作用: 获取购买的卡片 - 用于获取购买的卡片
    const std::map<std::string, int> &getCardsBought() const { return cardsBought; }
    // Fonction : Récupérer les cartes utilisées -Utilisé pour récupérer les cartes utilisées
    //  作用: 获取使用过的卡片 - 用于获取使用过的卡片
    const std::map<std::string, int> &getCardsPlayed() const { return cardsPlayed; }
};