#pragma once
#include <string>
#include <algorithm>

class Utils {
public:
    // 转换为小写
    static std::string toLower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
    
    // 比较字符串（不区分大小写）
    static bool equalIgnoreCase(const std::string& str1, const std::string& str2) {
        return toLower(str1) == toLower(str2);
    }
}; 