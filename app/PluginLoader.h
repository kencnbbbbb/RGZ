#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <string>
#include <vector>
#include <memory>

#ifdef _WIN32
    #include <windows.h>
    #define PLUGIN_HANDLE HMODULE
    #define PLUGIN_EXT ".dll"
#else
    #include <dlfcn.h>
    #define PLUGIN_HANDLE void*
    #define PLUGIN_EXT ".so"
#endif

#include "ICryptoPlugin.h"

// Структура для хранения информации о плагине
struct Plugin {
    std::string name;           // Название алгоритма
    std::string path;           // Путь к библиотеке
    PLUGIN_HANDLE handle;       // Хэндл библиотеки
    
    // Указатели на функции из плагина
    const char* (*getAlgorithmName)();
    bool (*generateKeys)(std::vector<std::string>&, bool);
    bool (*encryptBinary)(const std::vector<unsigned char>&, const std::vector<std::string>&, const std::vector<unsigned char>&, std::vector<unsigned char>&, bool);
    bool (*decryptBinary)(const std::vector<unsigned char>&, const std::vector<std::string>&, const std::vector<unsigned char>&, std::vector<unsigned char>&, bool);
    bool (*encryptText)(const std::string&, const std::vector<std::string>&, std::string&, bool);
    bool (*decryptText)(const std::string&, const std::vector<std::string>&, std::string&, bool);
    bool (*requiresNonce)();
    size_t (*getNonceSize)();
};

class PluginLoader {
public:
    PluginLoader();
    ~PluginLoader();

    // Загрузить все плагины из папки plugins/
    bool loadAllPlugins(const std::string& pluginsDir);
    // Получить список загруженных плагинов
    const std::vector<Plugin>& getPlugins() const;
    // Найти плагин по названию
    const Plugin* findPlugin(const std::string& name) const;

private:
    std::vector<Plugin> plugins;

    // Загрузить один плагин
    bool loadPlugin(const std::string& path);
    // Закрыть все плагины
    void unloadAll();
};

#endif // PLUGINLOADER_H
