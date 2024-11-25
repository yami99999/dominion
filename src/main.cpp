#include <iostream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include "Game.h"
#include "Utils.h"

void displayWelcome()
{
    std::cout << "\n";
    std::cout << "================================\n";
    std::cout << "       Bienvenue dans Dominion!\n";
    std::cout << "================================\n\n";
}

int main(int argc, char *argv[])
{
    // 1. Afficher le message de bienvenue
    //  1.显示欢迎信息
    displayWelcome();

    bool randomKingdom = false;  // Est-ce un royaume aléatoire ?
                                 //  是否随机王国
    std::string kingdomSet = ""; // Le jeu de cartes du royaume est initialement vide
                                 //  王国卡集初始为空

    // 2. Demander s'il faut charger l'archive
    //  2.询问是否加载存档
    std::cout << "Voulez-vous charger une sauvegarde ? (y/n): ";
    std::string response;
    std::getline(std::cin, response);

    if (Utils::equalIgnoreCase(response, "y"))
    {
        std::cout << "Entrez le nom du fichier de sauvegarde (par défaut 'dominion_save.txt'): ";
        std::string filename;
        std::getline(std::cin, filename);

        // Si l'utilisateur ne saisit pas de nom de fichier, utilisez le nom de fichier par défaut
        //  如果用户没有输入文件名，则使用默认文件名
        if (filename.empty())
        {
            filename = "dominion_save.txt";
        }

        // 2.1 Charger les archives
        //  2.1加载存档
        try
        {

            Game game(0, true);      // Ne précisez pas encore le nombre de joueurs, car lors du chargement de l'archive, le nombre de joueurs peut être différent de celui dans l'archive
                                     //  暂时不指定玩家数量，因为加载存档时，玩家数量可能与存档中的数量不同
            game.loadGame(filename); // Charger les archives
                                     //  加载存档
            game.playGame();         // Commencer le jeu
                                     //  开始游戏
            return 0;
        }
        // 2.2 Échec du chargement de l'archive
        //  2.2加载存档失败
        catch (const std::exception &e)
        {
            std::cout << "chec du chargement de la sauvegarde : " << e.what() << "\n";
            std::cout << "Voulez-vous commencer une nouvelle partie ? (y/n): ";
            std::getline(std::cin, response);
            // Si l'entrée utilisateur n'est pas y ou yes, quittez le jeu
            //  如果用户输入的不是 y 或 yes，则退出游戏
            if (!Utils::equalIgnoreCase(response, "y") && !Utils::equalIgnoreCase(response, "yes"))
            {
                return 0;
            }
        }
    }
    // 2.2 + oui => 3.
    // 3. Démarrez un nouveau jeu sans charger l'archive
    //  2.2 + yes => 3.
    // 3.不加载存档则开始新游戏
    int playerCount;
    do
    { // Nombre de joueurs
        // 玩家数量
        std::cout << "\nEntrez le nombre de joueurs (2-4): ";
        std::cin >> playerCount;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } while (playerCount < 2 || playerCount > 4);

    // 4. Démarrer une nouvelle partie
    //  4.开始新游戏
    try
    {
        // Créer une partie (nombre de joueurs, chargement ou non des archives -faux, jeu de cartes royaume -vide par défaut)
        // Objectif : créer un joueur + méthode de collecte des cartes du royaume + initialiser le jeu (pile de ravitaillement + pile de cartes de joueur)
        //  创建游戏(玩家数量, 是否加载存档-false, 王国卡集-默认为空)
        // 目标: 创建玩家 + 王国卡集方式 + 初始化游戏(供应堆 + 玩家牌堆)
        Game game(playerCount, false, kingdomSet);

        // Commencer le jeu
        //  开始游戏
        game.playGame();
    }

    // Une erreur s'est produite lors de l'exécution du jeu
    //  游戏运行时发生错误
    catch (const std::exception &e)
    {
        std::cout << "Erreur lors de l'exécution du jeu : " << e.what() << "\n";
        return 1;
    }

    return 0;
}