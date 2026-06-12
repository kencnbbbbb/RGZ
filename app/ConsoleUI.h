#ifndef CONSOLEUI_H
#define CONSOLEUI_H

#include "PluginLoader.h"
#include <string>
#include <vector>

class ConsoleUI {
public:
    ConsoleUI(PluginLoader& loader);
    
    // Запустить главный цикл приложения
    void run();

private:
    PluginLoader& loader;
    std::vector<std::string> currentKeys;
    std::vector<unsigned char> currentNonce;
    const Plugin* selectedPlugin;

    // Выбрать плагин из списка
    bool selectPlugin();
    // Работа с текстом
    void handleTextMode();
    // Работа с файлами
    void handleFileMode();
    // Генератор ключей
    void handleKeyGenerator();
    // Выбрать файл через простой ввод или zenity (Linux)
    std::string selectFile(const std::string& prompt);
    // Чтение бинарного файла
    std::vector<unsigned char> readBinaryFile(const std::string& path);
    // Запись бинарного файла
    bool writeBinaryFile(const std::string& path, const std::vector<unsigned char>& data);
    // Очистка ввода
    void clearInput();
};

#endif // CONSOLEUI_H
