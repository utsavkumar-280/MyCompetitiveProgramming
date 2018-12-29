/*************************************************************************
 *                                                                       *
 *                     XVI Olimpiada Informatyczna                       *
 *                                                                       *
 *   Zadanie:  Wyspa (WYS)                                               *
 *   Plik:     wys3.cpp                                                  *
 *   Autor:    Pawel Parys                                               *
 *   Opis:     Rozwiazanie wzorcowe O(n+m) z ulamkami zamiast double     *
 *                                                                       *
 *************************************************************************/

#include <cstdio>
#include <vector>
#include <cmath>

using namespace std;

#define REP(a, n) for (int a = 0; a<(n); ++a)
#define FOR(a, b, c) for (int a = (b); a<=(c); ++a)
#define FORD(a, b, c) for (int a = (b); a>=(c); --a)
#define VAR(a, v) __typeof(v) a = (v)
#define FOREACH(a, v) for (VAR(a, (v).begin()); a!=(v).end(); ++a)

#define PB push_back
#define MP make_pair

template<class T>
inline int size(const T &t) { return t.size(); }
typedef long long LL;

/////////////// ulamki:

struct Ulamek {
    LL l;   // licznik to iloczyn max 3 wsporzednych, a mianownik max 2
    LL m;
    inline double to_double() {
        return l/(double)m;
    }
};

inline Ulamek operator*(Ulamek a, Ulamek b) {
    Ulamek w;
    w.l = a.l*b.l;
    w.m = a.m*b.m;
    return w;
}

inline Ulamek operator/(Ulamek a, Ulamek b) {
    Ulamek w;
    w.l = a.l*b.m;
    w.m = a.m*b.l;
    return w;
}


inline Ulamek operator-(Ulamek a, Ulamek b) {
    Ulamek w;
    w.l = a.l*b.m-b.l*a.m;
    w.m = a.m*b.m;
    return w;
}

inline Ulamek operator+(Ulamek a, Ulamek b) {
    Ulamek w;
    w.l = a.l*b.m+b.l*a.m;
    w.m = a.m*b.m;
    return w;
}


/////////////// punkty:

struct Punkt {
    Ulamek x, y;
};

inline Ulamek operator*(Punkt a, Punkt b) {
    return a.x*b.y-a.y*b.x;
}

inline Punkt operator*(Punkt p, Ulamek k) {
    p.x = p.x*k;
    p.y = p.y*k;
    return p;
}

inline Punkt operator+(Punkt a, Punkt b) {
    a.x = a.x+b.x;
    a.y = a.y+b.y;
    return a;
}

inline Punkt operator-(Punkt a, Punkt b) {
    a.x = a.x-b.x;
    a.y = a.y-b.y;
    return a;
}


Punkt pkt_przec(Punkt a, Punkt b, Punkt c, Punkt d) { // punkt przeciecia linii a-b z c-d
    Ulamek k = (a-c)*(d-c)/((a-b)*(d-c)); // (pole trojkata a-c-d)/(pole czworokata a-b-c-d)   [poniewaz * to il_wek ]
    return a+(b-a)*k; // tutaj * to mnozenie przez stala
}

////////////// program wlasciwy:

#define MAX_N 100000
//#define MAX_M 100000

// wszystkie numeracje od 0

Punkt punkty[MAX_N];

vector<int> brakujace[MAX_N]; // krawedzie, ktorych nie ma (czyli te z wejscia)

int ostatni[MAX_N]; // najwiekszy wierzcholek, do ktorego mamy krawedz
    // lub -1 jesli nie zadnego wiekszego niz ostatni dla wczesniejszych wierzcholkow

bool czy_obstawiana[MAX_N]; // tablica tymczasowa: czy krawedz z aktualnego jest blokowana

vector<pair<int, Punkt> > stos; // trasa
  // w parze sa: poczatek krawedzi (koniec jest w ostatni[]), punkt przeciecia z poprzednia (dla pierwszej punkt startowy)

bool czy_na_lewo(Punkt p, int a, int b) {
    Punkt w1 = p-punkty[a];  // tutaj mianowniki na x i y sa te same
    Punkt w2 = punkty[b]-punkty[a];  // tutaj mianowniki sa 1
    // mamy zwrocic w1.x*w2.y-w2.x*w1.y<0
//    printf("czy %Ld/%Ld %Ld/%Ld jest na lewo od %Ld/%Ld %Ld/%Ld\n", w1.x.l, w1.x.m, w1.y.l, w1.y.m, w2.x.l, w2.x.m, w2.y.l, w2.y.m);

    #define LICZBA 1000000000
    LL cz_c1 = w1.x.l/LICZBA;
    w1.x.l %= LICZBA;
    cz_c1 *= w2.y.l;
    w1.x.l *= w2.y.l;
    cz_c1 += w1.x.l/LICZBA;
    w1.x.l %= LICZBA;

    LL cz_c2 = w1.y.l/LICZBA;
    w1.y.l %= LICZBA;
    cz_c2 *= w2.x.l;
    w1.y.l *= w2.x.l;
    cz_c2 += w1.y.l/LICZBA;
    w1.y.l %= LICZBA;
    
//    printf("%Ld %Ld ? %Ld %Ld\n", cz_c1, w1.x.l, cz_c2, w1.y.l);
    if (cz_c1<cz_c2) return true;
    if (cz_c1>cz_c2) return false;
    return w1.x.l<w1.y.l;
}

int main() {
    // wczytywanie
    int N, M;
    scanf("%d%d", &N, &M);
    REP(a, N) {
        scanf("%Ld%Ld", &punkty[a].x.l, &punkty[a].y.l);
        punkty[a].x.m = punkty[a].y.m = 1;
    }
    REP(a, M) {
        int i, j;
        scanf("%d%d", &i, &j);
        --i, --j;
        brakujace[i].PB(j);
    }
    
    // znajdowanie krawedzi po ktorych w ogole warto chodzic
    int max_ost = 0;
    REP(a, N) {
        FOREACH(it, brakujace[a])
            czy_obstawiana[*it] = true;
        ostatni[a] = -1;
        FORD(b, N-1, max_ost+1)
            if (czy_obstawiana[b])
                czy_obstawiana[b] = false;
            else
                ostatni[a] = max_ost = b; // (czyli konczymy petle, dalej nie trzeba zerowac)
        // wewnetrzna petla zrobi w sumie najwyzej M obrotow
    } 
    
    // szukanie trasy
    // (z zalozenia jakas trasa istnieje)
    stos.PB(MP(0, punkty[0]));
    FOR(a, 1, N-1) {
        if (ostatni[a]<0) 
            continue;
        for (;;) {
            int ost = stos.back().first; // ostatni
            Punkt prz = pkt_przec(punkty[ost], punkty[ostatni[ost]], punkty[a], punkty[ostatni[a]]);
//            printf("punkt przeciecia %d z %d to %Ld/%Ld %Ld/%Ld\n", a, ost, prz.x.l, prz.x.m, prz.y.l, prz.y.m);
            // na pewno proste nie sa rownolegle (z zalozenia)
            if (size(stos)>1) {
                int poprz = stos[size(stos)-2].first; // przedostatni
                if (czy_na_lewo(prz, poprz, ostatni[poprz])) { // ostatnia krawedz na stosie niepotrzebna
                    stos.pop_back();
//                    printf("zdejmuje\n");
                    continue;
                }
            }
            stos.PB(MP(a, prz));
            break;
        }
    }
    
    // liczenie dlugosci trasy
    stos.PB(MP(N-1, punkty[N-1])); // wstawiamy ostatni punkt
    double len = 0;
    REP(a, size(stos)-1) {
        double x = stos[a+1].second.x.to_double();
        double y = stos[a+1].second.y.to_double();
        x -= stos[a].second.x.to_double();
        y -= stos[a].second.y.to_double();
        len += sqrt(x*x+y*y);
//        printf("%d %f %f\n", stos[a].first, stos[a].second.x, stos[a].second.y);
    }
    
//    fprintf(stderr, "Trasa sklada sie z %d odcinkow\n", size(stos)-1);
    printf("%lf\n", len);
}
