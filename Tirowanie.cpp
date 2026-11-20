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

//Wybniki trasy
struct WynikiTrasy {
    bool dopuszczalne;
    int liczba_tras;
    double koszt;
};

//Pozycje punktów
struct Pozycja {
    double x_pos;
    double y_pos;
};

vector<Dane> WczytajPlik(const string& nazwa_pliku, int& pojemnosc_tira) {
    ifstream plik(nazwa_pliku);
    if (!plik.is_open()) {
        cout << "BLAD: Nie udalo sie otworzyc pliku " << nazwa_pliku << endl;
        exit(1);
    }

    string slowo;
    int liczba_ciezarowek_K = 0;

    while (plik >> slowo) {
        if (slowo == "CAPACITY") {
            plik >> liczba_ciezarowek_K >> pojemnosc_tira; //Wczytywanie K i Q
            break;
        }
    }

    while (plik >> slowo) {
        if (slowo == "TIME") {
            getline(plik, slowo); //ZZjada resztę tej linii
            break;
        }
    }

    //Operator >> automatycznie ignoruje puste linie i białe znaki
    vector<Dane> wczytane_dane;
    Dane temp;
    while (plik >> temp.i_customer_nr
                >> temp.x_coordinate
                >> temp.y_coordinate
                >> temp.q_demand
                >> temp.e_ready_time
                >> temp.l_due_date
                >> temp.d_service_time) {
        wczytane_dane.push_back(temp);
    }

    plik.close();
    return wczytane_dane;
}


// Sekcja z Zadaniem

bool BetterChecker(const WynikiTrasy &a, const WynikiTrasy &b) {
    if (a.dopuszczalne != b.dopuszczalne) {
        return a.dopuszczalne > b.dopuszczalne;
    }
    if (a.dopuszczalne == false && b.dopuszczalne == false) {
        return false;
    }
    if (a.liczba_tras != b.liczba_tras) {
        return a.liczba_tras < b.liczba_tras;
    }
    return a.koszt < b.koszt;
}

//Droga do kolejnego
double RouteMath(double d, struct Pozycja nasz, struct Pozycja kolej) {
    double x_suma = (nasz.x_pos - kolej.x_pos)*(nasz.x_pos - kolej.x_pos);
    double y_suma = (nasz.y_pos - kolej.y_pos)*(nasz.y_pos - kolej.y_pos);
    d += sqrt(x_suma + y_suma);
    return d;
}

//Sprawdzanie czy jest miejsce
bool Decider(const int miejsce_stale, int &miejsce_tir, int miejsce_pkt) {
    if (miejsce_tir < miejsce_pkt) {
        miejsce_tir = miejsce_stale;
        return true;
    }
    miejsce_tir -= miejsce_pkt;
    return false;
}

//Sprawdzanie punktu
double PointChecker(double droga, double otw2, double zam2, double czk2) {
    //Sprawdzanie zamknięcia
    if (droga>zam2) {
        droga = -1;
        return droga;
    }

    //Sprawdzanie otwarcia
    if (droga<otw2) {
        droga = otw2;
    }
    //Dodanie czekania
    droga += czk2;

    return droga;
}

//Sekcja z Genetycznym algorytmem

//Generowanie genów
void GenerateGenome(vector<int>& wynik, int ilosc_punktow) {
    //Wypełnianie listy numerami od 1 do ilosci punktow
    for (int i = 0; i < ilosc_punktow; i++) {
        wynik[i] = i+1;
    }

    //Mieszanie listy pokolei
    for (int i = 0; i < ilosc_punktow; i++) {
        int grand = rand() % ilosc_punktow;
        swap(wynik[i],wynik[grand]);
    }
}

//Generowanie Populacji czyli listi o danej wielkosci, pełnej list genomów
void GeneratePopulation(vector<vector<int>>& populacja, int wielkosc_populacji, int ile_punktow) {
    for (int i = 0; i < wielkosc_populacji; i++) {
        GenerateGenome(populacja[i], ile_punktow);
    }
}

//To są warunki naszego zadania
WynikiTrasy Fitness(const vector<int>& gen, int ilosc_punktow, const vector<Dane>& lista_danych, const int miejsce_w_tirze) {
    struct Pozycja baza_pozycja{lista_danych[0].x_coordinate, lista_danych[0].y_coordinate};
    struct Pozycja nasza_pozycja = baza_pozycja;

    //Trasa jednego z tirów
    double pojedyncza_trasa = 0.0;
    //Trasa całkowita tirów
    double cala_trasa = 0.0;
    int liczba_tras = 1;
    int zmienne_miejsce_w_tirze = miejsce_w_tirze;

    for (int i = 0; i < ilosc_punktow; i++) {
        //Wybieranie punktu z naszego genomu i branie z tego punktu danych
        int wskazywacz = gen[i];
        int ile_towaru = lista_danych[wskazywacz].q_demand;
        if (ile_towaru > miejsce_w_tirze) {
            return {false, 999, 999999999.0};
        }

        struct Pozycja kolejna_pozycja{lista_danych[wskazywacz].x_coordinate, lista_danych[wskazywacz].y_coordinate};
        double godzina_o = lista_danych[wskazywacz].e_ready_time;
        double godzina_z = lista_danych[wskazywacz].l_due_date;
        double ile_czekam = lista_danych[wskazywacz].d_service_time;

        bool jedzie_do_bazy = Decider(miejsce_w_tirze, zmienne_miejsce_w_tirze, ile_towaru);

        if (jedzie_do_bazy == false) {
            pojedyncza_trasa = RouteMath(pojedyncza_trasa, nasza_pozycja, kolejna_pozycja);
            nasza_pozycja = kolejna_pozycja;
            pojedyncza_trasa = PointChecker(pojedyncza_trasa, godzina_o, godzina_z, ile_czekam);
            if (pojedyncza_trasa == -1) {
                return {false, 999, 999999999.0};
            }
        }
        else {
            //Dodanie 1 kroku, ponieważ tir jedzie do bazy
            i-=1;
            pojedyncza_trasa = RouteMath(pojedyncza_trasa, nasza_pozycja, baza_pozycja);
            cala_trasa += pojedyncza_trasa;
            pojedyncza_trasa = 0;
            nasza_pozycja = baza_pozycja;
            liczba_tras++;
        }

        //Powrót do bazy
        if (i == ilosc_punktow-1) {
            pojedyncza_trasa = RouteMath(pojedyncza_trasa, nasza_pozycja, baza_pozycja);
            if (pojedyncza_trasa > lista_danych[0].l_due_date) {
                return {false, 999, 999999999.0};
            }

            cala_trasa += pojedyncza_trasa;
        }
    }
    return  {true, liczba_tras, cala_trasa};
}

//Wybieranie rodzica do łączenia w parę
int PickPair(const vector<vector<int>>& populacja, int ilosc_populacji, int ilosc_punktow,  int miejsce_w_tirze, const vector<Dane>& lista_danych, const int ile_kandydatow) {
    //Miesznie jak w genomach
    vector<int> wybrani(ilosc_populacji);
    for (int i = 0; i < ilosc_populacji; i++) {
        wybrani[i] = i;
    }

    for (int i = ilosc_populacji-1; i > 0; i--) {
        int j = rand() % (i+1);
        swap(wybrani[i], wybrani[j]);
    }

    //Założenie, że 0 to najlepszy kandydat
    int najlepszy_kandydat = wybrani[0];
    WynikiTrasy najlepszy_wynik = Fitness(populacja[najlepszy_kandydat], ilosc_punktow, lista_danych, miejsce_w_tirze);

    //Sprawdzanie pokolei 4 z losowych punktów
    for (int i = 1; i < ile_kandydatow; i++) {
        int nasz_kandydat = wybrani[i];
        WynikiTrasy wynik_kandydat = Fitness(populacja[nasz_kandydat], ilosc_punktow, lista_danych, miejsce_w_tirze);

        //Ustawianie nowego najlepszgo wyniku
        if (BetterChecker(wynik_kandydat, najlepszy_wynik) == true) {
            najlepszy_kandydat = nasz_kandydat;
            najlepszy_wynik = wynik_kandydat;
        }
    }
    return najlepszy_kandydat;
}

//Dzielenie par
void CutPair(const vector<int>& rodzic1, const vector<int>& rodzic2, vector<int>& potomek, int ilosc_punktow) {
    //Wybieranie losowego miejsca do dzielenia
    int punkt_dzielenia = rand() % ilosc_punktow;

    //Zapełnianie listy potomka wartościami z pierwszego rodzica
    for (int i = 0; i <= punkt_dzielenia; i++) {
        potomek[i] = rodzic1[i];
    }
    int index_potomka = punkt_dzielenia+1;

    //Wypełnianie wartości od drugiego rodzica
    for (int i = 0; i < ilosc_punktow; i++) {
        int gen = rodzic2[i];
        bool sprawdz = false;
        //Sprawdzanie czy wartości się powtarzają
        for (int j = 0; j <= punkt_dzielenia; j++) {
            if (potomek[j] == gen) {
                sprawdz = true;
                break;
            }
        }
        //Jeśli się powtarzają omijamy
        if (sprawdz == false && index_potomka < ilosc_punktow) {
            potomek[index_potomka] = gen;
            index_potomka++;
        }
    }
}

//Mutacja wyniku
void Mutation(vector<int>& gen, int ilosc_punktow, double szansa_mutacji ) {
    //Losowa liczba z przedziału od 0 do 1
    double los = (double)rand() / (double)RAND_MAX;

    //Podobnie jak w genome tylko są 2 losowe liczby
    if (los < szansa_mutacji) {
        int index1 = rand() % ilosc_punktow;
        int index2 = rand() % ilosc_punktow;

        swap(gen[index1], gen[index2]);

    }
}


int main() {
    //Zaczynanie liczenia czasu
    auto start_czasu = chrono::high_resolution_clock::now();
    srand(time(NULL));

    //Wartości podane
    string nazwa_pliku = "m2kvrptw-0.txt";
    int miejsce_w_tirze = 0;
    vector<Dane> lista_danych = WczytajPlik(nazwa_pliku, miejsce_w_tirze);

    //Ilość punktów które trzeba odwiedzić (pomijamy punkt 0, czyli magazyn)
    int rozmiar_danych = lista_danych.size() - 1; //Dynamiczny rozmiar z wektora

    //Można je zmieniać
    const int ilosc_populacji = 30;
    const int liczba_pokolen = 500;
    const double szansa_mutacji = 0.5;
    const int licznik_braku_zmian = 5;
    const int ile_kandydatow = 4;

    int zmienny_licznik_braku_zmian = 0;
    vector<vector<int>> populacja(ilosc_populacji, vector<int>(rozmiar_danych));
    vector<vector<int>> nowa_populacja(ilosc_populacji, vector<int>(rozmiar_danych));
    vector<int> najlepszy_gen(rozmiar_danych);

    WynikiTrasy najlepszy_wynik_calkowity = {false, 999, 999999999.0f};


    GeneratePopulation(populacja, ilosc_populacji, rozmiar_danych);

    //Główna pętla
    for (int pokolenie = 0; pokolenie < liczba_pokolen; pokolenie++) {
        int index_najlepszego = 0;
        WynikiTrasy najlepszy_w_pokoleniu = {false, 999, 999999999.0f};
        bool czy_poprawiono = false;

        //Sprawdzanie czy wynik jest mniejszy od poprzedniego
        for (int i = 0; i < ilosc_populacji; i++) {
            WynikiTrasy wynik = Fitness(populacja[i], rozmiar_danych, lista_danych, miejsce_w_tirze);

            if (BetterChecker(wynik, najlepszy_w_pokoleniu) == true) {
                najlepszy_w_pokoleniu = wynik;
                index_najlepszego = i;
            }
            if (BetterChecker(wynik, najlepszy_wynik_calkowity) == true) {
                najlepszy_wynik_calkowity = wynik;
                czy_poprawiono = true;
                najlepszy_gen = populacja[i]; //Wektory można kopiować jednym przypisaniem zamiast pętli
            }
        }
        //Przerwa kodu gdy mamy wiele razy tą samą odpowiedź
        if (czy_poprawiono == true) {
            zmienny_licznik_braku_zmian = 0;
        }
        else if (najlepszy_wynik_calkowity.dopuszczalne == true) {
            zmienny_licznik_braku_zmian++;
        }
        if (zmienny_licznik_braku_zmian >= licznik_braku_zmian) {
            printf("Brak poprawy w pokoleniach zatrzymano wczesniej nr: %d\n", pokolenie);
            break;
        }

        //Elitaryzm
        nowa_populacja[0] = populacja[index_najlepszego];

        //Tworzenie nowego pokolenia
        for (int i = 1; i < ilosc_populacji; i++) {
            int index_rodzic1 = PickPair(populacja, ilosc_populacji, rozmiar_danych, miejsce_w_tirze, lista_danych, ile_kandydatow);
            int index_rodzic2 = PickPair(populacja, ilosc_populacji, rozmiar_danych, miejsce_w_tirze, lista_danych, ile_kandydatow);
            int sprawdzacz = 0;
            while (index_rodzic1 == index_rodzic2 && sprawdzacz < 9) {
                index_rodzic2 = PickPair(populacja, ilosc_populacji, rozmiar_danych, miejsce_w_tirze, lista_danych, ile_kandydatow);
                sprawdzacz++;
            }

            CutPair(populacja[index_rodzic1], populacja[index_rodzic2], nowa_populacja[i], rozmiar_danych);
            Mutation(nowa_populacja[i], rozmiar_danych, szansa_mutacji);
        }
        //Nadpisanie populcaji
        populacja = nowa_populacja;
    }


    //Ostateczne drukowanie wyników
    if (najlepszy_wynik_calkowity.dopuszczalne == false) {
        printf("Żadna pojedyncza_trasa nie spelnia okien trasowych\n");
    }
    else {
        printf("Liczba tras to: %d\n", najlepszy_wynik_calkowity.liczba_tras);
        printf("Najlepszy koszt(czas) to: %f\n", najlepszy_wynik_calkowity.koszt);
        printf("Najlepsza kolejnosc punktow to: \n0");
        int zmienne_miejsce_w_tirze = miejsce_w_tirze;

        //Wypisanie 0 jak jedzie do bazy, podobnie do fitness
        for (int i = 0; i < rozmiar_danych; i++) {
            int wskazywacz = najlepszy_gen[i];

            int ile_towaru = lista_danych[wskazywacz].q_demand;
            bool jedzie_do_bazy = Decider(miejsce_w_tirze, zmienne_miejsce_w_tirze, ile_towaru);

            if (jedzie_do_bazy == false) {
                printf(" -> %d", wskazywacz);
            }
            else {
                i -= 1;
                printf(" -> 0");
            }
        }
        printf(" -> 0\n");
    }
    //Wypisanie czasu który minoł/ą
    auto koniec_czasu = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> czas_trwania =  koniec_czasu - start_czasu;
    printf("Czas wykonania zadania to: %f ms\n",czas_trwania.count());

    return 0;
}
