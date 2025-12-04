#include <iostream>
#include <string>
#include <fstream>
#include <limits> 
#include "crypto_utils.h" 
#include "file_io.h"      

using namespace std;

// главная функция управления операциями шифрования/дешифрования

void process_file_action(int method, bool encrypt) {
    string inFile, outFile, password;
    string action = encrypt ? "Шифрование" : "Дешифрование";
    
    cout << "\n--- " << action << " файла ---" << endl;
    cout << "Введите имя исходного файла: ";
    cin >> inFile;
    
    // ввод пароля
    cout << "Введите пароль (ключ): ";
    cin >> password;
    
    cout << "Введите имя выходного файла: ";
    cin >> outFile;

    bool success = false;
    
    // выбор метода: 1 - TEA, 2 - Скитала 
    if (method == 1) { 
        success = tea_file_process(inFile, outFile, password, encrypt);
        
    } else if (method == 2) { 
        success = scytale_file_process(inFile, outFile, password, encrypt);
    }
    
    if (success) {
        cout << "\n[УСПЕХ] Файл '" << inFile << "' " << (encrypt ? "зашифрован" : "дешифрован") << " в '" << outFile << "'." << endl;
    } else {
        cerr << "\n[ОШИБКА] Не удалось выполнить операцию. Проверьте имена файлов и пароль." << endl;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");
    int choice = -1;
    
    // создаем тестовый файл
    ofstream test_file("input.txt");
    if (test_file.is_open()) {
        test_file << "Hello World!";
        test_file.close();
        cout << "Создан тестовый файл 'input.txt' для удобства тестирования." << endl;
    }

    while (choice != 0) {
        cout << "\n========================================================" << endl;
        cout << "РГР: Криптографическое ПО (Проект)" << endl;
        cout << "========================================================" << endl;
        cout << "1. Протокол Диффи-Хеллмана (Демонстрация генерации ключа)" << endl; 
        cout << "2. Шифр Перестановки 'Скитала' (Задание 2)" << endl;               
        cout << "3. Блочный шифр TEA (Задание 3)" << endl;                          
        cout << "4. Печать файла на экране " << endl;
        cout << "0. Выход" << endl;
        cout << "Ваш выбор: ";
        
        // обработка ввода и очистка буфера в случае ошибки (например, ввод буквы вместо цифры)
        if (!(cin >> choice)) { 
            cout << "Некорректный ввод. Повторите." << endl;
            cin.clear(); 
            
           // корректное использование numeric_limits
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
            continue;
        }

        if (choice == 1) {
            run_diffie_hellman_demonstration(); 
        } else if (choice == 2 || choice == 3) {
            int action;
            cout << "Выберите операцию для метода " << (choice == 2 ? "Скитала" : "TEA") << ":" << endl;
            cout << "  1. Зашифровать" << endl;
            cout << "  2. Дешифровать" << endl;
            cout << "Ваш выбор: ";
            if (!(cin >> action) || (action != 1 && action != 2)) {
                cout << "Неверный выбор операции." << endl;
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            
            // вызываем главную функцию обработки. Метод 2->Скитала, Метод 3->TEA
            // для функции process_file_action: 1=TEA (3), 2=Скитала (2)

            process_file_action(choice == 3 ? 1 : 2, action == 1); 
        } else if (choice == 4) {
            string fileName;
            cout << "Введите имя файла для печати: ";

            // очищаем буфер после ввода числа, чтобы cin >> fileName работал корректно

            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            cin >> fileName;
            print_file_content(fileName);
        } else if (choice != 0) {
            cout << "Неверный выбор. Повторите." << endl;
        }
    }
    cout << "Программа завершена." << endl;
    return 0;
}