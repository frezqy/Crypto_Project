#pragma once 
#include <vector>  
#include <string>  

// Заголовочный файл для функций ввода/вывода данных из файлов

/**
 * @brief Читает весь бинарный файл в буфер (вектор байтов).
 * @param filename Имя файла, который нужно прочитать.
 * @return Вектор байтов с содержимым файла или пустой вектор в случае ошибки.
 */
std::vector<char> read_file_to_buffer(const std::string& filename);

/**
 * @brief Записывает содержимое буфера в бинарный файл.
 * @param filename Имя выходного файла.
 * @param buffer Вектор байтов для записи.
 * @return true в случае успеха, false в случае ошибки.
 */
bool write_buffer_to_file(const std::string& filename, const std::vector<char>& buffer);

/**
 * @brief Выводит содержимое файла на экран (требование РГР).
 * @param filename Имя файла для печати.
 */
void print_file_content(const std::string& filename);