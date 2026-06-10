#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/evp.h>

void print_hex(const std::string& label, const std::vector<unsigned char>& data) {
    std::cout << "\n--- " << label << " ---" << std::endl;
    for (size_t i = 0; i < data.size(); ++i) {
        printf("%02x", data[i]);
    }
    std::cout << std::endl;
    std::cout << "Размер: " << data.size() << " байт" << std::endl;
}

std::vector<unsigned char> read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)), 
                                      std::istreambuf_iterator<char>());
    return buffer;
}

void write_file(const std::string& filename, const std::vector<unsigned char>& data) {
    std::ofstream file(filename, std::ios::binary);
    file.write((const char*)data.data(), data.size());
}

std::string select_file() {
#ifdef __linux__
    std::cout << "Открытие проводника (используем zenity)..." << std::endl;
    FILE* pipe = popen("zenity --file-selection --title='Выберите файл для шифрования'", "r");
    if (!pipe) return "";
    
    char buffer[1024];
    std::string filename = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        filename += buffer;
    }
    pclose(pipe);
    
    if (!filename.empty() && filename.back() == '\n') {
        filename.pop_back();
    }
    return filename;
#else
    std::string filename;
    std::cout << "Введите путь к файлу: ";
    std::cin >> filename;
    return filename;
#endif
}

void chacha20_encrypt_decrypt(const std::string& filename) {
    std::cout << "\n=== ChaCha20-Poly1305 ===" << std::endl;
    
    std::vector<unsigned char> plaintext = read_file(filename);
    std::cout << "Файл выбран: " << filename << std::endl;
    print_hex("ИСХОДНЫЙ ФАЙЛ", plaintext);
    
    unsigned char key[32];
    unsigned char nonce[12];
    RAND_bytes(key, sizeof(key));
    RAND_bytes(nonce, sizeof(nonce));
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* cipher = EVP_chacha20_poly1305();
    
    EVP_EncryptInit_ex(ctx, cipher, nullptr, key, nonce);
    
    std::vector<unsigned char> ciphertext(plaintext.size() + 16);
    int len;
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size());
    int ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;
    
    unsigned char tag[16];
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, tag);
    ciphertext.resize(ciphertext_len);
    ciphertext.insert(ciphertext.end(), tag, tag + 16);
    
    write_file(filename + ".chacha.enc", ciphertext);
    write_file(filename + ".chacha.key", std::vector<unsigned char>(key, key + sizeof(key)));
    write_file(filename + ".chacha.nonce", std::vector<unsigned char>(nonce, nonce + sizeof(nonce)));
    
    std::cout << "Зашифрованный файл сохранён: " << filename << ".chacha.enc" << std::endl;
    print_hex("ЗАШИФРОВАННЫЙ ФАЙЛ", ciphertext);
    
    std::cout << "\n--- Дешифрование ---" << std::endl;
    
    unsigned char read_tag[16];
    std::copy(ciphertext.end() - 16, ciphertext.end(), read_tag);
    std::vector<unsigned char> actual_ciphertext(ciphertext.begin(), ciphertext.end() - 16);
    
    EVP_DecryptInit_ex(ctx, cipher, nullptr, key, nonce);
    
    std::vector<unsigned char> decrypted(actual_ciphertext.size());
    EVP_DecryptUpdate(ctx, decrypted.data(), &len, actual_ciphertext.data(), actual_ciphertext.size());
    int decrypted_len = len;
    
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, 16, read_tag);
    EVP_DecryptFinal_ex(ctx, decrypted.data() + len, &len);
    decrypted_len += len;
    decrypted.resize(decrypted_len);
    
    write_file(filename + ".chacha.decrypted", decrypted);
    std::cout << "Дешифрованный файл сохранён: " << filename << ".chacha.decrypted" << std::endl;
    print_hex("РАСШИФРОВАННЫЙ ФАЙЛ", decrypted);
    
    EVP_CIPHER_CTX_free(ctx);
    std::cout << "\nГотово!" << std::endl;
}

void rsa_encrypt_decrypt(const std::string& filename) {
    std::cout << "\n=== RSA (Гибридный) ===" << std::endl;
    
    std::vector<unsigned char> plaintext = read_file(filename);
    std::cout << "Файл выбран: " << filename << std::endl;
    print_hex("ИСХОДНЫЙ ФАЙЛ", plaintext);
    
    EVP_PKEY* pkey = EVP_PKEY_new();
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    EVP_PKEY_keygen_init(ctx);
    EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048);
    EVP_PKEY_keygen(ctx, &pkey);
    EVP_PKEY_CTX_free(ctx);
    
    unsigned char chacha_key[32];
    unsigned char nonce[12];
    RAND_bytes(chacha_key, sizeof(chacha_key));
    RAND_bytes(nonce, sizeof(nonce));
    
    EVP_CIPHER_CTX* cipher_ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* cipher = EVP_chacha20_poly1305();
    
    EVP_EncryptInit_ex(cipher_ctx, cipher, nullptr, chacha_key, nonce);
    
    std::vector<unsigned char> file_ciphertext(plaintext.size() + 16);
    int len;
    EVP_EncryptUpdate(cipher_ctx, file_ciphertext.data(), &len, plaintext.data(), plaintext.size());
    int file_ciphertext_len = len;
    EVP_EncryptFinal_ex(cipher_ctx, file_ciphertext.data() + len, &len);
    file_ciphertext_len += len;
    
    unsigned char tag[16];
    EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_AEAD_GET_TAG, 16, tag);
    file_ciphertext.resize(file_ciphertext_len);
    file_ciphertext.insert(file_ciphertext.end(), tag, tag + 16);
    
    EVP_PKEY_CTX* rsa_ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    EVP_PKEY_encrypt_init(rsa_ctx);
    EVP_PKEY_CTX_set_rsa_padding(rsa_ctx, RSA_PKCS1_PADDING);
    
    size_t encrypted_key_len;
    EVP_PKEY_encrypt(rsa_ctx, nullptr, &encrypted_key_len, chacha_key, sizeof(chacha_key));
    std::vector<unsigned char> encrypted_key(encrypted_key_len);
    EVP_PKEY_encrypt(rsa_ctx, encrypted_key.data(), &encrypted_key_len, chacha_key, sizeof(chacha_key));
    
    std::vector<unsigned char> result;
    uint32_t key_len = encrypted_key.size();
    result.insert(result.end(), (unsigned char*)&key_len, (unsigned char*)&key_len + 4);
    result.insert(result.end(), encrypted_key.begin(), encrypted_key.end());
    result.insert(result.end(), nonce, nonce + sizeof(nonce));
    result.insert(result.end(), file_ciphertext.begin(), file_ciphertext.end());
    
    write_file(filename + ".rsa.enc", result);
    std::cout << "Зашифрованный файл сохранён: " << filename << ".rsa.enc" << std::endl;
    print_hex("ЗАШИФРОВАННЫЙ ФАЙЛ", result);
    
    std::cout << "\n--- Дешифрование ---" << std::endl;
    
    size_t offset = 0;
    uint32_t read_key_len = *(uint32_t*)(result.data() + offset);
    offset += 4;
    
    std::vector<unsigned char> read_encrypted_key(result.begin() + offset, result.begin() + offset + read_key_len);
    offset += read_key_len;
    
    unsigned char read_nonce[12];
    std::copy(result.begin() + offset, result.begin() + offset + 12, read_nonce);
    offset += 12;
    
    std::vector<unsigned char> read_file_ciphertext(result.begin() + offset, result.end());
    
    EVP_PKEY_CTX* rsa_decrypt_ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    EVP_PKEY_decrypt_init(rsa_decrypt_ctx);
    EVP_PKEY_CTX_set_rsa_padding(rsa_decrypt_ctx, RSA_PKCS1_PADDING);
    
    size_t decrypted_key_len;
    EVP_PKEY_decrypt(rsa_decrypt_ctx, nullptr, &decrypted_key_len, read_encrypted_key.data(), read_encrypted_key.size());
    std::vector<unsigned char> decrypted_key(decrypted_key_len);
    EVP_PKEY_decrypt(rsa_decrypt_ctx, decrypted_key.data(), &decrypted_key_len, read_encrypted_key.data(), read_encrypted_key.size());
    
    unsigned char read_tag2[16];
    std::copy(read_file_ciphertext.end() - 16, read_file_ciphertext.end(), read_tag2);
    std::vector<unsigned char> actual_file_ciphertext(read_file_ciphertext.begin(), read_file_ciphertext.end() - 16);
    
    EVP_DecryptInit_ex(cipher_ctx, cipher, nullptr, decrypted_key.data(), read_nonce);
    
    std::vector<unsigned char> decrypted(actual_file_ciphertext.size());
    EVP_DecryptUpdate(cipher_ctx, decrypted.data(), &len, actual_file_ciphertext.data(), actual_file_ciphertext.size());
    int decrypted_len = len;
    
    EVP_CIPHER_CTX_ctrl(cipher_ctx, EVP_CTRL_AEAD_SET_TAG, 16, read_tag2);
    EVP_DecryptFinal_ex(cipher_ctx, decrypted.data() + len, &len);
    decrypted_len += len;
    decrypted.resize(decrypted_len);
    
    write_file(filename + ".rsa.decrypted", decrypted);
    std::cout << "Дешифрованный файл сохранён: " << filename << ".rsa.decrypted" << std::endl;
    print_hex("РАСШИФРОВАННЫЙ ФАЙЛ", decrypted);
    
    EVP_PKEY_CTX_free(rsa_ctx);
    EVP_PKEY_CTX_free(rsa_decrypt_ctx);
    EVP_CIPHER_CTX_free(cipher_ctx);
    EVP_PKEY_free(pkey);
    
    std::cout << "\nГотово!" << std::endl;
}

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
