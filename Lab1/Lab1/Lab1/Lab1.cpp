#include <iostream>
#include <random>
#include <omp.h>
#include <chrono>
#include <Windows.h>

using namespace std;

double calculate_pi(int num_points) {
    int inside_circle = 0;

    // Используем OpenMP для параллельного расчета
#pragma omp parallel for reduction(+:inside_circle)
    for (int i = 0; i < num_points; ++i) {
        // Генерация случайных координат (x, y) в диапазоне [-1, 1]
        double x = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
        double y = (rand() / (double)RAND_MAX) * 2.0 - 1.0;

        // Проверяем, лежит ли точка внутри окружности радиусом 1
        if (x * x + y * y <= 1.0) {
            inside_circle++;
        }
    }

    // Расчет числа pi
    return 4.0 * inside_circle / num_points;
}

double calculate_pi_single_thread(int num_points) {
    int inside_circle = 0;

    // Однопоточный расчет числа pi
    for (int i = 0; i < num_points; ++i) {
        double x = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
        double y = (rand() / (double)RAND_MAX) * 2.0 - 1.0;

        if (x * x + y * y <= 1.0) {
            inside_circle++;
        }
    }

    return 4.0 * inside_circle / num_points;
}

int main() {

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int num_points = 10000000;  // Число точек для вычислений
    int num_threads = 4;         // Количество потоков для многопоточной версии

    // Замеры времени для однопоточной версии
    auto start_single = chrono::high_resolution_clock::now();
    double pi_single = calculate_pi_single_thread(num_points);
    auto end_single = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_single = end_single - start_single;

    // Выводим результат однопоточной версии
    cout << "Число Пи при однопоточной реализации " << pi_single << endl;
    cout << "Время выполнения при однопоточной реализации " << duration_single.count() << " seconds" << endl;

    // Замеры времени для многопоточной версии
    omp_set_num_threads(num_threads);  // Устанавливаем количество потоков
    auto start_multi = chrono::high_resolution_clock::now();
    double pi_multi = calculate_pi(num_points);
    auto end_multi = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_multi = end_multi - start_multi;

    // Выводим результат многопоточной версии
    cout << "Число Пи при многопоточной реализации " << pi_multi << endl;
    cout << "Время выполнения при многопоточной реализации " << duration_multi.count() << " seconds" << endl;

    // Вычисляем ускорение
    double speedup = duration_single.count() / duration_multi.count();
    cout << "Speedup (with " << num_threads << " threads): " << speedup << endl;

    return 0;
}
