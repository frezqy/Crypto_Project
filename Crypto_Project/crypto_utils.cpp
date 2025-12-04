#include "crypto_utils.h"
#include "file_io.h" 
#include <iostream>
#include <fstream>       
#include <algorithm>
#include <iomanip>       
#include <cmath>         

using namespace std;

// ====================================================================
//                             АЛГОРИТМ TEA (Tiny Encryption Algorithm)
// ====================================================================

const uint32_t DELTA = 0x9E3779B9; // "Магическая" константа, обеспечивающая диффузию

// Шифрование одного 64-битного блока (два 32-битных слова)
void tea_encrypt_block(uint32_t v[2], const uint32_t k[4]) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0; // Инициализация 
    for (int i = 0; i < 32; ++i) {          // Выполняем 32 раунда
        sum += DELTA;                       // Прибавляем константу к сумме
        // Первый раунд (v0)
        v0 += ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        // Второй раунд (v1)
        v1 += ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
    }
    v[0] = v0; v[1] = v1; // Записываем результат обратно
}

// Дешифрование одного 64-битного блока
void tea_decrypt_block(uint32_t v[2], const uint32_t k[4]) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t sum = DELTA * 32; // Начинаем с конечной суммы
    for (int i = 0; i < 32; ++i) { // Выполняем 32 обратных раунда
        // Операции обратны шифрованию (вычитание вместо сложения, обратные ключи)
        v1 -= ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
        v0 -= ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        sum -= DELTA; // Вычитаем константу
    }
    v[0] = v0; v[1] = v1; // Записываем результат обратно
}

// TEA: Главная функция обработки файла
bool tea_file_process(const string& in_file, const string& out_file, 
                      const string& password, bool encrypt) {
    uint32_t key[4];
    hash_password(password, key); // Генерация 128-битного ключа из пароля

    // Открываем потоки для работы в бинарном режиме
    ifstream fin(in_file, ios::binary);
    ofstream fout(out_file, ios::binary);
    
    // !!! ПРОВЕРКА: Если файлы не открылись, немедленно возвращаем ошибку
    if (!fin.is_open() || !fout.is_open()) {
        cerr << "ОШИБКА I/O: Не удалось открыть один из файлов." << endl;
        // Закрытие потоков, даже если они не открыты, безопасно
        fin.close(); fout.close(); 
        return false;
    }

    char buffer[8]; // 64-битный блок для обработки
    
    // Определение размера файла для корректного дешифрования
    long long file_size;
    fin.seekg(0, ios::end);
    file_size = fin.tellg();
    fin.seekg(0, ios::beg);

    if (encrypt) {
        // --- ШИФРОВАНИЕ (с добавлением PKCS#7 Padding) ---
        
        // Читаем файл блоками по 8 байт
        while (fin.read(buffer, 8)) {
            // Шифруем прочитанный блок
            tea_encrypt_block(reinterpret_cast<uint32_t*>(buffer), key); 
            // Записываем зашифрованный блок в выходной файл
            fout.write(buffer, 8);
        }
        
        // Обработка последнего (неполного) блока с добавлением padding
        int bytes_read = fin.gcount(); // Количество байт, прочитанных в последней попытке
        // Если прочитано > 0 байт, padding_len = 8 - bytes_read, иначе padding_len = 8
        int padding_len = (bytes_read > 0) ? (8 - bytes_read) : 8; 
        
        // Заполняем остаток блока значением padding_len
        for (int i = bytes_read; i < 8; ++i) buffer[i] = static_cast<char>(padding_len);
        
        // Шифруем блок с padding
        tea_encrypt_block(reinterpret_cast<uint32_t*>(buffer), key);
        fout.write(buffer, 8); // Записываем последний блок
        
    } else {
        // --- ДЕШИФРОВАНИЕ (с удалением Padding) ---
        if (file_size == 0 || file_size % 8 != 0) return false; // Проверка на целостность
        long long blocks_count = file_size / 8;
        
        for (long long i = 0; i < blocks_count; ++i) {
            fin.read(buffer, 8);
            tea_decrypt_block(reinterpret_cast<uint32_t*>(buffer), key); // Дешифруем блок
            
            if (i < blocks_count - 1) {
                // Все блоки, кроме последнего, записываются полностью
                fout.write(buffer, 8);
            } else {
                // Последний блок: определяем padding по последнему байту
                int padding_len = static_cast<int>(buffer[7]);
                // Проверка на корректность padding
                if (padding_len < 1 || padding_len > 8) return false; 
                // Записываем только полезные данные, отбрасывая padding
                fout.write(buffer, 8 - padding_len);
            }
        }
    }
    
    // !!! ГАРАНТИРУЕМ: Закрываем потоки для сброса данных на диск !!!
    fin.close(); 
    fout.close();
    
    // Проверка, что запись прошла успешно (без ошибок на уровне ОС)
    if (fout.fail()) {
        cerr << "ОШИБКА I/O: Запись выходного файла не удалась." << endl;
        return false;
    }
    
    return true; // Успех
}

// ====================================================================
//                            ШИФР СКИЛАЛЫ (Задание 2)
// ====================================================================
// ... (Оставляем реализацию Скиталы без изменений) ...

// Скитала: Шифрование (основная логика перестановки)
vector<char> scytale_encrypt_impl(const vector<char>& data, int key) {
    if (key <= 0 || data.empty()) return {};
    int cols = ceil((double)data.size() / key);
    vector<vector<char>> matrix(key, vector<char>(cols, 0x00));
    int k = 0;
    // Запись в матрицу ПО СТОЛБЦАМ
    for (int j = 0; j < cols; ++j) {
        for (int i = 0; i < key; ++i) {
            if (k < data.size()) matrix[i][j] = data[k++];
        }
    }
    // Чтение из матрицы ПО СТРОКАМ (формирование шифртекста)
    vector<char> cipherData;
    for (int i = 0; i < key; ++i) {
        for (int j = 0; j < cols; ++j) {
            cipherData.push_back(matrix[i][j]);
        }
    }
    return cipherData;
}

// Скитала: Дешифрование (обратная логика)
vector<char> scytale_decrypt_impl(const vector<char>& cipherData, int key) {
    if (key <= 0 || cipherData.empty()) return {};
    int cols = cipherData.size() / key; 
    vector<vector<char>> matrix(key, vector<char>(cols, 0x00));
    int k = 0;
    // Запись шифртекста в матрицу ПО СТРОКАМ
    for (int i = 0; i < key; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (k < cipherData.size()) matrix[i][j] = cipherData[k++];
        }
    }
    // Чтение исходного текста ПО СТОЛБЦАМ
    vector<char> plainData;
    for (int j = 0; j < cols; ++j) {
        for (int i = 0; i < key; ++i) {
            plainData.push_back(matrix[i][j]);
        }
    }
    // Удаляем нулевые байты-заполнители (padding)
    while (!plainData.empty() && plainData.back() == 0x00) plainData.pop_back();
    return plainData;
}

// Скитала: Главная функция обработки файла
bool scytale_file_process(const string& in_file, const string& out_file, 
                          const string& password, bool encrypt) {
    // Ключ (диаметр скиталы) берется как длина пароля
    int key = max(1, (int)password.length());

    // Читаем весь файл в память (подходит для небольших файлов)
    vector<char> buffer = read_file_to_buffer(in_file);
    if (buffer.empty()) {
        cerr << "Ошибка: Не удалось прочитать файл или он пуст." << endl;
        return false;
    }

    vector<char> result;
    if (encrypt) {
        result = scytale_encrypt_impl(buffer, key);
    } else {
        result = scytale_decrypt_impl(buffer, key);
    }

    // Записываем результат в файл
    return write_buffer_to_file(out_file, result);
}

//ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ

// Хэширование пароля (для TEA)
void hash_password(const string& password, uint32_t key[4]) {
    // Инициализация ключа нулями
    key[0] = 0;
    key[1] = 0;
    key[2] = 0;
    key[3] = 0;

    // Простая схема: Каждый символ XOR'ится с соответствующей частью ключа
    for (size_t i = 0; i < password.length(); ++i) {
        key[i % 4] ^= (uint32_t)password[i];
    }
    
    // Добавляем не-нулевые константы, чтобы ключ не был нулевым
    key[0] |= 0x11111111;
    key[1] |= 0x22222222;
    key[2] |= 0x33333333;
    key[3] |= 0x44444444;
}

// Быстрое модульное возведение в степень (для DH)
long long power(long long base, long long exp, long long mod) {
    long long res = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) res = (res * base) % mod;
        exp = exp >> 1; 
        base = (base * base) % mod;
    }
    return res;
}

// Генерация одной части секретного ключа DH
uint32_t generate_dh_secret_part(long long p, long long g, long long a_secret, long long b_secret) {
    // Алиса: A = (g^a) % p. Общий секрет: S = (A^b) % p
    long long A = power(g, a_secret, p);
    long long secret_S = power(A, b_secret, p);
    // Берем нижние 32 бита общего секрета
    return static_cast<uint32_t>(secret_S & 0xFFFFFFFF);
}

// Демонстрация Диффи-Хеллмана (Задание 1)
void run_diffie_hellman_demonstration() {
    cout << "\n--- Задание 1: Протокол Диффи-Хеллмана ---" << endl;
    cout << "ДЕМОНСТРАЦИЯ: Генерация общего 128-битного ключа." << endl;

    // Фиксированные секреты Алисы и Боба для демонстрации
    long long a_secret = 13, b_secret = 17; 
    
    // 4 набора общих параметров (p, g) для генерации 4 частей 128-битного ключа
    vector<pair<long long, long long>> params = {
        {101, 7}, {103, 11}, {107, 13}, {109, 17}
    };
    uint32_t key[4];
    for (int i = 0; i < 4; ++i) {
        // Генерируем каждую 32-битную часть ключа
        key[i] = generate_dh_secret_part(params[i].first, params[i].second, a_secret, b_secret);
    }

    cout << "  Секретный ключ Алисы (a) = " << a_secret << endl;
    cout << "  Секретный ключ Боба (b) = " << b_secret << endl;
    cout << "  Сгенерированный общий 128-битный ключ: ";
    cout << hex << uppercase << setfill('0');
    for(int i = 0; i < 4; ++i) cout << setw(8) << key[i] << " ";
    cout << dec << endl;
}