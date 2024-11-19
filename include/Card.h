#pragma once
#include <string>
#include <memory>
#include <fstream>

// 前向声明
class Player;
class Game;

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
    
    // 序列化支持
    virtual void serialize(std::ofstream& out) const;
    virtual void deserialize(std::ifstream& in);
    
    // 工厂方法
    static std::shared_ptr<Card> createCard(const std::string& cardName);
    
    std::string getName() const { return name; }
    int getCost() const { return cost; }
    CardType getType() const { return type; }
}; 