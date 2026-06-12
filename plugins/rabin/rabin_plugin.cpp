#include "../../include/ICryptoPlugin.h"
#include "Rabin.h"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// Глобальные переменные для хранения ключей (для плагина)
static int rabin_p = 0, rabin_q = 0, rabin_n = 0, rabin_b = 0;

extern "C" const char* getAlgorithmName() {
    return "Криптосистема Рабина";
}

extern "C" bool generateKeys(vector<string>& keys, bool showSteps) {
    cout << "Генерация ключей для Рабина..." << endl;
    generateRabinParams(rabin_p, rabin_q, rabin_n, rabin_b);
    keys = {
        "p=" + to_string(rabin_p),
        "q=" + to_string(rabin_q),
        "N=" + to_string(rabin_n),
        "B=" + to_string(rabin_b)
    };
    return true;
}

extern "C" bool encryptText(const string& text, const vector<string>& keys, string& output, bool showSteps) {
    cout << "Шифрование Рабина (текст)..." << endl;
    
    if (rabin_p == 0 || rabin_q == 0) {
        if (keys.size() < 4) return false;
        rabin_p = stoi(keys[0].substr(2));
        rabin_q = stoi(keys[1].substr(2));
        rabin_n = stoi(keys[2].substr(2));
        rabin_b = stoi(keys[3].substr(2));
    }
    
    vector<int> encrypted = rabinEncryptString(text, rabin_n, rabin_b);
    stringstream ss;
    for (int i = 0; i < encrypted.size(); i++) {
        if (i > 0) ss << " ";
        ss << encrypted[i];
    }
    output = ss.str();
    return true;
}

extern "C" bool decryptText(const string& encryptedText, const vector<string>& keys, string& output, bool showSteps) {
    cout << "Дешифрование Рабина (текст)..." << endl;
    
    if (rabin_p == 0 || rabin_q == 0) {
        if (keys.size() < 4) return false;
        rabin_p = stoi(keys[0].substr(2));
        rabin_q = stoi(keys[1].substr(2));
        rabin_n = stoi(keys[2].substr(2));
        rabin_b = stoi(keys[3].substr(2));
    }
    
    vector<int> encrypted;
    stringstream ss(encryptedText);
    int num;
    while (ss >> num) encrypted.push_back(num);
    output = rabinDecryptString(encrypted, rabin_p, rabin_q, rabin_b);
    return true;
}

// Бинарные данные: конвертируем в текст (для простоты)
extern "C" bool encryptBinary(const vector<unsigned char>& data, const vector<string>& keys, const vector<unsigned char>& nonce, vector<unsigned char>& output, bool showSteps) {
    string text(data.begin(), data.end());
    string encText;
    bool ok = encryptText(text, keys, encText, showSteps);
    if (!ok) return false;
    output.assign(encText.begin(), encText.end());
    return true;
}

extern "C" bool decryptBinary(const vector<unsigned char>& encryptedData, const vector<string>& keys, const vector<unsigned char>& nonce, vector<unsigned char>& output, bool showSteps) {
    string encText(encryptedData.begin(), encryptedData.end());
    string decText;
    bool ok = decryptText(encText, keys, decText, showSteps);
    if (!ok) return false;
    output.assign(decText.begin(), decText.end());
    return true;
}

extern "C" bool requiresNonce() {
    return false;
}

extern "C" size_t getNonceSize() {
    return 0;
}
