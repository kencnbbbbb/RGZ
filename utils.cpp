#include "utils.h"
#include <iostream>
#include <fstream>
#include <vector>

void print_hex(const std::string& label, const std::vector<unsigned char>& data) {
    std::cout << "\n--- " << label << " ---" << std::endl;
    for (size_t i = 0; i < data.size(); ++i) {
        printf("%02x", data[i]);
    }
    std::cout << std::endl;
    std::cout << "Размер: " << data.size() << " байт" << std::endl;
}

std::vector<unsigned char> read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)), 
                                      std::istreambuf_iterator<char>());
    return buffer;
}

void write_file(const std::string& filename, const std::vector<unsigned char>& data) {
    std::ofstream file(filename, std::ios::binary);
    file.write((const char*)data.data(), data.size());
}

std::string select_file() {
#ifdef __linux__
    std::cout << "Открытие проводника (используем zenity)..." << std::endl;
    FILE* pipe = popen("zenity --file-selection --title='Выберите файл для шифрования'", "r");
    if (!pipe) return "";
    
    char buffer[1024];
    std::string filename = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        filename += buffer;
    }
    pclose(pipe);
    
    if (!filename.empty() && filename.back() == '\n') {
        filename.pop_back();
    }
    return filename;
#else
    std::string filename;
    std::cout << "Введите путь к файлу: ";
    std::cin >> filename;
    return filename;
#endif
}
