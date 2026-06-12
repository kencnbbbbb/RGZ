# RasGrafRab — Криптографическое приложение (РГР)
Приложение с **динамически подключаемыми библиотеками (плагинами)** для шифрования/дешифрования текста и бинарных файлов. Все алгоритмы загружаются из отдельных .dll (Windows) или .so (Linux) — **не жёстко зашиты в exe!**

---

## Требования
- Компилятор C++17 (GCC, Clang, MSVC)
- CMake ≥3.15
- OpenSSL (библиотеки + заголовки)
- Linux: `libssl-dev`, `build-essential`
- Windows: vcpkg (для установки OpenSSL)

---

## Сборка и запуск

### 1. Linux (Debian/Ubuntu/Mint)
```bash
# Установить зависимости
sudo apt-get update
sudo apt-get install build-essential cmake libssl-dev zenity

# Клонировать/перейти в папку проекта
cd ~/path/to/RGZ

# Сборка
mkdir build && cd build
cmake ..
make -j$(nproc)

# Запуск (плагины лежат в build/plugins/)
./RasGrafRab
```

### 2. Windows (Visual Studio + vcpkg)
1. Установите vcpkg:
   ```powershell
   cd C:\
   git clone https://github.com/microsoft/vcpkg
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install
   .\vcpkg install openssl:x64-windows
   ```
2. Сборка:
   ```powershell
   cd C:\Users\Maks\Desktop\ргз
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release
   ```
3. Запустите: `Release\RasGrafRab.exe`

---

## Архитектура
```
ргз/
├── app/               # Основное приложение
│   ├── main.cpp       # Точка входа
│   ├── ConsoleUI.h/cpp # Интерфейс
│   └── PluginLoader.h/cpp # Загрузчик плагинов
├── include/
│   └── ICryptoPlugin.h # Единый интерфейс плагинов
├── plugins/           # Динамические библиотеки
│   ├── rabin/
│   ├── mess/
│   ├── a1ya33/
│   ├── shamir/
│   ├── rsa/
│   └── chacha20/
└── CMakeLists.txt
```

---

## Функционал (по ТЗ)
✅ **Динамические плагины**: алгоритмы в отдельных .dll/.so  
✅ **Универсальный интерфейс ICryptoPlugin**: все плагины имеют одинаковые функции  
✅ **Бинарные файлы**: чтение/запись в ios::binary режиме  
✅ **Меню по ТЗ**:
  1. Работа с текстом (шифр/дешифр)
  2. Работа с бинарными файлами
  3. Генератор ключей
  4. Выход
✅ **Русский интерфейс**
✅ **Обработка ошибок**: проверка файлов, ключей и т.д.

---

## Плагины
- **Криптосистема Рабина**
- **Протокол Месси-Омуры**
- **Шифр А1Я33**
- **Шифр Шамира**
- **RSA (гибрид с ChaCha20)**
- **ChaCha20-Poly1305**

---
