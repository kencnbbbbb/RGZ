# Простой Makefile для компиляции C++ программы шифрования
# Использование:
#   make         - компилировать программу
#   make clean   - удалить скомпилированные файлы

CC = g++
CFLAGS = -std=c++17 -Wall -Wextra -O2
LIBS = -lssl -lcrypto

TARGET = crypto_demo
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)
	@echo "Программа скомпилирована успешно!"
	@echo "Запустите: ./$(TARGET)"

clean:
	rm -f $(TARGET)
	rm -f *.enc *.key *.nonce *.decrypted
	@echo "Файлы очищены!"

.PHONY: all clean
