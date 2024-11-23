#pragma once
#include <string>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Utils {
public:
    // 辅助函数: 转换为小写
    static std::string toLower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower); // 使用 std::transform 和 lambda 表达式，将每个字符转换为小写
        return str; // 返回转换后的字符串
    }
    
    // 比较字符串（不区分大小写）
    static bool equalIgnoreCase(const std::string& str1, const std::string& str2) {
        return toLower(str1) == toLower(str2);
    }
}; 