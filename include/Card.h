#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <nlohmann/json.hpp>

// déclaration avant
//  前向声明
class Player;
class Game;

// Utilisez nlohmann::json
//  使用 nlohmann::json
using json = nlohmann::json;

// Énumération : type de carte
//  枚举: 卡片类型
enum class CardType
{
    TREASURE,
    VICTORY,
    ACTION
};

// Classe de carte
//  卡片类
class Card
{
protected:
    // Attributs communs -partagés par les sous-classes
    // nom
    // dépenser
    // taper
    //  共同属性 - 子类共享
    // 名称
    // 花费
    // 类型
    std::string name;
    int cost;
    CardType type;

public:
    // Constructeur
    //  构造函数
    Card(std::string name, int cost, CardType type)
        : name(name), cost(cost), type(type) {}

    // destructeur virtuel
    //  虚析构函数
    virtual ~Card() = default;

    // Fonction virtuelle pure -les sous-classes doivent implémenter
    //  纯虚函数 - 子类必须实现
    virtual void play(Player &player, Game &game) = 0; // Exécuter l'effet de la carte
                                                       //  执行卡片效果
    virtual std::string toString() const { return name; } // Récupère le nom de la carte
    // 获取卡片名称
    virtual int getVictoryPoints() const { return 0; } // Obtenez des points de victoire de carte
    // 获取卡片胜利点数
    virtual std::string getDescription() const = 0; // Récupère la description de la carte
                                                    //  获取卡片描述

    // Méthode de sérialisation JSON
    //  JSON序列化方法
    virtual json toJson() const
    {
        json j;
        j["name"] = name;
        j["cost"] = cost;
        j["type"] = static_cast<int>(type);
        return j;
    }

    // méthode de fabrique statique
    //  静态工厂方法
    static std::shared_ptr<Card> fromJson(const json &j); // Créer des cartes à partir de JSON
                                                          //  从JSON中创建卡片
    static std::shared_ptr<Card> createCard(const std::string &cardName); // Créer une carte basée sur le nom
                                                                          //  根据名称创建卡片

    // getteurs
    //  Getters
    std::string getName() const { return name; } // Récupère le nom de la carte
    // 获取卡片名称
    int getCost() const { return cost; } // Obtenez le coût de la carte
    // 获取卡片花费
    CardType getType() const { return type; } // Récupère le type de carte
    // 获取卡片类型

    // inutile
    // Ajouter des déclarations pour les méthodes de sérialisation et de désérialisation
    // virtual void serialize(std::ofstream& out) const;
    // désérialisation du vide virtuel (std::ifstream& in);
    //  useless
    // 添加序列化和反序列化方法的声明
    // virtual void serialize(std::ofstream& out) const;
    // virtual void deserialize(std::ifstream& in);
};