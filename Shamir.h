#ifndef SHAMIR_H
#define SHAMIR_H

#include <string>
#include <vector>

class ShamirCipher {
public:
    ShamirCipher();

    bool keysAreCorrect() const;
    std::string encrypt(const std::string& text, bool showSteps) const;
    std::string decrypt(const std::string& encryptedText, bool showSteps) const;

private:
    int p;
    int ca;
    int da;
    int cb;
    int db;

    int gcd(int a, int b) const;
    int extendedGcd(int a, int b, int& x, int& y) const;
    int modInverse(int value, int module) const;
    int modPower(int base, int power, int module) const;
    std::vector<int> parseEncryptedCodes(const std::string& encryptedText) const;
};

bool writeToFile(const std::string& fileName, const std::string& text);
bool readFromFile(const std::string& fileName, std::string& text);

#endif
