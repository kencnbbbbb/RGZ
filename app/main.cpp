#include <iostream>
#include "PluginLoader.h"
#include "ConsoleUI.h"

using namespace std;

int main() {
    PluginLoader loader;
    
    // Загрузить плагины из папки plugins/
    string pluginsDir = "plugins";
    if (!loader.loadAllPlugins(pluginsDir)) {
        cerr << "Ошибка загрузки плагинов. Проверьте папку plugins/ с .dll/.so." << endl;
        return 1;
    }
    
    ConsoleUI ui(loader);
    ui.run();
    
    cout << "\nДо свидания!" << endl;
    return 0;
}
