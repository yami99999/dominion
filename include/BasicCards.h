#pragma once
#include "Card.h"
#include "Player.h"
#include "Game.h"

// 铜币卡 Copper Cuivre
class Copper : public Card {
public:
    Copper() : Card("Copper", 0, CardType::TREASURE) {}
    void play(Player& player, Game& game) override;
    std::string getDescription() const override {
        return "Cuivre (0 pièces)\nEffet : +1 pièce\nDescription : Carte de base pour générer des pièces.";
    }
};

// 银币卡 Silver Argent
class Silver : public Card {
public:
    Silver() : Card("Silver", 3, CardType::TREASURE) {}
    void play(Player& player, Game& game) override;
    std::string getDescription() const override {
        return "Argent (3 pièces)\nEffet : +2 pièces\nDescription : Carte intermédiaire pour générer des pièces.";
    }
};

// 金币卡 Gold Or
class Gold : public Card {
public:
    Gold() : Card("Gold", 6, CardType::TREASURE) {}
    void play(Player& player, Game& game) override;
    std::string getDescription() const override {
        return "Or (6 pièces)\nEffet : +3 pièces\nDescription : Carte avancée pour générer des pièces.";
    }
};

// 庄园卡 Estate Domaine 
class Estate : public Card {
public:
    Estate() : Card("Estate", 2, CardType::VICTORY) {}
    void play(Player& player, Game& game) override {}
    int getVictoryPoints() const override { return 1; }
    std::string getDescription() const override {
        return "Domaine (2 pièces)\nEffet : 1 point de victoire\nDescription : Carte de base pour marquer des points de victoire.";
    }
};

// 公爵领地卡 Duché
class Duchy : public Card {
public:
    Duchy() : Card("Duchy", 5, CardType::VICTORY) {}
    void play(Player& player, Game& game) override {}
    int getVictoryPoints() const override { return 3; }
    std::string getDescription() const override {
        return "Duché (5 pièces)\nEffet : 3 points de victoire\nDescription : Carte intermédiaire pour marquer des points de victoire.";
    }
};

// 行省卡 Provinces
class Province : public Card {
public:
    Province() : Card("Province", 8, CardType::VICTORY) {}
    void play(Player& player, Game& game) override {}
    int getVictoryPoints() const override { return 6; }
    std::string getDescription() const override {
        return "Province (8 pièces)\nEffet : 6 points de victoire\nDescription : Carte avancée pour marquer des points de victoire.";
    }
};

// 诅咒卡 Malédiction
class Curse : public Card {
public:
    Curse() : Card("Curse", 0, CardType::VICTORY) {}
    void play(Player& player, Game& game) override {}
    int getVictoryPoints() const override { return -1; }
    std::string getDescription() const override {
        return "Malédiction (0 pièces)\nEffet : -1 point de victoire\nDescription : Carte à effet négatif.";
    }
}; 