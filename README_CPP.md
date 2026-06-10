# Crypto Demo (C++ версия)

Простая программа на C++ для шифрования и дешифрования файлов!

## Зависимости

Нужно установить OpenSSL (для шифрования):

**Linux (Debian/Ubuntu):
```bash
sudo apt install build-essential libssl-dev zenity
```

**Fedora:
```bash
sudo dnf install gcc-c++ openssl-devel zenity
```

**Arch Linux:
```bash
sudo pacman -S base-devel openssl zenity
```

## Компиляция и запуск

Скомпилировать и запустить одной командой (GNU Make):
```bash
make
./crypto_demo
```

Или вручную без Makefile:
```bash
g++ -std=c++17 -o crypto_demo main.cpp -lssl -lcrypto
./crypto_demo
```

## Что делает код?

**Основные функции:
1. **print_hex() - выводит данные в hex-формате
2. **read_file() - читает файл в память
3. **write_file() - записывает данные в файл
4. **select_file() - выбирает файл через проводник (Linux: zenity)
5. **chacha20_encrypt_decrypt() - шифрует и дешифрует ChaCha20-Poly1305
6. **rsa_encrypt_decrypt() - гибридное шифрование (RSA для ключа, ChaCha20 для файла

## Использование

1. Запустите программу
2. Выберите метод шифрования (1 или 2)
3. Выберите файл в открывшемся проводнике
4. Посмотрите результат!
