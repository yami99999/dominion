#include "BasicCards.h"
#include "Player.h"
#include "Game.h"

// Fonction : Augmenter les pièces d'or du joueur
//  作用: 增加玩家金币

void Copper::play(Player &player, Game &game)
{
    player.addCoin(1); // Ajouter 1 pièce d'or
                       //  增加1金币
}

void Silver::play(Player &player, Game &game)
{
    player.addCoin(2); // Ajouter 2 pièces d'or
                       //  增加2金币
}

void Gold::play(Player &player, Game &game)
{
    player.addCoin(3);
}