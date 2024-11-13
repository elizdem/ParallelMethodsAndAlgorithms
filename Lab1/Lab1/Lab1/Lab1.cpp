#include <iostream>
#include <random>
#include <omp.h>
#include <chrono>
#include <Windows.h>

using namespace std;

double calculate_pi(int num_points) {
    int inside_circle = 0;

    // ���������� OpenMP ��� ������������� �������
#pragma omp parallel for reduction(+:inside_circle)
    for (int i = 0; i < num_points; ++i) {
        // ��������� ��������� ��������� (x, y) � ��������� [-1, 1]
        double x = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
        double y = (rand() / (double)RAND_MAX) * 2.0 - 1.0;

        // ���������, ����� �� ����� ������ ���������� �������� 1
        if (x * x + y * y <= 1.0) {
            inside_circle++;
        }
    }

    // ������ ����� pi
    return 4.0 * inside_circle / num_points;
}

double calculate_pi_single_thread(int num_points) {
    int inside_circle = 0;

    // ������������ ������ ����� pi
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

    int num_points = 10000000;  // ����� ����� ��� ����������
    int num_threads = 4;         // ���������� ������� ��� ������������� ������

    // ������ ������� ��� ������������ ������
    auto start_single = chrono::high_resolution_clock::now();
    double pi_single = calculate_pi_single_thread(num_points);
    auto end_single = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_single = end_single - start_single;

    // ������� ��������� ������������ ������
    cout << "����� �� ��� ������������ ���������� " << pi_single << endl;
    cout << "����� ���������� ��� ������������ ���������� " << duration_single.count() << " seconds" << endl;

    // ������ ������� ��� ������������� ������
    omp_set_num_threads(num_threads);  // ������������� ���������� �������
    auto start_multi = chrono::high_resolution_clock::now();
    double pi_multi = calculate_pi(num_points);
    auto end_multi = chrono::high_resolution_clock::now();
    chrono::duration<double> duration_multi = end_multi - start_multi;

    // ������� ��������� ������������� ������
    cout << "����� �� ��� ������������� ���������� " << pi_multi << endl;
    cout << "����� ���������� ��� ������������� ���������� " << duration_multi.count() << " seconds" << endl;

    // ��������� ���������
    double speedup = duration_single.count() / duration_multi.count();
    cout << "Speedup (with " << num_threads << " threads): " << speedup << endl;

    return 0;
}
