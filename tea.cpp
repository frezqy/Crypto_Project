#include "tea.h"
#include <iostream>
#include <fstream>       

using namespace std;

const uint32_t DELTA = 0x9E3779B9; 

// хэширование пароля (для tea)
void hash_password(const string& password, uint32_t key[4]) {
    // инициализация ключа нулями
    key[0] = 0;
    key[1] = 0;
    key[2] = 0;
    key[3] = 0;

    // каждый символ xor'ится с соответствующей частью ключа
    for (size_t i = 0; i < password.length(); ++i) {
        key[i % 4] ^= (uint32_t)password[i];
    }
    
    // добавляем ненулевые константы чтобы ключ не был нулевым
    key[0] |= 0x11111111;
    key[1] |= 0x22222222;
    key[2] |= 0x33333333;
    key[3] |= 0x44444444;
}

// шифрование одного 64-битного блока
void tea_encrypt_block(uint32_t v[2], const uint32_t k[4]) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0; 
    for (int i = 0; i < 32; ++i) {          
        sum += DELTA;                       
        v0 += ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        v1 += ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
    }
    v[0] = v0; v[1] = v1; 
}

// дешифрование одного 64-битного блока
void tea_decrypt_block(uint32_t v[2], const uint32_t k[4]) {
    uint32_t v0 = v[0], v1 = v[1];
    uint32_t sum = DELTA * 32; 
    for (int i = 0; i < 32; ++i) { 
        v1 -= ((v0 << 4) + k[2]) ^ (v0 + sum) ^ ((v0 >> 5) + k[3]);
        v0 -= ((v1 << 4) + k[0]) ^ (v1 + sum) ^ ((v1 >> 5) + k[1]);
        sum -= DELTA; 
    }
    v[0] = v0; v[1] = v1; 
}

// tea: главная функция обработки файла
bool tea_file_process(const string& in_file, const string& out_file, const string& password, bool encrypt) {
    uint32_t key[4];
    hash_password(password, key); 

    // открываем потоки для работы в бинарном режиме
    ifstream fin(in_file, ios::binary);
    ofstream fout(out_file, ios::binary);
    
    // проверка открытия файлов
    if (!fin.is_open() || !fout.is_open()) {
        cerr << "ошибка: не удалось открыть один из файлов." << endl;
        fin.close(); fout.close(); 
        return false;
    }

    char buffer[8]; 
    long long file_size;
    fin.seekg(0, ios::end);
    file_size = fin.tellg();
    fin.seekg(0, ios::beg);

    if (encrypt) {
        // шифрование с padding
        while (fin.read(buffer, 8)) {
            tea_encrypt_block(reinterpret_cast<uint32_t*>(buffer), key); 
            fout.write(buffer, 8);
        }
        
        int bytes_read = fin.gcount(); 
        int padding_len = (bytes_read > 0) ? (8 - bytes_read) : 8; 
        
        for (int i = bytes_read; i < 8; ++i) buffer[i] = static_cast<char>(padding_len);
        
        tea_encrypt_block(reinterpret_cast<uint32_t*>(buffer), key);
        fout.write(buffer, 8); 
        
    } else {
        // дешифрование
        if (file_size == 0 || file_size % 8 != 0) return false; 
        long long blocks_count = file_size / 8;
        
        for (long long i = 0; i < blocks_count; ++i) {
            fin.read(buffer, 8);
            tea_decrypt_block(reinterpret_cast<uint32_t*>(buffer), key); 
            
            if (i < blocks_count - 1) {
                fout.write(buffer, 8);
            } else {
                int padding_len = static_cast<int>(buffer[7]);
                if (padding_len < 1 || padding_len > 8) return false; 
                fout.write(buffer, 8 - padding_len);
            }
        }
    }
    
    fin.close(); 
    fout.close();
    
    if (fout.fail()) return false;
    return true; 
}