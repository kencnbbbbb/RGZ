#include "Shamir.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

ShamirCipher::ShamirCipher() {
    p = 257;
    ca = 17;
    cb = 19;
    da = modInverse(ca, p - 1);
    db = modInverse(cb, p - 1);
}

bool ShamirCipher::keysAreCorrect() const {
    return gcd(ca, p - 1) == 1 && gcd(cb, p - 1) == 1 && da != 0 && db != 0;
}

int ShamirCipher::gcd(int a, int b) const {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }

    return a;
}

int ShamirCipher::extendedGcd(int a, int b, int& x, int& y) const {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }

    int x1 = 0;
    int y1 = 0;
    int result = extendedGcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return result;
}

int ShamirCipher::modInverse(int value, int module) const {
    int x = 0;
    int y = 0;
    int result = extendedGcd(value, module, x, y);

    if (result != 1) {
        return 0;
    }

    int inverse = x % module;
    if (inverse < 0) {
        inverse += module;
    }

    return inverse;
}

int ShamirCipher::modPower(int base, int power, int module) const {
    int result = 1;
    base %= module;

    while (power > 0) {
        if (power % 2 == 1) {
            result = (result * base) % module;
        }

        base = (base * base) % module;
        power /= 2;
    }

    return result;
}

std::vector<int> ShamirCipher::parseEncryptedCodes(const std::string& encryptedText) const {
    std::istringstream input(encryptedText);
    std::vector<int> codes;
    int code = 0;

    while (input >> code) {
        codes.push_back(code);
    }

    return codes;
}

std::string ShamirCipher::encrypt(const std::string& text, bool showSteps) const {
    std::ostringstream encrypted;

    for (std::size_t i = 0; i < text.size(); ++i) {
        unsigned char byte = static_cast<unsigned char>(text[i]);
        int m = static_cast<int>(byte);
        int x1 = modPower(m, ca, p);
        int x2 = modPower(x1, cb, p);
        int x3 = modPower(x2, da, p);
        int x4 = modPower(x3, db, p);

        if (i > 0) {
            encrypted << ' ';
        }
        encrypted << x2;

        if (showSteps) {
            std::cout << "Байт: " << m;
            if (m >= 32 && m <= 126) {
                std::cout << " ('" << static_cast<char>(byte) << "')";
            }
            std::cout << "\n";
            std::cout << "m = " << m << "\n";
            std::cout << "Ca = " << ca << ", Da = " << da
                      << ", Cb = " << cb << ", Db = " << db << "\n";
            std::cout << "x1 = m^Ca mod p = " << x1 << "\n";
            std::cout << "x2 = x1^Cb mod p = " << x2 << "\n";
            std::cout << "x3 = x2^Da mod p = " << x3 << "\n";
            std::cout << "x4 = x3^Db mod p = " << x4 << "\n";
            std::cout << "Восстановленный байт: " << x4 << "\n\n";
        }
    }

    return encrypted.str();
}

std::string ShamirCipher::decrypt(const std::string& encryptedText, bool showSteps) const {
    std::vector<int> codes = parseEncryptedCodes(encryptedText);
    std::string result;

    for (std::size_t i = 0; i < codes.size(); ++i) {
        int x2 = codes[i];
        int x3 = modPower(x2, da, p);
        int m = modPower(x3, db, p);
        unsigned char byte = static_cast<unsigned char>(m);
        result += static_cast<char>(byte);

        if (showSteps) {
            std::cout << "Зашифрованный код x2: " << x2 << "\n";
            std::cout << "Ca = " << ca << ", Da = " << da
                      << ", Cb = " << cb << ", Db = " << db << "\n";
            std::cout << "x3 = x2^Da mod p = " << x3 << "\n";
            std::cout << "m = x3^Db mod p = " << m << "\n";
            std::cout << "Восстановленная строка после шага: " << result << "\n\n";
        }
    }

    return result;
}

bool writeToFile(const std::string& fileName, const std::string& text) {
    std::ofstream file(fileName);

    if (!file.is_open()) {
        return false;
    }

    file << text;
    return true;
}

bool readFromFile(const std::string& fileName, std::string& text) {
    std::ifstream file(fileName);

    if (!file.is_open()) {
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    text = buffer.str();
    return true;
}

int main() {
    const std::string fileName = "encrypted_shamir.txt";
    ShamirCipher cipher;
    bool showSteps = false;
    int choice = -1;

    if (!cipher.keysAreCorrect()) {
        std::cout << "Ошибка: ключи не подходят для алгоритма Шамира.\n";
        return 1;
    }

    do {
        std::cout << "\n=== Шифр Шамира ===\n";
        std::cout << "1. Ввести текст и зашифровать\n";
        std::cout << "2. Прочитать зашифрованный текст из файла и дешифровать\n";
        std::cout << "3. Включить / выключить показ этапов\n";
        std::cout << "0. Выход\n";
        std::cout << "Ваш выбор: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            std::string text;
            std::cout << "Введите текст: ";
            std::getline(std::cin, text);

            std::string encrypted = cipher.encrypt(text, showSteps);
            std::cout << "Зашифрованный текст:\n" << encrypted << "\n";

            if (writeToFile(fileName, encrypted)) {
                std::cout << "Текст сохранен в файл " << fileName << "\n";
            } else {
                std::cout << "Ошибка записи в файл.\n";
            }
        } else if (choice == 2) {
            std::string encrypted;

            if (readFromFile(fileName, encrypted)) {
                std::cout << "Зашифрованный текст из файла:\n" << encrypted << "\n";
                std::string decrypted = cipher.decrypt(encrypted, showSteps);
                std::cout << "Расшифрованный текст:\n" << decrypted << "\n";
            } else {
                std::cout << "Не удалось прочитать файл " << fileName << "\n";
            }
        } else if (choice == 3) {
            showSteps = !showSteps;
            std::cout << "Показ этапов: " << (showSteps ? "включен" : "выключен") << "\n";
        } else if (choice != 0) {
            std::cout << "Неверный пункт меню.\n";
        }
    } while (choice != 0);

    return 0;
}
