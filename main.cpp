#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "rsa_crypto.h"
#include "chacha20_crypto.h"
#include "utils.h"
#include "Rabin.h"
#include "mess.h"
#include "A1Ya33.h"
#include "Shamir.h"

using namespace std;

// Функции, необходимые для работы Rabin и mess
string inputMessage() {
    cout << "Введите текст (или оставьте пустым, чтобы выбрать файл): ";
    string msg;
    cin.ignore();
    getline(cin, msg);
    
    if (msg.empty()) {
        cout << "Выбор файла...\n";
        string filename = select_file();
        if (!filename.empty()) {
            ifstream file(filename);
            if (file.is_open()) {
                stringstream buffer;
                buffer << file.rdbuf();
                msg = buffer.str();
                file.close();
            }
        }
    }
    return msg;
}

void saveResult(const string& result) {
    cout << "\nРезультат:\n" << result << endl;
    
    string filename = "result.txt";
    cout << "Сохранить в файл (y/n, по умолчанию result.txt)? ";
    char choice;
    cin >> choice;
    
    if (choice == 'y' || choice == 'Y') {
        cout << "Введите имя файла для сохранения: ";
        cin.ignore();
        getline(cin, filename);
    }
    
    ofstream file(filename);
    if (file.is_open()) {
        file << result;
        file.close();
        cout << "Сохранено в " << filename << endl;
    } else {
        cerr << "Ошибка сохранения файла" << endl;
    }
}

void clearInput() {
    cin.clear();
    while (cin.get() != '\n');
}

// Функции для А1Я33 и Шамира
void a1y33_encrypt_decrypt() {
    cout << "\n   Шифр А1Я33" << endl;
    
    A1Ya33Cipher cipher;
    string text, result;
    int choice;
    
    cout << "1. Шифровать из файла" << endl;
    cout << "2. Дешифровать из файла" << endl;
    cout << "Выберите: ";
    cin >> choice;
    clearInput();
    
    string filename = select_file();
    if (filename.empty()) {
        cout << "Файл не выбран!" << endl;
        return;
    }
    
    if (!readFromFile(filename, text)) {
        cout << "Ошибка чтения файла!" << endl;
        return;
    }
    
    cout << "Показывать шаги? (1-да, 0-нет): ";
    bool showSteps;
    cin >> showSteps;
    clearInput();
    
    if (choice == 1) {
        result = cipher.encrypt(text, showSteps);
        string outFilename = filename + ".a1ya33.enc";
        if (writeToFile(outFilename, result)) {
            cout << "Зашифровано и сохранено в " << outFilename << endl;
        }
    } else if (choice == 2) {
        result = cipher.decrypt(text, showSteps);
        string outFilename = filename + ".a1ya33.decrypted";
        if (writeToFile(outFilename, result)) {
            cout << "Дешифровано и сохранено в " << outFilename << endl;
        }
    }
}

void shamir_encrypt_decrypt() {
    cout << "\n   Шифр Шамира" << endl;
    
    ShamirCipher cipher;
    string text, result;
    int choice;
    
    if (!cipher.keysAreCorrect()) {
        cout << "Ошибка в ключах Шамира!" << endl;
        return;
    }
    
    cout << "1. Шифровать из файла" << endl;
    cout << "2. Дешифровать из файла" << endl;
    cout << "Выберите: ";
    cin >> choice;
    clearInput();
    
    string filename = select_file();
    if (filename.empty()) {
        cout << "Файл не выбран!" << endl;
        return;
    }
    
    if (!readFromFile(filename, text)) {
        cout << "Ошибка чтения файла!" << endl;
        return;
    }
    
    cout << "Показывать шаги? (1-да, 0-нет): ";
    bool showSteps;
    cin >> showSteps;
    clearInput();
    
    if (choice == 1) {
        result = cipher.encrypt(text, showSteps);
        string outFilename = filename + ".shamir.enc";
        if (writeToFile(outFilename, result)) {
            cout << "Зашифровано и сохранено в " << outFilename << endl;
        }
    } else if (choice == 2) {
        result = cipher.decrypt(text, showSteps);
        string outFilename = filename + ".shamir.decrypted";
        if (writeToFile(outFilename, result)) {
            cout << "Дешифровано и сохранено в " << outFilename << endl;
        }
    }
}

int main() {
    std::cout << "ПРОСТАЯ ПРОГРАММА ДЛЯ ШИФРОВАНИЯ ФАЙЛОВ" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "1. RSA (Гибридный)" << std::endl;
    std::cout << "2. ChaCha20-Poly1305" << std::endl;
    std::cout << "3. Шифр А1Я33" << std::endl;
    std::cout << "4. Шифр Шамира" << std::endl;
    std::cout << "5. Протокол Месси-Омура" << std::endl;
    std::cout << "6. Криптосистема Рабина" << std::endl;
    std::cout << "7. Выход" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    int choice;
    std::cout << "Выберите вариант: ";
    std::cin >> choice;
    
    if (choice == 7) {
        return 0;
    }
    
    std::string filename; // Для совместимости
    if (choice == 1 || choice == 2) {
        filename = select_file();
        if (filename.empty()) {
            std::cout << "Файл не выбран!" << std::endl;
            return 1;
        }
    }
    
    if (choice == 1) {
        rsa_encrypt_decrypt(filename);
    } else if (choice == 2) {
        chacha20_encrypt_decrypt(filename);
    } else if (choice == 3) {
        a1y33_encrypt_decrypt();
    } else if (choice == 4) {
        shamir_encrypt_decrypt();
    } else if (choice == 5) {
        int sub_choice;
        cout << "\n1. Шифрование Месси-Омура\n2. Дешифрование Месси-Омура\nВыберите: ";
        cin >> sub_choice;
        if (sub_choice == 1) {
            masseyOmuraEncrypt();
        } else if (sub_choice == 2) {
            masseyOmuraDecrypt();
        }
    } else if (choice == 6) {
        int sub_choice;
        cout << "\n1. Шифрование Рабина\n2. Дешифрование Рабина\nВыберите: ";
        cin >> sub_choice;
        if (sub_choice == 1) {
            rabinEncrypt();
        } else if (sub_choice == 2) {
            rabinDecrypt();
        }
    }
    
    return 0;
}
