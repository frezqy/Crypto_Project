#pragma once
#include <vector>  
#include <string>  
#include <cstdint> 

// Заголовочный файл для всех криптографических функций проекта

// --- Вспомогательные функции ---

/**
 * @brief Преобразует пароль пользователя в 128-битный ключ (для TEA).
 * @param password Пароль, введенный пользователем.
 * @param key Массив из четырех 32-битных чисел для хранения ключа.
 */
void hash_password(const std::string& password, uint32_t key[4]);

/**
 * @brief Демонстрация протокола Диффи-Хеллмана (Задание 1).
 * Выводит на экран процесс генерации общего секретного ключа.
 */
void run_diffie_hellman_demonstration(); 

// --- Методы шифрования (Задания 2 и 3) ---

/**
 * @brief Шифрует или дешифрует файл с помощью алгоритма TEA.
 * Блочный шифр, подходит для любых бинарных данных (видео, аудио, текст).
 * @param in_file Имя входного файла.
 * @param out_file Имя выходного файла.
 * @param password Пароль, который будет преобразован в 128-битный ключ.
 * @param encrypt Если true - шифрование, false - дешифрование.
 * @return true в случае успеха.
 */
bool tea_file_process(const std::string& in_file, const std::string& out_file, 
                      const std::string& password, bool encrypt);

/**
 * @brief Шифрует или дешифрует файл с помощью шифра Скиталы (перестановка).
 * Подходит для текстовых файлов.
 * @param in_file Имя входного файла.
 * @param out_file Имя выходного файла.
 * @param password Пароль (его длина используется как ключ - диаметр скиталы).
 * @param encrypt Если true - шифрование, false - дешифрование.
 * @return true в случае успеха.
 */
bool scytale_file_process(const std::string& in_file, const std::string& out_file, 
                          const std::string& password, bool encrypt);