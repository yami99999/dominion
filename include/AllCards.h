#pragma once

////Bibliothèques standards couramment utilisées
// // 常用标准库
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <fstream>

// Système de carte de base
//  核心卡牌系统
#include "Card.h"

// Type de carte de base
//  基础卡牌类型
#include "BasicCards.h" //Cartes au trésor de base, cartes de points de victoire de base -pièces de cuivre, pièces d'argent, pièces d'or, manoirs, ducs, provinces, jardins, malédictions
// 基础财宝牌、基础胜利点数牌 - 铜币、银币、金币、庄园、公爵、省份、花园、诅咒
#include "ActionCards.h" //Cartes d'action de base -Village, Marché, Artisan, Ferme forestière, Milice, Maison du Conseil, Changeur d'argent, Tranchée, Atelier, Cave à vin, Sorcière, Voleur, Festin, Laboratoire
// 基础行动牌 - 村庄、市场、工匠、林场、民兵、议事厅、钱币兑换商、壕沟、工坊、酒窖、女巫、窃贼、盛宴、实验室

// Composants du système de jeu
//  游戏系统组件
#include "Player.h" //Lié au joueur
// 玩家相关
#include "Game.h" //Logique de base du jeu
// 游戏核心逻辑
#include "Utils.h" //Fonction utilitaire
// 工具函数
#include "GameLogger.h" //journal de jeu
// 游戏日志

// Assurez-vous que tous les types de cartes sont correctement inclus -utilisez des pointeurs intelligents pour gérer les cartes
// tapez l'alias
//  确保所有卡片类型都被正确包含 - 使用智能指针管理卡牌
//  类型别名
using CardPtr = std::shared_ptr<Card>;
