#include "dh.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

// быстрое модульное возведение в степень (для dh)
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

// генерация одной части секретного ключа dh
uint32_t generate_dh_secret_part(long long p, long long g, long long a_secret, long long b_secret) {
    // алиса: a = (g^a) % p. общий секрет: s = (a^b) % p
    long long A = power(g, a_secret, p);
    long long secret_S = power(A, b_secret, p);
    // берем нижние 32 бита общего секрета
    return static_cast<uint32_t>(secret_S & 0xFFFFFFFF);
}

// демонстрация диффи-хеллмана
void run_diffie_hellman_demonstration() {
    cout << "\n Задание 1: Протокол Диффи-Хеллмана" << endl;
    cout << "Демонстрация: Генерация общего 128-битного ключа." << endl;

    // фиксированные секреты алисы и боба для демонстрации
    long long a_secret = 13, b_secret = 17; 
    
    // 4 набора общих параметров (p, g) для генерации 4 частей 128-битного ключа
    vector<pair<long long, long long>> params = {
        {101, 7}, {103, 11}, {107, 13}, {109, 17}
    };
    uint32_t key[4];
    for (int i = 0; i < 4; ++i) {
        // генерируем каждую 32-битную часть ключа
        key[i] = generate_dh_secret_part(params[i].first, params[i].second, a_secret, b_secret);
    }

    cout << "  Секретный ключ Алисы (a) = " << a_secret << endl;
    cout << "  Секретный ключ Боба (b) = " << b_secret << endl;
    cout << "  Сгенерированный общий 128-битный ключ: ";
    cout << hex << uppercase << setfill('0');
    for(int i = 0; i < 4; ++i) cout << setw(8) << key[i] << " ";
    cout << dec << endl;
}