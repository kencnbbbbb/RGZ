#ifndef MESS_H
#define MESS_H

#include <string>
#include <vector>

using namespace std;

bool isPrime(int n);
int generatePrime(int min, int max);
int modPow(int base, int exp, int mod);
int gcd(int a, int b);
int modInverse(int a, int mod);
int crt(int a1, int a2, int m1, int m2);

vector<int> stringToNumbers(const string& str);
string numbersToString(const vector<int>& numbers);

void generateKeys(int p, int& key1, int& key2);
vector<int> encryptMessage(const string& message, int p, int a1);
string encryptedToString(const vector<int>& data);

// ДОБАВИТЬ ЭТИ СТРОКИ:
void masseyOmuraEncrypt();
void masseyOmuraDecrypt();

#endif