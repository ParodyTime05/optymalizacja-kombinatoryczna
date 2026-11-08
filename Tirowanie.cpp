#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <time.h>

struct Dane {
    int i_customer_nr;
    int x_coord;
    int y_coord;
    int q_demand;
    int e_ready_time;
    int l_due_date;
    int d_service_time;
};

void generate_genome(int wynik[], int ile) {
    int zero_jeden[] = {0,1};
    int rozmiar_listy = sizeof(zero_jeden) / sizeof(zero_jeden[0]);
    for (int i = 0; i < ile; i++) {
        int randoms = rand() % rozmiar_listy;
        wynik[i] = zero_jeden[randoms];
    }
}

int main(){
    srand(time(NULL));
    const int ile_genome = 10;
    int wynik[ile_genome];

    struct Dane lista_danych[] = {
        {0, 0, 0, 0, 0, 100, 0},
        {1, 0, 10, 10, 10, 300, 10},
        {2, 0, 20, 10, 20, 120, 10},
        {3, 0, 30, 10, 30, 180, 10}
    };

    generate_genome(wynik,ile_genome);
    for (int i = 0; i < ile_genome; i++) {
        printf("%d",wynik[i]);
    }
    printf("\n\n");
    int rozmiar_danych = sizeof(lista_danych) / sizeof(lista_danych[0]);

    for (int i = 0; i < rozmiar_danych; i++) {
        printf("(%d, %d, %d) \n", lista_danych[i].i_customer_nr, lista_danych[i].x_coord, lista_danych[i].y_coord);
    }
    return 0;
}
