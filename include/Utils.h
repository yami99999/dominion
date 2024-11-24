#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <sstream>

class Utils {
public:
    // Traitement des chaînes  字符串处理
    static std::string toLower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
    
    static bool equalIgnoreCase(const std::string& str1, const std::string& str2) {
        return toLower(str1) == toLower(str2);
    }
    
    // Génération de nombres aléatoires  随机数生成
    static int getRandomNumber(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }
    
    // Fractionnement des chaînes  字符串分割
    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    // Sortie formatée  格式化输出
    static void printDivider(char c = '-', int length = 40) {
        std::cout << std::string(length, c) << "\n";
    }
    
    static void printHeader(const std::string& text, char c = '=', int length = 40) {
        printDivider(c, length);
        std::cout << text << "\n";
        printDivider(c, length);
    }
}; 