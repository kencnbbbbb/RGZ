#include <iostream>
#include "Rabin.h"
#include "mess.h"
#include <random>
#include <sstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

// Объявление внешних функций из main.cpp
string inputMessage();
void saveResult(const string& result);
void clearInput();

bool isMod4Condition(int p) {
    return (p % 4 == 3);
}

int generatePrimeMod4(int min, int max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(min, max);
    while (true) {
        int num = dist(gen);
        if (num % 2 == 0) num++;
        if (num < min) num = min;
        while (num <= max) {
            if (isPrime(num) && isMod4Condition(num)) {
                return num;
            }
            num += 2;
        }
    }
}

void generateRabinParams(int& p, int& q, int& N, int& B) {
    cout << "Генерация параметров для криптосистемы Рабина...\n";
    p = generatePrimeMod4(300, 1000);
    do {
        q = generatePrimeMod4(300, 1000);
    } while (q == p);
    N = p * q;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, N - 1);
    B = dist(gen);
    cout << "p = " << p << " (p % 4 = " << p % 4 << ")\n";
    cout << "q = " << q << " (q % 4 = " << q % 4 << ")\n";
    cout << "N = " << N << endl;
    cout << "B = " << B << endl;
}

int rabinEncrypt(int m, int N, int B) {
    if (m < 0 || m > 255) {
        throw runtime_error("m должен быть в диапазоне 0-255");
    }
    return (m * (m + B)) % N;
}

vector<int> rabinEncryptString(const string& message, int N, int B) {
    vector<int> result;
    for (char c : message) {
        int m = static_cast<unsigned char>(c);
        result.push_back(rabinEncrypt(m, N, B));
    }
    return result;
}

int sqrtModPrime(int a, int p) {
    if (p % 4 != 3) {
        throw runtime_error("p должно быть ≡ 3 (mod 4)");
    }
    a = ((a % p) + p) % p;
    return modPow(a, (p + 1) / 4, p);
}

pair<int, int> solveQuadraticModPrime(int C, int B, int p) {
    int B_squared = (B % p) * (B % p) % p;
    int four_C = (4 % p) * (C % p) % p;
    int D = (B_squared + four_C) % p;
    int sqrtD = sqrtModPrime(D, p);
    int inv2 = modInverse(2, p);
    int minus_B = (-B % p + p) % p;
    int m1 = ((minus_B + sqrtD) % p) * inv2 % p;
    int m2 = ((minus_B - sqrtD + p) % p) * inv2 % p;
    return {m1, m2};
}

vector<int> rabinDecrypt(int C, int p, int q, int B) {
    vector<int> roots;
    auto roots_p = solveQuadraticModPrime(C, B, p);
    auto roots_q = solveQuadraticModPrime(C, B, q);
    for (int rp : {roots_p.first, roots_p.second}) {
        for (int rq : {roots_q.first, roots_q.second}) {
            int root = crt(rp, rq, p, q);
            roots.push_back(root);
        }
    }
    sort(roots.begin(), roots.end());
    roots.erase(unique(roots.begin(), roots.end()), roots.end());
    return roots;
}

int selectCorrectRoot(const vector<int>& roots) {
    for (int root : roots) {
        if (root >= 0 && root <= 255) {
            return root;
        }
    }
    return roots.empty() ? -1 : roots[0];
}

string rabinDecryptString(const vector<int>& encrypted, int p, int q, int B) {
    string result;
    for (int c : encrypted) {
        vector<int> roots = rabinDecrypt(c, p, q, B);
        int m = selectCorrectRoot(roots);
        if (m >= 0 && m <= 255) {
            result.push_back(static_cast<char>(m));
        } else {
            result.push_back('?');
        }
    }
    return result;
}

// Функция шифрования Рабина
void rabinEncrypt() {
    cout << "\n   Шифрование Рабина \n";
    string message = inputMessage();
    if (message.empty()) return;

    int p, q, N, B;
    cout << "Сгенерировать параметры? (y/n): ";
    char choice;
    cin >> choice;
    clearInput();

    if (choice == 'y' || choice == 'Y') {
        generateRabinParams(p, q, N, B);
    } else {
        cout << "Введите p: ";
        cin >> p;
        cout << "Введите q: ";
        cin >> q;
        cout << "Введите B: ";
        cin >> B;
        clearInput();
        N = p * q;
        if (!isPrime(p) || !isPrime(q)) {
            cout << "Ошибка: p и q должны быть простыми\n";
            return;
        }
        if (!isMod4Condition(p) || !isMod4Condition(q)) {
            cout << "Ошибка: p и q должны быть ≡ 3 mod 4\n";
            return;
        }
    }

    try {
        vector<int> encrypted = rabinEncryptString(message, N, B);
        string result = "Зашифрованное сообщение (Рабин)\n";
        result += "p = " + to_string(p) + "\n";
        result += "q = " + to_string(q) + "\n";
        result += "N = " + to_string(N) + "\n";
        result += "B = " + to_string(B) + "\n";
        result += "Данные: ";
        for (int v : encrypted) result += to_string(v) + " ";
        saveResult(result);
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

// Функция дешифрования Рабина
void rabinDecrypt() {
    cout << "\n   Дешифрование Рабина \n";

    int p, q, B;
    cout << "Введите p: ";
    cin >> p;
    cout << "Введите q: ";
    cin >> q;
    cout << "Введите B: ";
    cin >> B;
    clearInput();

    cout << "Введите зашифрованные числа через пробел: ";
    string line;
    getline(cin, line);

    vector<int> encrypted;
    stringstream ss(line);
    int num;
    while (ss >> num) encrypted.push_back(num);

    try {
        string decrypted = rabinDecryptString(encrypted, p, q, B);
        cout << "Дешифрованное сообщение:\n" << decrypted << endl;
        saveResult(decrypted);
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}