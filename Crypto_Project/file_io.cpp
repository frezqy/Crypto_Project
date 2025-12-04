#include "file_io.h"     
#include <fstream>      
#include <iostream>     
#include <algorithm>    

using namespace std;

// --- Реализация функций для работы с файлами ---

vector<char> read_file_to_buffer(const string& filename) {
    // Открываем файл в бинарном режиме, устанавливая курсор в конец (ios::ate)
    ifstream fin(filename, ios::binary | ios::ate);
    
    // Проверка успешного открытия файла
    if (!fin.is_open()) {
        cerr << "Ошибка: Не удалось открыть файл для чтения: " << filename << endl;
        return {}; // Возвращаем пустой вектор
    }
    
    // Получаем размер файла, используя текущую позицию курсора (конец файла)
    streamsize size = fin.tellg(); 
    fin.seekg(0, ios::beg); // Возвращаем курсор в начало для чтения

    vector<char> buffer(size); // Создаем буфер нужного размера
    
    // Читаем весь файл одним куском в буфер
    fin.read(buffer.data(), size);
    
    // Закрываем файл
    fin.close();
    
    return buffer;
}

bool write_buffer_to_file(const string& filename, const vector<char>& buffer) {
    // Открываем файл для записи в бинарном режиме
    ofstream fout(filename, ios::binary);
    
    // Проверка успешного открытия файла
    if (!fout.is_open()) {
        cerr << "Ошибка: Не удалось открыть файл для записи: " << filename << endl;
        return false;
    }
    
    // Записываем все байты из буфера в файл
    fout.write(buffer.data(), buffer.size());
    
    // Закрываем файл, чтобы гарантировать запись данных на диск
    fout.close();
    
    // Проверяем, произошла ли ошибка при записи
    if (fout.fail()) {
        cerr << "Ошибка: Запись данных в файл '" << filename << "' не удалась." << endl;
        return false;
    }
    
    return true;
}

void print_file_content(const string& filename) {
    // Читаем весь файл в буфер
    vector<char> buffer = read_file_to_buffer(filename);
    
    // Проверка, что файл не пуст
    if (buffer.empty()) {
        cout << "Файл '" << filename << "' не найден или пуст." << endl;
        return;
    }
    
    cout << "--- Содержимое файла ---" << endl;
    for (char c : buffer) {
        // Выводим только печатаемые символы (ASCII 32 и выше)
        if (c >= 32 || c == '\n' || c == '\t') { 
            cout << c;
        } else {
            // Бинарные символы (как в видео, аудио) заменяем точкой для наглядности
            cout << "."; 
        }
    }
    cout << "\n--------------------------" << endl;
}