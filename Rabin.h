#ifndef RABIN_H
#define RABIN_H

#include <string>
#include <vector>

using namespace std;

bool isMod4Condition(int p);
int generatePrimeMod4(int min, int max);
void generateRabinParams(int& p, int& q, int& N, int& B);
int rabinEncrypt(int m, int N, int B);
vector<int> rabinEncryptString(const string& message, int N, int B);
string rabinDecryptString(const vector<int>& encrypted, int p, int q, int B);
int selectCorrectRoot(const vector<int>& roots);
vector<int> rabinDecrypt(int C, int p, int q, int B);

// ДОБАВИТЬ ЭТИ СТРОКИ:
void rabinEncrypt();
void rabinDecrypt();

#endif