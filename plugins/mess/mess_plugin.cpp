#include "../../include/ICryptoPlugin.h"
#include "mess.h"
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// Глобальные переменные для ключей
static int mess_p = 0, mess_a1 = 0, mess_a2 = 0, mess_b1 = 0, mess_b2 = 0;

extern "C" const char* getAlgorithmName() {
    return "Протокол Месси-Омуры";
}

extern "C" bool generateKeys(vector<string>& keys, bool showSteps) {
    cout << "Генерация ключей для Месси-Омуры..." << endl;
    mess_p = generatePrime(300, 1000);
    generateKeys(mess_p, mess_a1, mess_a2);
    generateKeys(mess_p, mess_b1, mess_b2);
    
    keys = {
        "p=" + to_string(mess_p),
        "a1=" + to_string(mess_a1),
        "a2=" + to_string(mess_a2),
        "b1=" + to_string(mess_b1),
        "b2=" + to_string(mess_b2)
    };
    cout << "Ключи A: a1=" << mess_a1 << ", a2=" << mess_a2 << endl;
    cout << "Ключи B: b1=" << mess_b1 << ", b2=" << mess_b2 << endl;
    return true;
}

extern "C" bool encryptText(const string& text, const vector<string>& keys, string& output, bool showSteps) {
    cout << "Шифрование Месси-Омуры (текст)..." << endl;
    
    if (mess_p == 0) {
        if (keys.size() < 5) return false;
        mess_p = stoi(keys[0].substr(2));
        mess_a1 = stoi(keys[1].substr(3));
        mess_a2 = stoi(keys[2].substr(3));
        mess_b1 = stoi(keys[3].substr(3));
        mess_b2 = stoi(keys[4].substr(3));
    }
    
    vector<int> encrypted = encryptMessage(text, mess_p, mess_a1);
    output = encryptedToString(encrypted);
    return true;
}

extern "C" bool decryptText(const string& encryptedText, const vector<string>& keys, string& output, bool showSteps) {
    cout << "Дешифрование Месси-Омуры (текст)..." << endl;
    
    if (mess_p == 0) {
        if (keys.size() < 5) return false;
        mess_p = stoi(keys[0].substr(2));
        mess_a1 = stoi(keys[1].substr(3));
        mess_a2 = stoi(keys[2].substr(3));
        mess_b1 = stoi(keys[3].substr(3));
        mess_b2 = stoi(keys[4].substr(3));
    }
    
    vector<int> encrypted;
    stringstream ss(encryptedText);
    int num;
    while (ss >> num) encrypted.push_back(num);
    
    // Имитация полного протокола: E_a1 → E_b1(E_a1) → D_a2(E_b1(E_a1)) → D_b2 = текст
    vector<int> step1, step2, step3;
    for (int val : encrypted) {
        step1.push_back(modPow(val, mess_b1, mess_p));
    }
    for (int val : step1) {
        step2.push_back(modPow(val, mess_a2, mess_p));
    }
    for (int val : step2) {
        step3.push_back(modPow(val, mess_b2, mess_p));
    }
    
    output = numbersToString(step3);
    return true;
}

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
