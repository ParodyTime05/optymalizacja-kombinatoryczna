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
    int l_due_date;
    int d_service_time;
};

struct Pozycja {
    int x_pos;
    int y_pos;
};

void GenerateGenome(int wynik[], int ilosc_punktow) {
    for (int i = 0; i < ilosc_punktow; i++) {
        wynik[i] = i+1;
    }

    for (int i = 0; i < ilosc_punktow; i++) {
        int grand = rand() % ilosc_punktow;
        int temp = wynik[0];
        wynik[0] = wynik[grand];
        wynik[grand] = temp;
    }
}

void Fitness() {

}

void PickPair() {

}

void CutPair() {

}

void Mutation() {

}


float RouteMath(float d, struct Pozycja nasz, struct Pozycja kolej, bool b) {
    float x_suma = (nasz.x_pos - kolej.x_pos)*(nasz.x_pos - kolej.x_pos);
    float y_suma = (nasz.y_pos - kolej.y_pos)*(nasz.y_pos - kolej.y_pos);
    d += sqrt(x_suma + y_suma);
    return d;
}

float GoToBase(float droga, struct Pozycja nasz, struct Pozycja baza, bool jedzie_do_bazy) {
    printf("Jade do bazy\n");
    jedzie_do_bazy = true;
    return RouteMath(droga, nasz, baza, jedzie_do_bazy);
}

float GoToNext(float droga, struct Pozycja nasz, struct Pozycja kolej, bool jedzie_do_bazy) {
    printf("Jade do nastepnego\n");
    jedzie_do_bazy = false;
    return RouteMath(droga, nasz, kolej, jedzie_do_bazy);
}

float Decider(float droga, const int miejsce_stale, int miejsce_tir, int miejsce_pkt, struct Pozycja nasz, struct Pozycja kolej, struct Pozycja baza, bool jedzie_do_bazy) {
    // Sprawdzanie czy jest miejsce
    if (miejsce_tir < miejsce_pkt) {
        miejsce_tir = miejsce_stale;
        return GoToBase(droga, nasz, baza, jedzie_do_bazy);
    }
    miejsce_tir -= miejsce_pkt;
    return GoToNext(droga, nasz, kolej, jedzie_do_bazy);
}

// Sekcja z Zadaniem
float RouteChecker(int droga, int otw2, int zam2, int czk2) {
    // Sprawdzanie zamknięcia
    if (droga>=zam2) {
        printf("Zamknięte :( \n");
        droga = -1;
        return droga;
    }

    // Sprawdzanie otwarcia
    if (droga<otw2) {
        printf("Czekam na otwarcie\n");
        otw2 -= droga;
        droga += otw2;
    }

    // Dodanie czekania
    printf("Czekam \n");
    droga += czk2;

    return droga;
}



int main() {
    srand(time(NULL));
    const int miejsce_w_tirze = 20;
    int zmienne_miejsce_w_tirze = 20;

    struct Dane lista_danych[] = {
        {0, 0, 0, 0, 0, 100, 0},
        {1, 0, 10, 10, 10, 300, 10},
        {2, 0, 20, 10, 20, 120, 10},
        {3, 0, 30, 10, 30, 180, 10}
    };

    struct Pozycja baza_pozycja{lista_danych[0].x_coord, lista_danych[0].y_coord};
    struct Pozycja nasza_pozycja = baza_pozycja;
    int rozmiar_danych = sizeof(lista_danych) / sizeof(lista_danych[0]);
    int wynik[rozmiar_danych-1];

    float trasa = 0;
    bool jedzie_do_bazy = false;

    GenerateGenome(wynik, rozmiar_danych-1);
    for (int i = 0; i < rozmiar_danych-1; i++) {
        printf("%d",wynik[i]);
    }
    printf("\n");
    for (int i = 0; i < rozmiar_danych-1; i++) {
        int wskazywacz = wynik[i];
        struct Pozycja kolejna_pozycja{lista_danych[wskazywacz].x_coord, lista_danych[wskazywacz].y_coord};

        int godzina_o = lista_danych[i+1].e_ready_time;
        int godzina_z = lista_danych[i+1].l_due_date;
        int ile_czekam = lista_danych[i+1].d_service_time;

        int ile_towaru = lista_danych[i+1].q_demand;

        printf("Mam %d towaru zabrali %d \n", zmienne_miejsce_w_tirze, ile_towaru );

        trasa = Decider(trasa, miejsce_w_tirze, zmienne_miejsce_w_tirze, ile_towaru, nasza_pozycja, kolejna_pozycja, baza_pozycja, jedzie_do_bazy);
        printf("%f\n", trasa);
        if (jedzie_do_bazy == false){
            nasza_pozycja = kolejna_pozycja;
            trasa = RouteChecker(trasa, godzina_o, godzina_z, ile_czekam);
        }
        else {
            nasza_pozycja = kolejna_pozycja;
        }
        printf("%f", trasa);
        printf("\n\n");
        if (i==rozmiar_danych-2) {
            trasa = GoToBase(trasa, nasza_pozycja, baza_pozycja, jedzie_do_bazy);
            printf("%f", trasa);
        }
    }
}
