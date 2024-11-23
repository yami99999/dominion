#pragma once
#include "Card.h"
#include "Player.h"
#include "Game.h"

// 铜币卡
class Copper : public Card {
public:
    Copper() : Card("Copper", 0, CardType::TREASURE) {}
    void play(Player& player, Game& game) override;
    std::string getDescription() const override {
        return "Cuivre (0 pièces)\nEffet : +1 pièce\nDescription : Carte de base pour générer des pièces."; // 铜币 (0金币)\n效果: +1金币\n说明: 基础的金币卡
    }
};

// 银币卡
class Silver : public Card {
public:
    Silver() : Card("Silver", 3, CardType::TREASURE) {}
    void play(Player& player, Game& game) override;
    std::string getDescription() const override {
        return "Argent (3 pièces)\nEffet : +2 pièces\nDescription : Carte de niveau intermédiaire pour générer des pièces.";  // 银币 (3金币)\n效果: +2金币\n说明: 中级金币卡
    }
};

// 金币卡
class Gold : public Card {
public:
    Gold() : Card("Gold", 6, CardType::TREASURE) {}
    void play(Player& player, Game& game) override;
    std::string getDescription() const override {
        return "Or (6 pièces)\nEffet : +3 pièces\nDescription : Carte avancée pour générer des pièces."; // 金币 (6金币)\n效果: +3金币\n说明: 高级金币卡
    }
};

// 庄园卡
class Estate : public Card {
public:
    Estate() : Card("Estate", 2, CardType::VICTORY) {}
    void play(Player& player, Game& game) override {}
    int getVictoryPoints() const override { return 1; }
    std::string getDescription() const override {
        return "Domaine (2 pièces)\nEffet : 1 point de victoire\nDescription : Carte de base pour marquer des points de victoire."; // 庄园 (2金币)\n效果: 1胜利点\n说明: 基础的胜利点数卡
    }
};

// 公爵领地卡
class Duchy : public Card {
public:
    Duchy() : Card("Duchy", 5, CardType::VICTORY) {}
    void play(Player& player, Game& game) override {}
    int getVictoryPoints() const override { return 3; }
    std::string getDescription() const override {
        return "Duché (5 pièces)\nEffet : 3 points de victoire\nDescription : Carte intermédiaire pour marquer des points de victoire.";  // 公爵领地 (5金币)\n效果: 3胜利点\n说明: 中级胜利点数卡
    }
};

// 行省卡
class Province : public Card {
public:
    Province() : Card("Province", 8, CardType::VICTORY) {}
    void play(Player& player, Game& game) override {}
    int getVictoryPoints() const override { return 6; }
    std::string getDescription() const override {
        return "Province (8 pièces)\nEffet : 6 points de victoire\nDescription : Carte avancée pour marquer des points de victoire.";  // 行省 (8金币)\n效果: 6胜利点\n说明: 高级胜利点数卡
    }
};

// 诅咒卡
class Curse : public Card {
public:
    Curse() : Card("Curse", 0, CardType::VICTORY) {}
    void play(Player& player, Game& game) override {}
    int getVictoryPoints() const override { return -1; }
    std::string getDescription() const override {
        return "Malédiction (0 pièces)\nEffet : -1 point de victoire\nDescription : Carte à effet négatif."; // 诅咒 (0金币)\n效果: -1胜利点\n说明: 负面效果卡牌
    }
}; 