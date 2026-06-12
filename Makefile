# Makefile для шифровальной программы
CC = g++
CFLAGS = -std=c++17 -Wall -Wextra -O2
LIBS = -lssl -lcrypto
TARGET = crypto_demo
OBJS = main.o utils.o rsa_crypto.o chacha20_crypto.o Rabin.o mess.o A1Ya33.o Shamir.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)
	@echo "Компиляция прошла успешно!"
	@echo "Запустите: ./$(TARGET)"

main.o: main.cpp rsa_crypto.h chacha20_crypto.h utils.h Rabin.h mess.h A1Ya33.h Shamir.h
	$(CC) $(CFLAGS) -c main.cpp

utils.o: utils.cpp utils.h
	$(CC) $(CFLAGS) -c utils.cpp

rsa_crypto.o: rsa_crypto.cpp rsa_crypto.h utils.h
	$(CC) $(CFLAGS) -c rsa_crypto.cpp

chacha20_crypto.o: chacha20_crypto.cpp chacha20_crypto.h utils.h
	$(CC) $(CFLAGS) -c chacha20_crypto.cpp

Rabin.o: Rabin.cpp Rabin.h mess.h
	$(CC) $(CFLAGS) -c Rabin.cpp

mess.o: mess.cpp mess.h
	$(CC) $(CFLAGS) -c mess.cpp

A1Ya33.o: A1Ya33.cpp A1Ya33.h
	$(CC) $(CFLAGS) -c A1Ya33.cpp

Shamir.o: Shamir.cpp Shamir.h
	$(CC) $(CFLAGS) -c Shamir.cpp

clean:
	rm -f $(TARGET) $(OBJS)
	rm -f *.enc *.key *.nonce *.decrypted result.txt
	@echo "Файлы очищены!"

.PHONY: all clean
