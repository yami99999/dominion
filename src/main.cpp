#include <iostream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include "Game.h"
#include "Utils.h"

void displayWelcome() { // 打印欢迎界面
    std::cout << "\n";
    std::cout << "================================\n";
    std::cout << "       Bienvenue à Dominion !\n";
    std::cout << "================================\n\n";
}

int main() {
    displayWelcome(); // ❇️打印欢迎界面
    



    // ❇️首先询问是否加载存档
    std::cout << "L'archive est-elle chargée ? (y/n): ";
    std::string response; // 储存用户的输入
    std::getline(std::cin, response); // 从标准输入读取一整行输入，并存储到 response 中(这样可以捕获包括空格在内的完整输入。)
    

    // 判断是否进入加载存档
    // 使用 Utils 类的 equalIgnoreCase 函数，不区分大小写地比较用户输入是否为 "y" 或 "yes"。
    // 如果是，则进入加载存档的流程。
    if (Utils::equalIgnoreCase(response, "y") || Utils::equalIgnoreCase(response, "yes")) { 
        
        
        
        std::cout << "Saisis le nom du fichier d'archive (la valeur par défaut est 'dominion_save.txt').: ";
        std::string filename;
        std::getline(std::cin, filename);
        
        // 没输入, 则为默认值
        if (filename.empty()) {
            filename = "dominion_save.txt";
        }
        


        // 使用 try-catch 块来捕捉和处理可能在加载存档过程中抛出的异常。
        try {

            // 创建一个临时游戏实例
            // 参数0 - 这表示创建一个临时的游戏实例，不初始化玩家(玩家数-0)
            Game game(0);  
            game.loadGame(filename); // 尝试从指定的文件中加载游戏状态
            game.playGame(); // 如果加载成功，调用 playGame 方法，开始或继续游戏。
            return 0;
        } 
        
        // 如果在 try 块中发生异常（例如文件不存在、解析错误等），捕捉异常并处理。 
        catch (const std::exception& e) {
            std::cout << "加载存档失败: " << e.what() << "\n"; // 输出加载存档失败的错误信息，e.what() 提供具体的异常描述
            std::cout << "是否开始新游戏? (y/n): ";
            std::getline(std::cin, response);

            // 如果用户输入的不是 "y" 或 "yes"，则程序结束，返回 0。
            // 否则，继续执行后续的代码以开始新游戏。
            if (!Utils::equalIgnoreCase(response, "y") && !Utils::equalIgnoreCase(response, "yes")) {
                return 0;
            }
        }
    }
    



    // ❇️不加载存档则开始新游戏
    int playerCount; // 用于存储玩家数量
    // 使用 do-while 循环，确保用户输入的玩家数量在 2 到 4 之间。
    do {
        std::cout << "\n请输入玩家数量 (2-4): ";
        std::cin >> playerCount; 
        
        if (std::cin.fail()) { // 检查输入是否失败（例如用户输入了非数字字符）
            std::cin.clear(); // 清除输入流的错误标志，使得输入流可以继续使用。
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 忽略输入流中的所有字符，直到遇到换行符为止，防止无效输入影响后续读取。
            playerCount = 0; // 将 playerCount 设置为 0，确保循环继续，直到用户输入有效的玩家数量。
        }
    } while (playerCount < 2 || playerCount > 4);
    


    // 再次清除输入流中的剩余字符，确保后续的 std::getline 调用不会受到之前输入的影响。
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    


    // 捕捉和处理在游戏运行过程中可能发生的异常。
    try {
        Game game(playerCount); // 初始化游戏并创建指定数量的玩家
        game.playGame(); // 开始游戏的主循环
    } catch (const std::exception& e) {
        std::cout << "Le jeu s'exécute avec l'erreur:  " << e.what() << "\n";
        return 1;
    }
    
    return 0;
} 