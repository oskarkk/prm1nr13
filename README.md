# prm1nr13
Program ma sprawdzić ilość nawiasów ‘’{‘’ i ‘’}’’ oraz numery linii w jakich one występują. Ma wyszukać wszystkie deklaracje zmiennych i posortować je alfabetycznie. Bez użycia zewnętrznych funkcji do operacji na stringach. Raport z analizy ma zostać zapisany do pliku.

## 1.	Opis algorytmu

Działanie programu polega na pobieraniu z pliku słów, rozumianych jako ciąg znaków nienależących do znaków w tablicy endChars. Funkcja getWord pobiera słowo i zapisuje je w zmiennej word oraz zwraca znak należący do endChars który został pobrany po słowie word. W kodzie ten znak jest zapisywany do zmiennej c.

Funkcja checkType dopisuje word do tymczasowej zmiennej tempType i porównuje tempType z nazwami typów w C. Zależnie od wyniku porównania i zawartości zmiennej c funkcja checkType albo zapisuje znalezioną zmienną w tablicy i wywołuje funkcję checkName, albo kończy się, albo wywołuje samą siebie by pobrać następne słowo typu/nazwę zmiennej.

Funkcja checkName wyszukuje następujących po nazwie zmiennej słów i znaków: średników, znaków równości, następnych zmiennych po przecinku i innych rzeczy. Będzie wywoływać samą siebie dopóki nie napotka średnika.

Program poprawnie reaguje na komentarze, wskaźniki, dowolną zawartość między cudzysłowami. Ignoruje czy dana zmienna jest tablicą, ignoruje struktury, unie, typy definiowane (prawdopodobnie wystąpiłyby błędy przy analizie kodu ze strukturami). Nie wykrywa zmiennych wewnątrz zwykłych nawiasów, przykładowo nie znajdzie zmiennej i w „for(int i;;)”.

Parametry programu:
* 1 – plik z kodem do analizy
* 2 – plik w którym zostanie zapisany raport

## 2. Stałe

* VAR_MAX – maksymalna liczba zmiennych w analizowanym kodzie
* NAME_MAX – maksymalna długość typu i nazwy zmiennej
* WORD_MAX – maksymalna długość pobranego w getWord słowa
* BRACES_MAX – maksymalna liczba zakręconych nawiasów


## 3. Test działania programu na przykładowym kodzie

Kod wejściowy:
```c
int i ;
int j;
long int i;
int**** b ;
double c, y=100, *bb[ ]={"ccccdfs"} ;
razraz;
qwerty(1234567);


// int xxx; tej zmiennej program nie widzi

/* int zzzz;
* tej tez {{{{{{
*/

int yyy;

unsigned 
long 
long 
int
szuruburu
=
123456;

char tablica[100] [ 10000 ] = "float dwa \", trzy";


// funkcja
int ptrVar(char *word, FILE* f, char *tempType) {
    int c;
    ptrCount++;
    while((c=getWord(word,f))=='*') ptrCount++;
    saveVar(tempType,word);
    return c;
}
```

Wynik:

```
Liczba lewych nawiasow "{": 2
W liniach: 5 28 

Liczba prawych nawiasow "}": 2
W liniach: 5 34 


Alfabetyczna lista zmiennych i ich typow:

char tablica
double *bb
double c
double y
int ****b
int c
int i
int j
int yyy
long int i
unsigned long long int szuruburu
```

## 4. Test działania programu na jego własnym kodzie

Kod wejściowy: kod programu (program analizuje sam siebie)

Wynik:

```
Liczba lewych nawiasow "{": 56
W liniach: 31 71 72 79 84 104 115 117 118 121 130 136 140 144 150 152 156 164 165 175 177 190 192 196 251 253 254 256 259 274 284 286 295 297 299 316 321 328 336 349 350 355 358 368 383 384 394 411 413 423 425 435 439 449 457 466 

Liczba prawych nawiasow "}": 56
W liniach: 65 71 72 79 106 111 117 118 125 126 147 148 156 161 164 173 174 175 182 185 186 194 245 246 253 258 266 267 268 289 299 306 307 308 310 325 333 341 344 361 362 363 364 378 388 389 403 417 418 428 430 443 444 452 461 473 


Alfabetyczna lista zmiennych i ich typow:

char endChars
char temp
char tempType
char typeNames
char varArray
char whitespace
char word
int c
int c
int i
int i
int i
int i
int i
int i
int i
int i
int i
int i
int i
int isType
int j
int j
int lastSpace
int leftCount
int leftLines
int lineNumber
int minIndex
int ptrCount
int rightCount
int rightLines
int temp
int varCount 
```
