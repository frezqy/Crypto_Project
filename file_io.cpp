#include "file_io.h"     
#include <fstream>      
#include <iostream>     
#include <algorithm>    

using namespace std;

// реализация функций для работы с файлами 

vector<char> read_file_to_buffer(const string& filename) {
    // открываем файл в бинарном режиме, устанавливая курсор в конец (ios::ate)
    ifstream fin(filename, ios::binary | ios::ate);
    
    // проверка успешного открытия файла
    if (!fin.is_open()) {
        cerr << "Ошибка: Не удалось открыть файл для чтения: " << filename << endl;
        return {}; // возвращаем пустой вектор
    }
    
    // получаем размер файла, используя текущую позицию курсора (конец файла)
    streamsize size = fin.tellg(); 
    fin.seekg(0, ios::beg); // возвращаем курсор в начало для чтения

    vector<char> buffer(size); // создаем буфер нужного размера
    
    // читаем весь файл одним куском в буфер
    fin.read(buffer.data(), size);
    
    // закрываем файл
    fin.close();
    
    return buffer;
}

bool write_buffer_to_file(const string& filename, const vector<char>& buffer) {
    // открываем файл для записи в бинарном режиме
    ofstream fout(filename, ios::binary);
    
    // проверка успешного открытия файла
    if (!fout.is_open()) {
        cerr << "Ошибка: Не удалось открыть файл для записи: " << filename << endl;
        return false;
    }
    
    // записываем все байты из буфера в файл
    fout.write(buffer.data(), buffer.size());
    
    // закрываем файл, чтобы гарантировать запись данных на диск
    fout.close();
    
    // проверяем, произошла ли ошибка при записи
    if (fout.fail()) {
        cerr << "Ошибка: Запись данных в файл '" << filename << "' не удалась." << endl;
        return false;
    }
    
    return true;
}

void print_file_content(const string& filename) {
    vector<char> buffer = read_file_to_buffer(filename);
    
    if (buffer.empty()) {
        cout << "Файл '" << filename << "' не найден или пуст." << endl;
        return;
    }
    
    cout << "--- Содержимое файла ---" << endl;
    
    // Выводим весь буфер напрямую разом, терминал сам склеит UTF-8
    cout.write(buffer.data(), buffer.size());
    
    cout << "\n--------------------------" << endl;
}