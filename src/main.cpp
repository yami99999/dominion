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

int main(int argc, char* argv[]) {
    displayWelcome();
    
    // 处理命令行参数
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
    
    // 询问是否加载存档
    std::cout << "是否加载存档? (y/n): ";
    std::string response;
    std::getline(std::cin, response);
    
    if (Utils::equalIgnoreCase(response, "y")) {
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
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } while (playerCount < 2 || playerCount > 4);
    
    try {
        Game game(playerCount, randomKingdom, kingdomSet);
        game.playGame();
    } catch (const std::exception& e) {
        std::cout << "游戏运行时发生错误: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
} 