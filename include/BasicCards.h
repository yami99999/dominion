#pragma once
#include "Card.h"
#include "Player.h"
#include "Game.h"

// 铜币卡
class Copper : public Card {
public:
    Copper() : Card("Copper", 0, CardType::TREASURE) {}
    
    void play(Player& player, Game& game) override;
};

// 银币卡
class Silver : public Card {
public:
    Silver() : Card("Silver", 3, CardType::TREASURE) {}
    
    void play(Player& player, Game& game) override;
};

// 金币卡
class Gold : public Card {
public:
    Gold() : Card("Gold", 6, CardType::TREASURE) {}
    
    void play(Player& player, Game& game) override;
};

// 庄园卡
class Estate : public Card {
public:
    Estate() : Card("Estate", 2, CardType::VICTORY) {}
    
    void play(Player& player, Game& game) override {}
    int getVictoryPoints() const override { return 1; }
};

// 公爵领地卡
class Duchy : public Card {
public:
    Duchy() : Card("Duchy", 5, CardType::VICTORY) {}
    
    void play(Player& player, Game& game) override {}
    int getVictoryPoints() const override { return 3; }
};

// 行省卡
class Province : public Card {
public:
    Province() : Card("Province", 8, CardType::VICTORY) {}
    
    void play(Player& player, Game& game) override {}
    int getVictoryPoints() const override { return 6; }
};

// 诅咒卡 (-1 胜利点)
class Curse : public Card {
public:
    Curse() : Card("Curse", 0, CardType::VICTORY) {}
    
    void play(Player& player, Game& game) override {} // 诅咒卡不需要实现play
    int getVictoryPoints() const override { return -1; }  // 诅咒卡值-1分
}; 