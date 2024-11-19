#include "BasicCards.h"
#include "Player.h"
#include "Game.h"

void Copper::play(Player& player, Game& game) {
    player.addCoin(1);
}

void Silver::play(Player& player, Game& game) {
    player.addCoin(2);
}

void Gold::play(Player& player, Game& game) {
    player.addCoin(3);
} 