#pragma once
#include <string>
#include <memory>
#include <fstream>
#include <nlohmann/json.hpp>

// 前向声明
class Player;
class Game;

// 使用 nlohmann::json
using json = nlohmann::json;

enum class CardType {
    TREASURE,
    VICTORY,
    ACTION
};

class Card {
protected:
    std::string name;
    int cost;
    CardType type;
    
public:
    Card(std::string name, int cost, CardType type)
        : name(name), cost(cost), type(type) {}
    virtual ~Card() = default;
    
    virtual void play(Player& player, Game& game) = 0;
    virtual std::string toString() const { return name; }
    virtual int getVictoryPoints() const { return 0; }
    virtual std::string getDescription() const = 0;
    
    // 添加序列化和反序列化方法的声明
    virtual void serialize(std::ofstream& out) const;
    virtual void deserialize(std::ifstream& in);
    
    // JSON序列化方法
    virtual json toJson() const {
        json j;
        j["name"] = name;
        j["cost"] = cost;
        j["type"] = static_cast<int>(type);
        return j;
    }
    
    // 静态工厂方法
    static std::shared_ptr<Card> fromJson(const json& j);
    static std::shared_ptr<Card> createCard(const std::string& cardName);
    
    // Getters
    std::string getName() const { return name; }
    int getCost() const { return cost; }
    CardType getType() const { return type; }
}; 