#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

using namespace std;

void print_hex(const string& label, const vector<unsigned char>& data);
vector<unsigned char> read_file(const string& filename);
void write_file(const string& filename, const vector<unsigned char>& data);
string select_file();

#endif
