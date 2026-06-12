#include "PluginLoader.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>

using namespace std;
namespace fs = std::filesystem;

PluginLoader::PluginLoader() {}

PluginLoader::~PluginLoader() {
    unloadAll();
}

bool PluginLoader::loadAllPlugins(const std::string& pluginsDir) {
    cout << "Загрузка плагинов из " << pluginsDir << endl;
    
    if (!fs::exists(pluginsDir)) {
        cerr << "Папка плагинов не существует: " << pluginsDir << endl;
        return false;
    }

    for (const auto& entry : fs::directory_iterator(pluginsDir)) {
        if (entry.is_regular_file() && entry.path().extension() == PLUGIN_EXT) {
            string path = entry.path().string();
            if (loadPlugin(path)) {
                cout << "Загружен: " << plugins.back().name << endl;
            } else {
                cerr << "Ошибка загрузки: " << path << endl;
            }
        }
    }

    return !plugins.empty();
}

bool PluginLoader::loadPlugin(const std::string& path) {
    Plugin plugin;
    plugin.path = path;

#ifdef _WIN32
    plugin.handle = LoadLibraryA(path.c_str());
    if (!plugin.handle) {
        cerr << "LoadLibrary failed: " << GetLastError() << endl;
        return false;
    }
#else
    plugin.handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!plugin.handle) {
        cerr << "dlopen failed: " << dlerror() << endl;
        return false;
    }
#endif

    // Загрузить функции
#ifdef _WIN32
    plugin.getAlgorithmName = (const char* (*)())GetProcAddress(plugin.handle, "getAlgorithmName");
    plugin.generateKeys = (bool (*)(std::vector<std::string>&, bool))GetProcAddress(plugin.handle, "generateKeys");
    plugin.encryptBinary = (bool (*)(const std::vector<unsigned char>&, const std::vector<std::string>&, const std::vector<unsigned char>&, std::vector<unsigned char>&, bool))GetProcAddress(plugin.handle, "encryptBinary");
    plugin.decryptBinary = (bool (*)(const std::vector<unsigned char>&, const std::vector<std::string>&, const std::vector<unsigned char>&, std::vector<unsigned char>&, bool))GetProcAddress(plugin.handle, "decryptBinary");
    plugin.encryptText = (bool (*)(const std::string&, const std::vector<std::string>&, std::string&, bool))GetProcAddress(plugin.handle, "encryptText");
    plugin.decryptText = (bool (*)(const std::string&, const std::vector<std::string>&, std::string&, bool))GetProcAddress(plugin.handle, "decryptText");
    plugin.requiresNonce = (bool (*)())GetProcAddress(plugin.handle, "requiresNonce");
    plugin.getNonceSize = (size_t (*)())GetProcAddress(plugin.handle, "getNonceSize");
#else
    plugin.getAlgorithmName = (const char* (*)())dlsym(plugin.handle, "getAlgorithmName");
    plugin.generateKeys = (bool (*)(std::vector<std::string>&, bool))dlsym(plugin.handle, "generateKeys");
    plugin.encryptBinary = (bool (*)(const std::vector<unsigned char>&, const std::vector<std::string>&, const std::vector<unsigned char>&, std::vector<unsigned char>&, bool))dlsym(plugin.handle, "encryptBinary");
    plugin.decryptBinary = (bool (*)(const std::vector<unsigned char>&, const std::vector<std::string>&, const std::vector<unsigned char>&, std::vector<unsigned char>&, bool))dlsym(plugin.handle, "decryptBinary");
    plugin.encryptText = (bool (*)(const std::string&, const std::vector<std::string>&, std::string&, bool))dlsym(plugin.handle, "encryptText");
    plugin.decryptText = (bool (*)(const std::string&, const std::vector<std::string>&, std::string&, bool))dlsym(plugin.handle, "decryptText");
    plugin.requiresNonce = (bool (*)())dlsym(plugin.handle, "requiresNonce");
    plugin.getNonceSize = (size_t (*)())dlsym(plugin.handle, "getNonceSize");
#endif

    if (!plugin.getAlgorithmName) {
#ifdef _WIN32
        FreeLibrary(plugin.handle);
#else
        dlclose(plugin.handle);
#endif
        cerr << "Ошибка загрузки функции getAlgorithmName" << endl;
        return false;
    }

    plugin.name = plugin.getAlgorithmName();
    plugins.push_back(plugin);
    return true;
}

const std::vector<Plugin>& PluginLoader::getPlugins() const {
    return plugins;
}

const Plugin* PluginLoader::findPlugin(const std::string& name) const {
    for (const auto& plugin : plugins) {
        if (plugin.name == name) {
            return &plugin;
        }
    }
    return nullptr;
}

void PluginLoader::unloadAll() {
    for (auto& plugin : plugins) {
        if (plugin.handle) {
#ifdef _WIN32
            FreeLibrary(plugin.handle);
#else
            dlclose(plugin.handle);
#endif
            plugin.handle = nullptr;
        }
    }
    plugins.clear();
}
