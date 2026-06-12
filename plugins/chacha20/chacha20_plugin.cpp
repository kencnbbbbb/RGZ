#include "../../include/ICryptoPlugin.h"
#include "../utils.h"
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Глобальные ключи для ChaCha20
static unsigned char chacha_key[32];
static unsigned char chacha_nonce[12];

extern "C" const char* getAlgorithmName() {
    return "ChaCha20-Poly1305";
}

extern "C" bool generateKeys(vector<string>& keys, bool showSteps) {
    cout << "Генерация ключа для ChaCha20 (256 бит)..." << endl;
    RAND_bytes(chacha_key, sizeof(chacha_key));
    RAND_bytes(chacha_nonce, sizeof(chacha_nonce));
    keys = { "KEY=" + string(reinterpret_cast<char*>(chacha_key), 32),
             "NONCE=" + string(reinterpret_cast<char*>(chacha_nonce), 12) };
    cout << "Ключи ChaCha20 сгенерированы." << endl;
    return true;
}

extern "C" bool encryptBinary(const vector<unsigned char>& data, const vector<string>& keys, const vector<unsigned char>& nonce, vector<unsigned char>& output, bool showSteps) {
    cout << "Шифрование ChaCha20-Poly1305..." << endl;
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* cipher = EVP_chacha20_poly1305();
    
    EVP_EncryptInit_ex(ctx, cipher, nullptr, chacha_key, chacha_nonce);
    
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
    output = ciphertext;
    return true;
}

extern "C" bool decryptBinary(const vector<unsigned char>& encryptedData, const vector<string>& keys, const vector<unsigned char>& nonce, vector<unsigned char>& output, bool showSteps) {
    cout << "Дешифрование ChaCha20-Poly1305..." << endl;
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER* cipher = EVP_chacha20_poly1305();
    
    unsigned char tag[16];
    copy(encryptedData.end() - 16, encryptedData.end(), tag);
    vector<unsigned char> actual_ct(encryptedData.begin(), encryptedData.end() - 16);
    
    EVP_DecryptInit_ex(ctx, cipher, nullptr, chacha_key, chacha_nonce);
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
