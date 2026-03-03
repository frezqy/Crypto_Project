#pragma once
#include <string>

// шифрует или дешифрует файл шифром скиталы
bool scytale_file_process(const std::string& in_file, const std::string& out_file, const std::string& password, bool encrypt);