#include "A1Ya33.h"

#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

A1Ya33Cipher::A1Ya33Cipher() {
    russianUpper = {"А", "Б", "В", "Г", "Д", "Е", "Ё", "Ж", "З", "И", "Й",
                    "К", "Л", "М", "Н", "О", "П", "Р", "С", "Т", "У", "Ф",
                    "Х", "Ц", "Ч", "Ш", "Щ", "Ъ", "Ы", "Ь", "Э", "Ю", "Я"};

    russianLower = {"а", "б", "в", "г", "д", "е", "ё", "ж", "з", "и", "й",
                    "к", "л", "м", "н", "о", "п", "р", "с", "т", "у", "ф",
                    "х", "ц", "ч", "ш", "щ", "ъ", "ы", "ь", "э", "ю", "я"};

    englishUpper = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K",
                    "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
                    "W", "X", "Y", "Z"};

    englishLower = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
                    "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v",
                    "w", "x", "y", "z"};
}

std::vector<std::string> A1Ya33Cipher::splitUtf8(const std::string& text) const {
    std::vector<std::string> symbols;

    for (std::size_t i = 0; i < text.size();) {
        unsigned char byte = static_cast<unsigned char>(text[i]);
        std::size_t symbolSize = 1;

        if ((byte & 0xE0) == 0xC0) {
            symbolSize = 2;
        } else if ((byte & 0xF0) == 0xE0) {
            symbolSize = 3;
        } else if ((byte & 0xF8) == 0xF0) {
            symbolSize = 4;
        }

        symbols.push_back(text.substr(i, symbolSize));
        i += symbolSize;
    }

    return symbols;
}

int A1Ya33Cipher::findIndex(const std::vector<std::string>& alphabet, const std::string& symbol) const {
    for (std::size_t i = 0; i < alphabet.size(); ++i) {
        if (alphabet[i] == symbol) {
            return static_cast<int>(i) + 1;
        }
    }

    return 0;
}

std::string A1Ya33Cipher::getUtf8Symbol(const std::vector<std::string>& alphabet, int number) const {
    if (number < 1 || number > static_cast<int>(alphabet.size())) {
        return "";
    }

    return alphabet[static_cast<std::size_t>(number - 1)];
}

std::string A1Ya33Cipher::encrypt(const std::string& text, bool showSteps) const {
    std::vector<std::string> symbols = splitUtf8(text);
    std::ostringstream encrypted;

    for (std::size_t i = 0; i < symbols.size(); ++i) {
        std::string symbol = symbols[i];
        std::string alphabetName = "служебный символ";
        std::string encryptedElement;
        int number = 0;

        number = findIndex(russianUpper, symbol);
        if (number != 0) {
            alphabetName = "русский, верхний регистр";
            encryptedElement = "R" + std::to_string(number) + "U";
        } else {
            number = findIndex(russianLower, symbol);
            if (number != 0) {
                alphabetName = "русский, нижний регистр";
                encryptedElement = "R" + std::to_string(number) + "L";
            }
        }

        if (encryptedElement.empty()) {
            number = findIndex(englishUpper, symbol);
            if (number != 0) {
                alphabetName = "английский, верхний регистр";
                encryptedElement = "E" + std::to_string(number) + "U";
            } else {
                number = findIndex(englishLower, symbol);
                if (number != 0) {
                    alphabetName = "английский, нижний регистр";
                    encryptedElement = "E" + std::to_string(number) + "L";
                }
            }
        }

        if (encryptedElement.empty()) {
            if (symbol == " ") {
                encryptedElement = "_";
            } else {
                encryptedElement = "S";
                for (std::size_t byteIndex = 0; byteIndex < symbol.size(); ++byteIndex) {
                    if (byteIndex > 0) {
                        encryptedElement += ".";
                    }
                    unsigned char byte = static_cast<unsigned char>(symbol[byteIndex]);
                    encryptedElement += std::to_string(static_cast<int>(byte));
                }
            }
        }

        if (i > 0) {
            encrypted << ' ';
        }
        encrypted << encryptedElement;

        if (showSteps) {
            std::cout << "Символ: " << symbol << "\n";
            std::cout << "Алфавит: " << alphabetName << "\n";
            std::cout << "Номер: " << (number == 0 ? "-" : std::to_string(number)) << "\n";
            std::cout << "Зашифрованный элемент: " << encryptedElement << "\n\n";
        }
    }

    return encrypted.str();
}

std::string A1Ya33Cipher::decrypt(const std::string& encryptedText, bool showSteps) const {
    std::istringstream input(encryptedText);
    std::string element;
    std::string result;

    while (input >> element) {
        std::string alphabetName = "служебный символ";
        std::string symbol;

        if (element == "_") {
            symbol = " ";
        } else if (element.size() >= 2 && element[0] == 'S') {
            std::string codes = element.substr(1);
            std::istringstream codeInput(codes);
            std::string codeText;

            while (std::getline(codeInput, codeText, '.')) {
                int code = std::stoi(codeText);
                symbol += static_cast<char>(code);
            }
        } else if (element.size() >= 2 && (element[0] == 'R' || element[0] == 'E')) {
            char alphabetCode = element[0];
            char caseCode = element[element.size() - 1];
            std::string numberText = element.substr(1, element.size() - 2);
            int number = std::stoi(numberText);

            if (alphabetCode == 'R' && caseCode == 'U') {
                alphabetName = "русский, верхний регистр";
                symbol = getUtf8Symbol(russianUpper, number);
            } else if (alphabetCode == 'R' && caseCode == 'L') {
                alphabetName = "русский, нижний регистр";
                symbol = getUtf8Symbol(russianLower, number);
            } else if (alphabetCode == 'E' && caseCode == 'U') {
                alphabetName = "английский, верхний регистр";
                symbol = getUtf8Symbol(englishUpper, number);
            } else if (alphabetCode == 'E' && caseCode == 'L') {
                alphabetName = "английский, нижний регистр";
                symbol = getUtf8Symbol(englishLower, number);
            }
        }

        result += symbol;

        if (showSteps) {
            std::cout << "Зашифрованный элемент: " << element << "\n";
            std::cout << "Алфавит: " << alphabetName << "\n";
            std::cout << "Найденная буква/символ: " << symbol << "\n";
            std::cout << "Строка после шага: " << result << "\n\n";
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
    const std::string fileName = "encrypted_a1ya33.txt";
    A1Ya33Cipher cipher;
    bool showSteps = false;
    int choice = -1;

    do {
        std::cout << "\n=== Шифр А1Я33 ===\n";
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
