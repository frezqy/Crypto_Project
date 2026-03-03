#include "scytale.h"
#include "file_io.h" 
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

// скитала: шифрование (основная логика перестановки)
vector<char> scytale_encrypt_impl(const vector<char>& data, int key) {
    if (key <= 0 || data.empty()) return {};
    int cols = ceil((double)data.size() / key);
    vector<vector<char>> matrix(key, vector<char>(cols, 0x00));
    int k = 0;
    // запись в матрицу по столбцам
    for (int j = 0; j < cols; ++j) {
        for (int i = 0; i < key; ++i) {
            if (k < data.size()) matrix[i][j] = data[k++];
        }
    }
    // чтение из матрицы по строкам
    vector<char> cipherData;
    for (int i = 0; i < key; ++i) {
        for (int j = 0; j < cols; ++j) {
            cipherData.push_back(matrix[i][j]);
        }
    }
    return cipherData;
}

// скитала: дешифрование (обратная логика)
vector<char> scytale_decrypt_impl(const vector<char>& cipherData, int key) {
    if (key <= 0 || cipherData.empty()) return {};
    int cols = cipherData.size() / key; 
    vector<vector<char>> matrix(key, vector<char>(cols, 0x00));
    int k = 0;
    // запись шифртекста в матрицу по строкам
    for (int i = 0; i < key; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (k < cipherData.size()) matrix[i][j] = cipherData[k++];
        }
    }
    // чтение исходного текста по столбцам
    vector<char> plainData;
    for (int j = 0; j < cols; ++j) {
        for (int i = 0; i < key; ++i) {
            plainData.push_back(matrix[i][j]);
        }
    }
    // удаляем нулевые байты-заполнители
    while (!plainData.empty() && plainData.back() == 0x00) plainData.pop_back();
    return plainData;
}

// скитала: главная функция обработки файла
bool scytale_file_process(const string& in_file, const string& out_file, const string& password, bool encrypt) {
    // ключ берется как длина пароля
    int key = max(1, (int)password.length());

    // читаем файл
    vector<char> buffer = read_file_to_buffer(in_file);
    if (buffer.empty()) {
        cerr << "ошибка: не удалось прочитать файл." << endl;
        return false;
    }

    vector<char> result;
    if (encrypt) {
        result = scytale_encrypt_impl(buffer, key);
    } else {
        result = scytale_decrypt_impl(buffer, key);
    }

    // пишем результат
    return write_buffer_to_file(out_file, result);
}