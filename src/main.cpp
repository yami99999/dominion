#include <iostream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include "Game.h"
#include "Utils.h"

void displayWelcome() {
    std::cout << "\n";
    std::cout << "================================\n";
    std::cout << "       Bienvenue dans Dominion!\n";
    std::cout << "================================\n\n";
}

int main(int argc, char* argv[]) {
    displayWelcome();
    
    // 处理命令行参数 Traitement des arguments de la ligne de commande
    bool randomKingdom = false;
    std::string kingdomSet;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--random-kingdom") {
            randomKingdom = true;
        } else if (arg == "--kingdom-set" && i + 1 < argc) {
            kingdomSet = argv[i + 1];
            i++;
        }
    }
    
    // Demander s'il faut charger l'archive  询问是否加载存档
    std::cout << "Voulez-vous charger une sauvegarde ? (y/n): ";
    std::string response;
    std::getline(std::cin, response);
    
    if (Utils::equalIgnoreCase(response, "y")) {
        std::cout << "Entrez le nom du fichier de sauvegarde (par défaut 'dominion_save.txt'): ";
        std::string filename;
        std::getline(std::cin, filename);
        if (filename.empty()) {
            filename = "dominion_save.txt";
        }
        
        try {
            Game game(0);  // Créer une instance de jeu temporaire  创建一个临时游戏实例
            game.loadGame(filename);
            game.playGame();
            return 0;
        } catch (const std::exception& e) {
            std::cout << "chec du chargement de la sauvegarde : " << e.what() << "\n";
            std::cout << "Voulez-vous commencer une nouvelle partie ? (y/n): ";
            std::getline(std::cin, response);
            if (!Utils::equalIgnoreCase(response, "y") && !Utils::equalIgnoreCase(response, "yes")) {
                return 0;
            }
        }
    }
    
    // Commencer un nouveau jeu  开始新游戏
    int playerCount;
    do {
        std::cout << "\nEntrez le nombre de joueurs (2-4): ";
        std::cin >> playerCount;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } while (playerCount < 2 || playerCount > 4);
    
    try {
        Game game(playerCount, randomKingdom, kingdomSet);
        game.playGame();
    } catch (const std::exception& e) {
        std::cout << "Erreur lors de l'exécution du jeu : " << e.what() << "\n";
        return 1;
    }
    
    return 0;
} 