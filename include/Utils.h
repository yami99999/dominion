#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <sstream>

class Utils
{
public:
    // Traitement des chaînes
    // static Une fonction membre statique, indépendante de l'instance spécifique de la classe, peut être appelée directement via le nom de la classe.
    //  字符串处理
    // static 一个静态成员函数，与类的具体实例无关，可以通过类名直接调用。

    // Convertir la chaîne en minuscule
    // La copie d'entrée est indépendante de la chaîne d'origine et la chaîne d'origine ne sera pas modifiée.
    //  将字符串转换为小写
    // 输入副本与原字符串独立, 原始字符串不会被修改
    static std::string toLower(std::string str)
    {
        // str.begin() et str.end(), c'est-à-dire la plage de la chaîne entière.
        // Le résultat est stocké dans str.begin(), c'est-à-dire que la chaîne elle-même est directement modifiée
        //::vers le bas
        // Convertir un seul caractère en minuscule
        // Invalide pour les caractères non alphabétiques (tels que les chiffres ou les signes de ponctuation), les caractères originaux seront renvoyés directement.
        // La méthode de dénomination de ::tolower indique la version appelant l'espace de noms global pour éviter d'éventuels conflits de noms.
        //  str.begin() 和 str.end()，即整个字符串的范围。
        // 结果存储在 str.begin()，即直接修改字符串自身
        // ::tolower
        // 将单个字符转换为小写
        // 对非字母字符（如数字或标点符号）无效，会直接返回原字符。
        // ::tolower 的命名方式表示调用全局命名空间的版本，避免潜在的命名冲突。
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }

    // Ignore la casse et compare deux chaînes pour voir si elles sont égales.
    //  忽略大小写比较两个字符串是否相等
    static bool equalIgnoreCase(const std::string &str1, const std::string &str2)
    {
        return toLower(str1) == toLower(str2);
    }

    // Génération de nombres aléatoires
    //  随机数生成
    static int getRandomNumber(int min, int max)
    {
        // Périphérique aléatoire, utilisé pour générer des graines aléatoires (un générateur matériel de nombres aléatoires (si le matériel le prend en charge), sinon il dégénérera en génération de nombres pseudo-aléatoires)
        // static garantit qu'il n'est initialisé qu'une seule fois pendant toute l'exécution du programme pour éviter la surcharge liée à la création d'objets plusieurs fois. -Même si getRandomNumber est appelé plusieurs fois, il réutilisera la même instance
        //  随机设备，用于生成随机种子(一个硬件随机数生成器（如果硬件支持），否则会退化为伪随机数生成)
        // static 确保在整个程序运行期间只初始化一次，避免多次创建对象的开销。 - 即使多次调用 getRandomNumber，它也会复用同一个 rd 实例
        static std::random_device rd;

        // Générateur d'algorithme de rotation de Mersenne -efficace et aléatoire
        // static garantit qu'il n'est initialisé qu'une seule fois pendant toute l'exécution du programme pour éviter la surcharge liée à la création d'objets plusieurs fois.
        //  梅森旋转算法生成器 - 效率高且随机性良好
        // static 确保在整个程序运行期间只初始化一次，避免多次创建对象的开销。
        static std::mt19937 gen(rd());

        // dis est une variable locale, qui sera recréée à chaque appel de la fonction, afin que la plage de génération de nombres aléatoires puisse être ajustée de manière flexible.
        //  dis 是局部变量，每次调用函数时都会重新创建，这样可以灵活调整随机数生成范围
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }

    // Divise la chaîne selon le délimiteur spécifié et renvoie la liste des chaînes divisées
    //  字符串按照指定分割符分割, 返回分割后的字符串列表
    static std::vector<std::string> split(const std::string &str, char delimiter)
    {
        std::vector<std::string> tokens; // Stockage de la chaîne divisée
                                         //  存储分割后的字符串
        std::string token; // Stocke une seule chaîne divisée
                           //  存储单个分割后的字符串
        std::istringstream tokenStream(str); // Flux de chaîne (convertir la chaîne en flux)
                                             //  字符串流(将字符串转换为流)
        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens; // Renvoie la chaîne divisée
                       //  返回分割后的字符串
    }

    // Sortie formatée -imprimer la ligne de séparation
    //  格式化输出 - 打印分割线
    static void printDivider(char c = '-', int length = 40)
    {
        std::cout << std::string(length, c) << "\n";
    }

    // Sortie formatée -imprimer le titre
    //  格式化输出 - 打印标题
    static void printHeader(const std::string &text, char c = '=', int length = 40)
    {
        printDivider(c, length);
        std::cout << text << "\n";
        printDivider(c, length);
    }
};