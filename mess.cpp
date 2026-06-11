#include <iostream>
#include "mess.h"
#include <random>
#include <sstream>
#include <stdexcept>

using namespace std;

// Объявление внешних функций из main.cpp
string inputMessage();
void saveResult(const string& result);
void clearInput();

bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

int generatePrime(int min, int max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(min, max);
    while (true) {
        int candidate = dis(gen);
        if (isPrime(candidate)) return candidate;
    }
}

int modPow(int base, int exponent, int modulus) {
    if (modulus == 1) return 0;
    int result = 1;
    base = base % modulus;
    while (exponent > 0) {
        if (exponent & 1) result = (result * base) % modulus;
        base = (base * base) % modulus;
        exponent >>= 1;
    }
    return result;
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int modInverse(int a, int m) {
    int m0 = m, y = 0, x = 1;
    if (m == 1) return 0;
    int a_temp = a, m_temp = m;
    while (a_temp > 1) {
        int q = a_temp / m_temp;
        int t = m_temp;
        m_temp = a_temp % m_temp;
        a_temp = t;
        t = y;
        y = x - q * y;
        x = t;
    }
    if (x < 0) x += m0;
    return x;
}

int crt(int a1, int a2, int m1, int m2) {
    int M = m1 * m2;
    int M1 = m2;
    int M2 = m1;
    int y1 = modInverse(M1 % m1, m1);
    int y2 = modInverse(M2 % m2, m2);
    int x = (a1 * (M1 % M) % M * y1 % M + a2 * (M2 % M) % M * y2 % M) % M;
    return x;
}

vector<int> stringToNumbers(const string& text) {
    vector<int> numbers;
    for (size_t i = 0; i < text.length(); i++) {
        numbers.push_back(static_cast<int>(static_cast<unsigned char>(text[i])));
    }
    return numbers;
}

string numbersToString(const vector<int>& numbers) {
    string text;
    for (int num : numbers) {
        text.push_back(static_cast<char>(static_cast<unsigned char>(num)));
    }
    return text;
}

void generateKeys(int p, int& key1, int& key2) {
    int phi = p - 1;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(2, phi - 1);
    do {
        key1 = dis(gen);
    } while (gcd(key1, phi) != 1);
    key2 = modInverse(key1, phi);
}

vector<int> encryptMessage(const string& message, int p, int a1) {
    vector<int> numbers = stringToNumbers(message);
    vector<int> result;
    for (int num : numbers) {
        if (num >= p) {
            throw runtime_error("Число превышает модуль p");
        }
        result.push_back(modPow(num, a1, p));
    }
    return result;
}

string encryptedToString(const vector<int>& encrypted) {
    stringstream ss;
    for (size_t i = 0; i < encrypted.size(); ++i) {
        if (i > 0) ss << " ";
        ss << encrypted[i];
    }
    return ss.str();
}

// Функция шифрования Месси-Омуры
void masseyOmuraEncrypt() {
    cout << "\n   Шифрование Месси-Омуры \n";
    string message = inputMessage();
    if (message.empty()) return;

    int p;
    cout << "Введите p (0 - сгенерировать): ";
    cin >> p;
    clearInput();

    if (p == 0) {
        p = generatePrime(300, 1000);
        cout << "Сгенерировано p = " << p << endl;
    } else if (!isPrime(p)) {
        cout << "Ошибка: p не простое\n";
        return;
    }

    int a1, a2, b1, b2;
    generateKeys(p, a1, a2);
    generateKeys(p, b1, b2);

    cout << "Ключи A: (" << a1 << ", " << a2 << ")\n";
    cout << "Ключи B: (" << b1 << ", " << b2 << ")\n";

    try {
        vector<int> encrypted = encryptMessage(message, p, a1);
        string result = "Зашифрованное сообщение (Месси-Омура)\n";
        result += "p = " + to_string(p) + "\n";
        result += "Ключи A: (" + to_string(a1) + ", " + to_string(a2) + ")\n";
        result += "Ключи B: (" + to_string(b1) + ", " + to_string(b2) + ")\n";
        result += "Данные: " + encryptedToString(encrypted) + "\n";
        saveResult(result);
    } catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
    }
}

// Функция дешифрования Месси-Омуры (упрощенная)
void masseyOmuraDecrypt() {
    cout << "\n   Дешифрование Месси-Омуры \n";

    int p, a2, b1, b2;

    cout << "Введите p: ";
    cin >> p;

    // Для дешифрования данных после этапа 1 нужны b1, a2, b2
    cout << "Введите b1 (первый ключ B): ";
    cin >> b1;

    cout << "Введите a2 (второй ключ A): ";
    cin >> a2;

    cout << "Введите b2 (второй ключ B): ";
    cin >> b2;

    clearInput();

    cout << "Введите зашифрованные числа: ";
    string line;
    getline(cin, line);

    vector<int> encrypted;
    stringstream ss(line);
    int num;
    while (ss >> num) encrypted.push_back(num);

    // Продолжаем протокол с того места, где остановились
    // У нас есть E_a1(M). Нужно:
    // 1. Применить b1: E_b1(E_a1(M))
    // 2. Применить a2: D_a2(E_b1(E_a1(M))) = E_b1(M)
    // 3. Применить b2: D_b2(E_b1(M)) = M

    cout << "\nЭтап 2: применяем b1=" << b1 << endl;
    vector<int> stage2;
    for (int val : encrypted) {
        int res = modPow(val, b1, p);
        stage2.push_back(res);
        cout << val << "^" << b1 << " mod " << p << " = " << res << endl;
    }

    cout << "\nЭтап 3: применяем a2=" << a2 << endl;
    vector<int> stage3;
    for (int val : stage2) {
        int res = modPow(val, a2, p);
        stage3.push_back(res);
        cout << val << "^" << a2 << " mod " << p << " = " << res << endl;
    }

    cout << "\nЭтап 4: применяем b2=" << b2 << endl;
    vector<int> stage4;
    for (int val : stage3) {
        int res = modPow(val, b2, p);
        stage4.push_back(res);
        cout << val << "^" << b2 << " mod " << p << " = " << res << endl;
    }

    cout << "\nЧисла после дешифрования: ";
    for (int val : stage4) {
        cout << val << " ";
    }
    cout << endl;

    string result = numbersToString(stage4);
    cout << "\nДешифрованное сообщение:\n" << result << endl;
    saveResult(result);
}