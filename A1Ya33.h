#ifndef A1YA33_H
#define A1YA33_H

#include <string>
#include <vector>

class A1Ya33Cipher {
public:
    A1Ya33Cipher();

    std::string encrypt(const std::string& text, bool showSteps) const;
    std::string decrypt(const std::string& encryptedText, bool showSteps) const;

private:
    std::vector<std::string> russianUpper;
    std::vector<std::string> russianLower;
    std::vector<std::string> englishUpper;
    std::vector<std::string> englishLower;

    std::vector<std::string> splitUtf8(const std::string& text) const;
    int findIndex(const std::vector<std::string>& alphabet, const std::string& symbol) const;
    std::string getUtf8Symbol(const std::vector<std::string>& alphabet, int number) const;
};

bool writeToFile(const std::string& fileName, const std::string& text);
bool readFromFile(const std::string& fileName, std::string& text);

#endif
