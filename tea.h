#pragma once
#include <string>
#include <cstdint>

// преобразует пароль в 128-битный ключ
void hash_password(const std::string& password, uint32_t key[4]);

// шифрует или дешифрует файл алгоритмом tea
bool tea_file_process(const std::string& in_file, const std::string& out_file, const std::string& password, bool encrypt);