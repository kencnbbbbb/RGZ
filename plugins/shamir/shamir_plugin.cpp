#include "../../include/ICryptoPlugin.h"
#include "Shamir.h"
#include <iostream>
#include <vector>

using namespace std;

extern "C" const char* getAlgorithmName() {
    return "Шифр Шамира";
}

extern "C" bool generateKeys(vector<string>& keys, bool showSteps) {
    cout << "Шифр Шамира: ключи генерируются автоматически при создании объекта." << endl;
    keys = {};
    return true;
}

extern "C" bool encryptText(const string& text, const vector<string>& keys, string& output, bool showSteps) {
    ShamirCipher cipher;
    if (!cipher.keysAreCorrect()) return false;
    output = cipher.encrypt(text, showSteps);
    return true;
}

extern "C" bool decryptText(const string& encryptedText, const vector<string>& keys, string& output, bool showSteps) {
    ShamirCipher cipher;
    if (!cipher.keysAreCorrect()) return false;
    output = cipher.decrypt(encryptedText, showSteps);
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
