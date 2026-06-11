#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

void print_hex(const std::string& label, const std::vector<unsigned char>& data);
std::vector<unsigned char> read_file(const std::string& filename);
void write_file(const std::string& filename, const std::vector<unsigned char>& data);
std::string select_file();

#endif
