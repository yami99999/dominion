#include <iostream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include "Game.h"
#include "Utils.h"

void displayWelcome() {
    std::cout << "\n";
    std::cout << "================================\n";
    std::cout << "       欢迎来到 Dominion!\n";
    std::cout << "================================\n\n";
}

int main() {
    displayWelcome();
    
    // 首先询问是否加载存档
    std::cout << "是否加载存档? (y/n): ";
    std::string response;
    std::getline(std::cin, response);
    
    if (Utils::equalIgnoreCase(response, "y") || Utils::equalIgnoreCase(response, "yes")) {
        std::cout << "请输入存档文件名 (默认为 'dominion_save.txt'): ";
        std::string filename;
        std::getline(std::cin, filename);
        if (filename.empty()) {
            filename = "dominion_save.txt";
        }
        
        try {
            Game game(0);  // 创建一个临时游戏实例
            game.loadGame(filename);
            game.playGame();
            return 0;
        } catch (const std::exception& e) {
            std::cout << "加载存档失败: " << e.what() << "\n";
            std::cout << "是否开始新游戏? (y/n): ";
            std::getline(std::cin, response);
            if (!Utils::equalIgnoreCase(response, "y") && !Utils::equalIgnoreCase(response, "yes")) {
                return 0;
            }
        }
    }
    
    // 开始新游戏
    int playerCount;
    do {
        std::cout << "\n请输入玩家数量 (2-4): ";
        std::cin >> playerCount;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            playerCount = 0;
        }
    } while (playerCount < 2 || playerCount > 4);
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    try {
        Game game(playerCount);
        game.playGame();
    } catch (const std::exception& e) {
        std::cout << "游戏运行时发生错误: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
} 