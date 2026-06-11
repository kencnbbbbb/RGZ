#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <limits>
#include <clocale>
#include <cstdlib>
#include "mess.h"
#include "Rabin.h"

using namespace std;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void clearScreen() {
    system("clear");
}

bool authenticate() {
    string username, password;
    const string VALID_USERNAME = "Admin";
    const string VALID_PASSWORD = "123";
    int attempts = 0;

    cout << "    АВТОРИЗАЦИЯ \n";

    while (attempts < 3) {
        cout << "Логин: ";
        getline(cin, username);
        cout << "Пароль: ";
        getline(cin, password);

        if (username == VALID_USERNAME && password == VALID_PASSWORD) {
            return true;
        }
        cout << "Ошибка. Попробуйте снова.\n";
        attempts++;
    }
    return false;
}

string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл");
    }
    string content, line;
    while (getline(file, line)) {
        content += line + "\n";
    }
    return content;
}

void writeFile(const string& filename, const string& content) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Не удалось создать файл");
    }
    file << content;
    cout << "Сохранено в: " << filename << endl;
}

void printMainMenu() {
    cout << "\n    КРИПТОГРАФИЧЕСКИЕ ПРОТОКОЛЫ \n";
    cout << "1. Протокол Месси-Омуры\n";
    cout << "2. Криптосистема Рабина\n";
    cout << "3. Выход\n";
    cout << "Ваш выбор: ";
}

string inputMessage() {
    cout << "1. Ввести с клавиатуры\n";
    cout << "2. Загрузить из файла\n";
    cout << "Ваш выбор: ";
    int choice;
    cin >> choice;
    clearInput();

    if (choice == 1) {
        cout << "Введите сообщение: ";
        string message;
        getline(cin, message);
        return message;
    } else if (choice == 2) {
        cout << "Путь к файлу: ";
        string filename;
        getline(cin, filename);
        return readFile(filename);
    }
    return "";
}

void saveResult(const string& result) {
    cout << "Сохранить в файл? (y/n): ";
    char choice;
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        cout << "Имя файла: ";
        string filename;
        cin >> filename;
        writeFile(filename, result);
    } else {
        cout << "Результат:\n" << result << endl;
    }
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    cout << "Программа RaGraRa\n";

    if (!authenticate()) return 1;
    clearScreen();

    int mainChoice;
    do {
        printMainMenu();
        cin >> mainChoice;
        clearInput();

        switch (mainChoice) {
        case 1: {
            int moChoice;
            do {
                cout << "\n    Протокол Месси-Омуры \n";
                cout << "1. Шифрование\n2. Дешифрование\n3. Назад\nВыбор: ";
                cin >> moChoice;
                clearInput();
                if (moChoice == 1) masseyOmuraEncrypt();
                else if (moChoice == 2) masseyOmuraDecrypt();
                else if (moChoice == 3) clearScreen();
                else cout << "Неверный выбор\n";
            } while (moChoice != 3);
            break;
        }
        case 2: {
            int rabinChoice;
            do {
                cout << "\n    Криптосистема Рабина \n";
                cout << "1. Шифрование\n2. Дешифрование\n3. Назад\nВыбор: ";
                cin >> rabinChoice;
                clearInput();
                if (rabinChoice == 1) rabinEncrypt();
                else if (rabinChoice == 2) rabinDecrypt();
                else if (rabinChoice == 3) clearScreen();
                else cout << "Неверный выбор\n";
            } while (rabinChoice != 3);
            break;
        }
        case 3:
            cout << "Выход\n";
            break;
        }
    } while (mainChoice != 3);

    return 0;
}