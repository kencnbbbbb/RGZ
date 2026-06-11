#include "utils.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void print_hex(const string& label, const vector<unsigned char>& data) {
    cout << "\n--- " << label << " ---" << endl;
    for (size_t i = 0; i < data.size(); ++i) {
        printf("%02x", data[i]);
    }
    cout << endl;
    cout << "Размер: " << data.size() << " байт" << endl;
}

vector<unsigned char> read_file(const string& filename) {
    ifstream file(filename, ios::binary);
    vector<unsigned char> buffer((istreambuf_iterator<char>(file)), 
                                  istreambuf_iterator<char>());
    return buffer;
}

void write_file(const string& filename, const vector<unsigned char>& data) {
    ofstream file(filename, ios::binary);
    file.write((const char*)data.data(), data.size());
}

string select_file() {
#ifdef __linux__
    cout << "Открытие проводника (используем zenity)..." << endl;
    FILE* pipe = popen("zenity --file-selection --title='Выберите файл для шифрования'", "r");
    if (!pipe) return "";
    
    char buffer[1024];
    string filename = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        filename += buffer;
    }
    pclose(pipe);
    
    if (!filename.empty() && filename.back() == '\n') {
        filename.pop_back();
    }
    return filename;
#else
    string filename;
    cout << "Введите путь к файлу: ";
    cin >> filename;
    return filename;
#endif
}
