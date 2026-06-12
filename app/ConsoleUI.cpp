#include "ConsoleUI.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>

#ifdef __linux__
    #include <cstring>
#endif

using namespace std;

ConsoleUI::ConsoleUI(PluginLoader& loader) : loader(loader), selectedPlugin(nullptr) {}

void ConsoleUI::run() {
    cout << "========================================" << endl;
    cout << "  Криптографическое приложение (РГР)" << endl;
    cout << "========================================" << endl;
    cout << "Загружено плагинов: " << loader.getPlugins().size() << endl << endl;
    
    while (true) {
        cout << "\nГлавное меню:" << endl;
        cout << "1. Работа с текстом" << endl;
        cout << "2. Работа с файлом" << endl;
        cout << "3. Генератор ключей" << endl;
        cout << "4. Выход" << endl;
        cout << "Выберите пункт: ";
        
        int choice;
        cin >> choice;
        clearInput();
        
        if (choice == 4) break;
        
        if (choice < 1 || choice > 4) {
            cerr << "Неверный пункт." << endl;
            continue;
        }
        
        if (!selectedPlugin && !selectPlugin()) continue;
        
        switch (choice) {
            case 1: handleTextMode(); break;
            case 2: handleFileMode(); break;
            case 3: handleKeyGenerator(); break;
        }
    }
}

bool ConsoleUI::selectPlugin() {
    const auto& plugins = loader.getPlugins();
    if (plugins.empty()) {
        cerr << "Нет загруженных плагинов!" << endl;
        return false;
    }
    
    cout << "\nДоступные алгоритмы:" << endl;
    for (size_t i = 0; i < plugins.size(); i++) {
        cout << (i + 1) << ". " << plugins[i].name << endl;
    }
    cout << "Выберите алгоритм: ";
    
    int choice;
    cin >> choice;
    clearInput();
    
    if (choice < 1 || choice > (int)plugins.size()) {
        cerr << "Неверный выбор." << endl;
        return false;
    }
    
    selectedPlugin = &plugins[choice - 1];
    cout << "Выбран: " << selectedPlugin->name << endl;
    return true;
}

void ConsoleUI::handleTextMode() {
    cout << "\n--- Работа с текстом ---" << endl;
    cout << "1. Шифровать текст" << endl;
    cout << "2. Дешифровать текст" << endl;
    cout << "Выберите действие: ";
    
    int choice;
    cin >> choice;
    clearInput();
    
    cout << "Введите текст: ";
    string text;
    getline(cin, text);
    
    cout << "Показывать шаги? (1-да,0-нет): ";
    bool showSteps;
    cin >> showSteps;
    clearInput();
    
    if (choice == 1) {
        string output;
        if (selectedPlugin->encryptText(text, currentKeys, output, showSteps)) {
            cout << "\nЗашифрованный текст:" << endl << output << endl;
            string save = selectFile("Сохранить результат в файл? Введите путь или оставьте пустым чтобы пропустить:");
            if (!save.empty()) {
                vector<unsigned char> data(output.begin(), output.end());
                writeBinaryFile(save, data);
            }
        }
    } else if (choice == 2) {
        string output;
        if (selectedPlugin->decryptText(text, currentKeys, output, showSteps)) {
            cout << "\nРасшифрованный текст:" << endl << output << endl;
            string save = selectFile("Сохранить результат в файл? Введите путь или оставьте пустым:");
            if (!save.empty()) {
                vector<unsigned char> data(output.begin(), output.end());
                writeBinaryFile(save, data);
            }
        }
    }
}

void ConsoleUI::handleFileMode() {
    cout << "\n--- Работа с бинарными файлами ---" << endl;
    cout << "1. Шифровать файл" << endl;
    cout << "2. Дешифровать файл" << endl;
    cout << "Выберите действие: ";
    
    int choice;
    cin >> choice;
    clearInput();
    
    string inputPath = selectFile("Введите путь к входному файлу или оставьте пустым чтобы пропустить:");
    if (inputPath.empty()) return;
    
    string outputPath = selectFile("Введите путь к выходному файлу:");
    if (outputPath.empty()) return;
    
    vector<unsigned char> data = readBinaryFile(inputPath);
    if (data.empty()) return;
    
    cout << "Показывать шаги? (1-да,0-нет): ";
    bool showSteps;
    cin >> showSteps;
    clearInput();
    
    vector<unsigned char> output;
    if (choice == 1) {
        if (selectedPlugin->encryptBinary(data, currentKeys, currentNonce, output, showSteps)) {
            if (writeBinaryFile(outputPath, output)) {
                cout << "Файл зашифрован и сохранён в " << outputPath << endl;
            }
        }
    } else if (choice == 2) {
        if (selectedPlugin->decryptBinary(data, currentKeys, currentNonce, output, showSteps)) {
            if (writeBinaryFile(outputPath, output)) {
                cout << "Файл расшифрован и сохранён в " << outputPath << endl;
            }
        }
    }
}

void ConsoleUI::handleKeyGenerator() {
    cout << "\n--- Генератор ключей ---" << endl;
    cout << "Показывать шаги? (1-да,0-нет): ";
    bool showSteps;
    cin >> showSteps;
    clearInput();
    
    selectedPlugin->generateKeys(currentKeys, showSteps);
    
    cout << "\nСгенерированные ключи:" << endl;
    for (const string& key : currentKeys) cout << key << endl;
    
    string savePath = selectFile("Сохранить ключи в файл? Введите путь или оставьте пустым:");
    if (!savePath.empty()) {
        ofstream f(savePath);
        if (f.is_open()) {
            for (const string& key : currentKeys) f << key << endl;
            f.close();
            cout << "Ключи сохранены в " << savePath << endl;
        }
    }
}

string ConsoleUI::selectFile(const string& prompt) {
    cout << prompt << endl;
    string path;
#ifdef __linux__
    // Попробовать zenity
    FILE* pipe = popen("zenity --file-selection --title='Выберите файл' 2>/dev/null", "r");
    if (pipe) {
        char buf[1024];
        if (fgets(buf, sizeof(buf), pipe)) {
            path = buf;
            if (!path.empty() && path.back() == '\n') path.pop_back();
        }
        pclose(pipe);
        if (!path.empty()) return path;
    }
#endif
    cout << "Введите путь к файлу (или нажмите Enter чтобы пропустить): ";
    getline(cin, path);
    return path;
}

vector<unsigned char> ConsoleUI::readBinaryFile(const string& path) {
    ifstream file(path, ios::binary);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл для чтения." << endl;
        return {};
    }
    return vector<unsigned char>((istreambuf_iterator<char>(file), istreambuf_iterator<char>());
}

bool ConsoleUI::writeBinaryFile(const string& path, const vector<unsigned char>& data) {
    ofstream file(path, ios::binary);
    if (!file.is_open()) {
        cerr << "Не удалось открыть файл для записи." << endl;
        return false;
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    return true;
}

void ConsoleUI::clearInput() {
    cin.clear();
    while (cin.get() != '\n');
}
