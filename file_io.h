#pragma once 
#include <vector>  
#include <string>  



/**
 * @brief читает весь бинарный файл в буфер (вектор байтов).
 * @param filename имя файла, который нужно прочитать.
 * @return вектор байтов с содержимым файла или пустой вектор в случае ошибки.
 */
std::vector<char> read_file_to_buffer(const std::string& filename);

/**
 * @brief записывает содержимое буфера в бинарный файл.
 * @param filename имя выходного файла.
 * @param buffer вектор байтов для записи.
 * @return true в случае успеха, false в случае ошибки.
 */
bool write_buffer_to_file(const std::string& filename, const std::vector<char>& buffer);

/**
 * @brief выводит содержимое файла на экран (требование РГР).
 * @param filename имя файла для печати.
 */
void print_file_content(const std::string& filename);