#pragma once
#include "Card.h"
#include "Player.h"
#include "Game.h"

// Carte pièce en cuivre
//  铜币卡
class Copper : public Card
{
public:
    // Constructeur : initialiser le nom de la carte, la consommation de pièces d'or, le type de carte
    //  构造函数: 初始化卡牌名称、金币消耗、卡牌类型
    Copper() : Card("Copper", 0, CardType::TREASURE) {}
    // Exécuter l'effet de la carte
    //  执行卡牌效果
    void play(Player &player, Game &game) override;
    // Récupère la description de la carte
    //  获取卡牌描述
    std::string getDescription() const override
    {
        return "Cuivre (0 pièces)\nEffet : +1 pièce\nDescription : Carte de base pour générer des pièces.";
    }
};

// Carte pièce d'argent
//  银币卡
class Silver : public Card
{
public:
    // Constructeur : initialiser le nom de la carte, la consommation de pièces d'or, le type de carte
    //  构造函数: 初始化卡牌名称、金币消耗、卡牌类型
    Silver() : Card("Silver", 3, CardType::TREASURE) {}
    // Exécuter l'effet de la carte
    //  执行卡牌效果
    void play(Player &player, Game &game) override;
    // Récupère la description de la carte
    //  获取卡牌描述
    std::string getDescription() const override
    {
        return "Argent (3 pièces)\nEffet : +2 pièces\nDescription : Carte intermédiaire pour générer des pièces.";
    }
};

// carte pièce d'or
//  金币卡
class Gold : public Card
{
public:
    // Constructeur : initialiser le nom de la carte, la consommation de pièces d'or, le type de carte
    //  构造函数: 初始化卡牌名称、金币消耗、卡牌类型
    Gold() : Card("Gold", 6, CardType::TREASURE) {}
    // Exécuter l'effet de la carte
    //  执行卡牌效果
    void play(Player &player, Game &game) override;
    // Récupère la description de la carte
    //  获取卡牌描述
    std::string getDescription() const override
    {
        return "Or (6 pièces)\nEffet : +3 pièces\nDescription : Carte avancée pour générer des pièces.";
    }
};

// Carte Manoir
//  庄园卡
class Estate : public Card
{
public:
    // Constructeur : initialiser le nom de la carte, la consommation de pièces d'or, le type de carte
    //  构造函数: 初始化卡牌名称、金币消耗、卡牌类型
    Estate() : Card("Estate", 2, CardType::VICTORY) {}
    // Exécuter l'effet de la carte
    //  执行卡牌效果
    void play(Player &player, Game &game) override {}
    // Obtenez des points de victoire
    //  获取胜利点数
    int getVictoryPoints() const override { return 1; }
    // Récupère la description de la carte
    //  获取卡牌描述
    std::string getDescription() const override
    {
        return "Domaine (2 pièces)\nEffet : 1 point de victoire\nDescription : Carte de base pour marquer des points de victoire.";
    }
};

// Carte Duché
//  公爵领地卡
class Duchy : public Card
{
public:
    // Constructeur : initialiser le nom de la carte, la consommation de pièces d'or, le type de carte
    //  构造函数: 初始化卡牌名称、金币消耗、卡牌类型
    Duchy() : Card("Duchy", 5, CardType::VICTORY) {}
    // Exécuter l'effet de la carte
    //  执行卡牌效果
    void play(Player &player, Game &game) override {}
    // Obtenez des points de victoire
    //  获取胜利点数
    int getVictoryPoints() const override { return 3; }
    // Récupère la description de la carte
    //  获取卡牌描述
    std::string getDescription() const override
    {
        return "Duché (5 pièces)\nEffet : 3 points de victoire\nDescription : Carte intermédiaire pour marquer des points de victoire.";
    }
};

// Carte provinciale
//  行省卡
class Province : public Card
{
public:
    // Constructeur : initialiser le nom de la carte, la consommation de pièces d'or, le type de carte
    //  构造函数: 初始化卡牌名称、金币消耗、卡牌类型
    Province() : Card("Province", 8, CardType::VICTORY) {}
    // Exécuter l'effet de la carte
    //  执行卡牌效果
    void play(Player &player, Game &game) override {}
    // Obtenez des points de victoire
    //  获取胜利点数
    int getVictoryPoints() const override { return 6; }
    // Récupère la description de la carte
    //  获取卡牌描述
    std::string getDescription() const override
    {
        return "Province (8 pièces)\nEffet : 6 points de victoire\nDescription : Carte avancée pour marquer des points de victoire.";
    }
};

// carte malédiction
//  诅咒卡
class Curse : public Card
{
public:
    // Constructeur : initialiser le nom de la carte, la consommation de pièces d'or, le type de carte
    //  构造函数: 初始化卡牌名称、金币消耗、卡牌类型
    Curse() : Card("Curse", 0, CardType::VICTORY) {}
    // Exécuter l'effet de la carte
    //  执行卡牌效果
    void play(Player &player, Game &game) override {}
    // Obtenez des points de victoire
    //  获取胜利点数
    int getVictoryPoints() const override { return -1; }
    // Récupère la description de la carte
    //  获取卡牌描述
    std::string getDescription() const override
    {
        return "Malédiction (0 pièces)\nEffet : -1 point de victoire\nDescription : Carte à effet négatif.";
    }
};