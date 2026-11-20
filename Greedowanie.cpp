#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <string>

using namespace std;

//Dane dla naszego zadania
struct Dane {
    int i_customer_nr;
    double x_coordinate;
    double y_coordinate;
    int q_demand;
    double e_ready_time;
    double l_due_date;
    double d_service_time;
};

struct Pozycja {
    double x_pos;
    double y_pos;
};

double RouteMath(double d, Pozycja nasz, Pozycja kolej) {
    double x_suma = (nasz.x_pos - kolej.x_pos)*(nasz.x_pos - kolej.x_pos);
    double y_suma = (nasz.y_pos - kolej.y_pos)*(nasz.y_pos - kolej.y_pos);
    d = sqrt(x_suma + y_suma);
    return d;
}


int main() {
    printf("Why so greedy?\n");
    struct Dane lista_danych[] = {
        {0, 0, 0, 0, 0, 300, 0},
        {1, 0, 10, 10, 10, 10, 10},
        {2, 0, 20, 10, 20, 120, 10},
        {3, 0, 30, 10, 30, 180, 10},
        {4, 0, 40, 10, 40, 400, 10}
    };
    int ilosc_punkow = (sizeof(lista_danych) / sizeof(lista_danych[0]))-1;
    struct Pozycja  baza_pozycja ={lista_danych[0].x_coordinate,lista_danych[0].y_coordinate};
    double maks_chciwosci[ilosc_punkow];


    for (int i = 1; i <= ilosc_punkow; i++) {
        struct Pozycja  kolejna_pozycja ={lista_danych[i].x_coordinate,lista_danych[i].y_coordinate};
        double droga = 0.0;
        droga = RouteMath(droga, baza_pozycja, kolejna_pozycja);
        maks_chciwosci[i-1] = droga;
    }
    printf("\n%f",*min_element(maks_chciwosci, maks_chciwosci+ilosc_punkow));
}
