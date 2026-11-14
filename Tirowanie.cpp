#include <stdio.h>
#include <cstdlib>
#include <time.h>
#include <cstdio>
#include <cmath>


struct Dane {
    int i_customer_nr;
    int x_coord;
    int y_coord;
    int q_demand;
    int e_ready_time;
    float l_due_date;
    int d_service_time;
};

struct Pozycja {
    int x_pos;
    int y_pos;
};

// Sekcja z Zadaniem

//Droga do kolejnego
float RouteMath(float d, struct Pozycja nasz, struct Pozycja kolej) {
    float x_suma = (nasz.x_pos - kolej.x_pos)*(nasz.x_pos - kolej.x_pos);
    float y_suma = (nasz.y_pos - kolej.y_pos)*(nasz.y_pos - kolej.y_pos);
    d += sqrt(x_suma + y_suma);
    return d;
}

// Sprawdzanie czy jest miejsce
bool Decider(const int miejsce_stale, int &miejsce_tir, int miejsce_pkt) {
    if (miejsce_tir < miejsce_pkt) {
        miejsce_tir = miejsce_stale;
        return true;
    }
    miejsce_tir -= miejsce_pkt;
    return false;
}

//Sprawdzanie punktu
float PointChecker(float droga, float otw2, int zam2, int czk2) {
    // Sprawdzanie zamknięcia
    if (droga>zam2) {
        droga = -1;
        return droga;
    }

    // Sprawdzanie otwarcia
    if (droga<otw2) {
        otw2 -= droga;
        droga += otw2;
    }
    // Dodanie czekania
    droga += czk2;

    return droga;
}

// Sekcja z Genetycznym algorytmem

//generowanie genów
void GenerateGenome(int wynik[], int ilosc_punktow) {
    for (int i = 0; i < ilosc_punktow; i++) {
        wynik[i] = i+1;
    }

    for (int i = 0; i < ilosc_punktow; i++) {
        int grand = rand() % ilosc_punktow;
        int temp = wynik[i];
        wynik[i] = wynik[grand];
        wynik[grand] = temp;
    }
}

float Fitness(int gen[], int ilosc_punktow, struct Dane lista_danych[], const int miejsce_w_tirze) {
    int zmienne_miejsce_w_tirze = miejsce_w_tirze;
    struct Pozycja baza_pozycja{lista_danych[0].x_coord, lista_danych[0].y_coord};
    struct Pozycja nasza_pozycja = baza_pozycja;

    float trasa = 0.0;
    float cala_trasa = 0.0;

    for (int i = 0; i < ilosc_punktow; i++) {
        int wskazywacz = gen[i];
        struct Pozycja kolejna_pozycja{lista_danych[wskazywacz].x_coord, lista_danych[wskazywacz].y_coord};
        float godzina_o = lista_danych[wskazywacz].e_ready_time;
        int godzina_z = lista_danych[wskazywacz].l_due_date;
        int ile_czekam = lista_danych[wskazywacz].d_service_time;
        int ile_towaru = lista_danych[wskazywacz].q_demand;
        bool jedzie_do_bazy = Decider(miejsce_w_tirze, zmienne_miejsce_w_tirze, ile_towaru);

        if (jedzie_do_bazy == false){
            trasa = RouteMath(trasa, nasza_pozycja, kolejna_pozycja);
            nasza_pozycja = kolejna_pozycja;
            trasa = PointChecker(trasa, godzina_o, godzina_z, ile_czekam);
        }
        else {
            i-=1;
            trasa = RouteMath(trasa, nasza_pozycja, baza_pozycja);
            cala_trasa += trasa;
            trasa = 0;
            nasza_pozycja = baza_pozycja;
        }

        if (i==ilosc_punktow-1) {
            trasa = RouteMath(trasa, nasza_pozycja, baza_pozycja);
            cala_trasa += trasa;
        }
    }
    return cala_trasa;
}

void PickPair() {

}

void CutPair() {

}

void Mutation() {

}


int main() {
    srand(time(NULL));

    const int miejsce_w_tirze = 20;

    struct Dane lista_danych[] = {
        {0, 0, 0, 0, 0, 100, 0},
        {1, 0, 10, 10, 10, 300, 10},
        {2, 0, 20, 10, 20, 120, 10},
        {3, 0, 30, 10, 30, 180, 10},
        {4, 0, 40, 10, 40, 400, 10}
    };

    int rozmiar_danych = sizeof(lista_danych) / sizeof(lista_danych[0])-1;
    int genom[rozmiar_danych];

    GenerateGenome(genom, rozmiar_danych);

    for (int i = 0; i < rozmiar_danych; i++) {
        printf("%d",genom[i]);
    }
    printf("\n");

    float wynik_f = Fitness(genom, rozmiar_danych, lista_danych, miejsce_w_tirze);

    printf("Koszt genomu powyzej to: %f", wynik_f);
    return 0;
}
