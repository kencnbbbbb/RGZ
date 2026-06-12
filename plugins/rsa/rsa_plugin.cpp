#include "../../include/ICryptoPlugin.h"
#include "../utils.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Глобальные ключи для RSA (OpenSSL)
static EVP_PKEY* rsa_key = nullptr;
static unsigned char rsa_chacha_key[32];
static unsigned char rsa_nonce[12];

extern "C" const char* getAlgorithmName() {
    return "RSA (гибридный с ChaCha20)";
}

extern "C" bool generateKeys(vector<string>& keys, bool showSteps) {
    cout << "Генерация RSA-ключей (2048 бит)..." << endl;
    
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    EVP_PKEY_keygen_init(ctx);
    EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048);
    
    rsa_key = EVP_PKEY_new();
    EVP_PKEY_keygen(ctx, &rsa_key);
    EVP_PKEY_CTX_free(ctx);
    
    // Генерация ключа для ChaCha20
    RAND_bytes(rsa_chacha_key, sizeof(rsa_chacha_key));
    RAND_bytes(rsa_nonce, sizeof(rsa_nonce));
    
    keys = { "CHACHA_KEY=" + string(reinterpret_cast<char*>(rsa_chacha_key), 32),
             "CHACHA_NONCE=" + string(reinterpret_cast<char*>(rsa_nonce), 12) };
    cout << "Ключи RSA сгенерированы." << endl;
    return true;
}

extern "C" bool encryptBinary(const vector<unsigned char>& data, const vector<string>& keys, const vector<unsigned char>& nonce, vector<unsigned char>& output, bool showSteps) {
    cout << "Шифрование RSA (гибрид)..." << endl;
    
    // Шифруем данные ChaCha20
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* cipher = EVP_chacha20_poly1305();
    
    EVP_EncryptInit_ex(ctx, cipher, nullptr, rsa_chacha_key, rsa_nonce);
    
    vector<unsigned char> ciphertext(data.size() + 16);
    int len;
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, data.data(), data.size());
    int ct_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ct_len += len;
    
    unsigned char tag[16];
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, tag);
    ciphertext.resize(ct_len);
    ciphertext.insert(ciphertext.end(), tag, tag + 16);
    
    EVP_CIPHER_CTX_free(ctx);
    
    // Шифруем ChaCha-ключ RSA
    EVP_PKEY_CTX* rsa_ctx = EVP_PKEY_CTX_new(rsa_key, nullptr);
    EVP_PKEY_encrypt_init(rsa_ctx);
    EVP_PKEY_CTX_set_rsa_padding(rsa_ctx, RSA_PKCS1_PADDING);
    
    size_t enc_key_len;
    EVP_PKEY_encrypt(rsa_ctx, nullptr, &enc_key_len, rsa_chacha_key, sizeof(rsa_chacha_key));
    vector<unsigned char> enc_key(enc_key_len);
    EVP_PKEY_encrypt(rsa_ctx, enc_key.data(), &enc_key_len, rsa_chacha_key, sizeof(rsa_chacha_key));
    
    EVP_PKEY_CTX_free(rsa_ctx);
    
    // Собираем выход: key_len (4 байта) || enc_key || nonce || ciphertext+tag
    output.clear();
    uint32_t key_len = enc_key.size();
    output.insert(output.end(), reinterpret_cast<unsigned char*>(&key_len), reinterpret_cast<unsigned char*>(&key_len) + 4);
    output.insert(output.end(), enc_key.begin(), enc_key.end());
    output.insert(output.end(), rsa_nonce, rsa_nonce + 12);
    output.insert(output.end(), ciphertext.begin(), ciphertext.end());
    
    return true;
}

extern "C" bool decryptBinary(const vector<unsigned char>& encryptedData, const vector<string>& keys, const vector<unsigned char>& nonce, vector<unsigned char>& output, bool showSteps) {
    cout << "Дешифрование RSA (гибрид)..." << endl;
    
    size_t offset = 0;
    uint32_t key_len = *reinterpret_cast<const uint32_t*>(encryptedData.data() + offset);
    offset += 4;
    
    vector<unsigned char> enc_key(encryptedData.begin() + offset, encryptedData.begin() + offset + key_len);
    offset += key_len;
    
    vector<unsigned char> use_nonce(encryptedData.begin() + offset, encryptedData.begin() + offset + 12);
    offset += 12;
    
    vector<unsigned char> ciphertext(encryptedData.begin() + offset, encryptedData.end());
    
    // Дешифруем ChaCha-ключ RSA
    EVP_PKEY_CTX* rsa_ctx = EVP_PKEY_CTX_new(rsa_key, nullptr);
    EVP_PKEY_decrypt_init(rsa_ctx);
    EVP_PKEY_CTX_set_rsa_padding(rsa_ctx, RSA_PKCS1_PADDING);
    
    size_t dec_key_len;
    EVP_PKEY_decrypt(rsa_ctx, nullptr, &dec_key_len, enc_key.data(), enc_key.size());
    vector<unsigned char> dec_key(dec_key_len);
    EVP_PKEY_decrypt(rsa_ctx, dec_key.data(), &dec_key_len, enc_key.data(), enc_key.size());
    
    EVP_PKEY_CTX_free(rsa_ctx);
    
    // Дешифруем данные ChaCha20
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* cipher = EVP_chacha20_poly1305();
    
    unsigned char tag[16];
    copy(ciphertext.end() - 16, ciphertext.end(), tag);
    vector<unsigned char> actual_ct(ciphertext.begin(), ciphertext.end() - 16);
    
    EVP_DecryptInit_ex(ctx, cipher, nullptr, dec_key.data(), use_nonce.data());
    output.resize(actual_ct.size());
    int len;
    EVP_DecryptUpdate(ctx, output.data(), &len, actual_ct.data(), actual_ct.size());
    int out_len = len;
    
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, 16, tag);
    EVP_DecryptFinal_ex(ctx, output.data() + len, &len);
    out_len += len;
    output.resize(out_len);
    
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

extern "C" bool encryptText(const string& text, const vector<string>& keys, string& output, bool showSteps) {
    vector<unsigned char> data(text.begin(), text.end());
    vector<unsigned char> enc;
    bool ok = encryptBinary(data, keys, {}, enc, showSteps);
    if (!ok) return false;
    output.assign(enc.begin(), enc.end());
    return true;
}

extern "C" bool decryptText(const string& encryptedText, const vector<string>& keys, string& output, bool showSteps) {
    vector<unsigned char> data(encryptedText.begin(), encryptedText.end());
    vector<unsigned char> dec;
    bool ok = decryptBinary(data, keys, {}, dec, showSteps);
    if (!ok) return false;
    output.assign(dec.begin(), dec.end());
    return true;
}

extern "C" bool requiresNonce() {
    return true;
}

extern "C" size_t getNonceSize() {
    return 12;
}
