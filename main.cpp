#include <iostream>
#include "rsa_crypto.h"
#include "chacha20_crypto.h"
#include "utils.h"

// Заглушки для новых шифров (пока только выводят сообщение)
void a1y33_encrypt_decrypt(const std::string& filename) {
    std::cout << "Шифр А1Я33 (заглушка, ещё не реализован)" << std::endl;
}

void shamir_encrypt_decrypt(const std::string& filename) {
    std::cout << "Шифр Шамира (заглушка, ещё не реализован)" << std::endl;
}

void messi_omura_encrypt_decrypt(const std::string& filename) {
    std::cout << "Протокол Месси-Омура (заглушка, ещё не реализован)" << std::endl;
}

void rabin_encrypt_decrypt(const std::string& filename) {
    std::cout << "Криптосистема Рабина (заглушка, ещё не реализован)" << std::endl;
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
    
    std::string filename = select_file();
    if (filename.empty()) {
        std::cout << "Файл не выбран!" << std::endl;
        return 1;
    }
    
    if (choice == 1) {
        rsa_encrypt_decrypt(filename);
    } else if (choice == 2) {
        chacha20_encrypt_decrypt(filename);
    } else if (choice == 3) {
        a1y33_encrypt_decrypt(filename);
    } else if (choice == 4) {
        shamir_encrypt_decrypt(filename);
    } else if (choice == 5) {
        messi_omura_encrypt_decrypt(filename);
    } else if (choice == 6) {
        rabin_encrypt_decrypt(filename);
    }
    
    return 0;
}
