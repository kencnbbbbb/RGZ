#include "chacha20_crypto.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/evp.h>

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
