#ifndef ICRYPTOPLUGIN_H
#define ICRYPTOPLUGIN_H

#include <string>
#include <vector>

// Единый интерфейс для всех криптоплагинов
extern "C" {
    // Получить название алгоритма
    const char* getAlgorithmName();

    // Сгенерировать ключи/параметры
    // keys: выходной вектор с ключами (строки для удобства)
    // showSteps: показывать шаги генерации
    bool generateKeys(std::vector<std::string>& keys, bool showSteps);

    // Шифровать бинарные данные
    // data: входные данные, key: ключ/параметры, nonce: синхропосылка (если нужна)
    // output: выходной шифртекст, showSteps: показывать шаги
    bool encryptBinary(const std::vector<unsigned char>& data,
                      const std::vector<std::string>& keys,
                      const std::vector<unsigned char>& nonce,
                      std::vector<unsigned char>& output,
                      bool showSteps);

    // Дешифровать бинарные данные
    bool decryptBinary(const std::vector<unsigned char>& encryptedData,
                      const std::vector<std::string>& keys,
                      const std::vector<unsigned char>& nonce,
                      std::vector<unsigned char>& output,
                      bool showSteps);

    // Шифровать текст
    bool encryptText(const std::string& text,
                    const std::vector<std::string>& keys,
                    std::string& output,
                    bool showSteps);

    // Дешифровать текст
    bool decryptText(const std::string& encryptedText,
                    const std::vector<std::string>& keys,
                    std::string& output,
                    bool showSteps);

    // Нужен ли nonce?
    bool requiresNonce();

    // Размер nonce (если нужен)
    size_t getNonceSize();
}

#endif // ICRYPTOPLUGIN_H
