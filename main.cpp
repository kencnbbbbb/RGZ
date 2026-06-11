#include <iostream>
#include "rsa_crypto.h"
#include "chacha20_crypto.h"
#include "utils.h"

int main() {
    std::cout << "ПРОСТАЯ ПРОГРАММА ДЛЯ ШИФРОВАНИЯ ФАЙЛОВ" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "1. RSA (Гибридный)" << std::endl;
    std::cout << "2. ChaCha20-Poly1305" << std::endl;
    std::cout << "3. Выход" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    int choice;
    std::cout << "Выберите вариант: ";
    std::cin >> choice;
    
    if (choice == 3) {
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
    }
    
    return 0;
}
