#include <stdio.h>
#include <stdlib.h>

#define VAR_MAX 1000
#define NAME_MAX 100
#define WORD_MAX 50
#define BRACES_MAX 1000

int getWord(char*, FILE*);

void analyse(FILE*);
void checkType(char*, FILE*, char*, int*);
void checkName(char*, FILE*, char*, int*);

void saveVar(char*, char*);
int ptrVar(char*, FILE*, char*);
void sortVarArray();

void checkBraces(int);
int hasSpace(char*);

void removeIgnoredText(FILE*, char*);
void removeComments(FILE*, char*);

int checkChar(char, char*);
int compareStr(char*, char*);
void appendStr(char*, char*);
void nullStr(char*);
void removeLastWord(char*);

char typeNames[40][30] = {
    "_Bool",
    "char",
    "double",
    "double _Complex",
    "float",
    "float _Complex",
    "int",
    "long",
    "long double",
    "long double _Complex",
    "long int",
    "long long",
    "long long int",
    "short",
    "short int",
    "signed",
    "signed char",
    "signed int",
    "signed long",
    "signed long int",
    "signed long long",
    "signed long long int",
    "signed short",
    "signed short int",
    "unsigned",
    "unsigned char",
    "unsigned int",
    "unsigned long",
    "unsigned long int",
    "unsigned long long",
    "unsigned long long int",
    "unsigned short",
    "unsigned short int",
};

// zmienne liczby zakreconych nawiasow
int leftCount = 0;
int rightCount = 0;
// tablice numerow linii kodu w ktorych sa nawiasy
int leftLines[BRACES_MAX] = {0};
int rightLines[BRACES_MAX] = {0};
// zmienna tymczasowej linii kodu (liczymy od 1)
int lineNumber = 1;

// zmienna liczby zapisanych zmiennych
int varCount = 0;
// tablica z wszystkimi znalezionymi zmiennymi i ich typami
char varArray[VAR_MAX][NAME_MAX+1] = {0};
// zmienna tymczasowej liczby pointerow przy nazwie zmiennej
int ptrCount = 0;


int main(int argc, char *argv[]) {
    int i;

    FILE *fileIn;
    fileIn = fopen(argv[1], "r");

    FILE *fileOut;
    fileOut = fopen(argv[2], "w");

    analyse(fileIn);
    sortVarArray();

    fprintf(fileOut,"Liczba lewych nawiasow \"{\": %d\nW liniach: ",leftCount);
    for(i=0; leftLines[i]!=0; i++) fprintf(fileOut,"%d ",leftLines[i]);
    fprintf(fileOut,"\n\n");
    fprintf(fileOut,"Liczba prawych nawiasow \"}\": %d\nW liniach: ",rightCount);
    for(i=0; rightLines[i]!=0; i++) fprintf(fileOut,"%d ",rightLines[i]);
    fprintf(fileOut,"\n\n\n");

    fprintf(fileOut,"Alfabetyczna lista zmiennych i ich typow:\n\n");
    for(i=0; i<varCount; i++){
        fprintf(fileOut,"%s\n",varArray[i]);
    }

    fclose(fileIn);
    fclose(fileOut);
    return 0;
}


void analyse(FILE* f) {
    int c;
    char word[WORD_MAX+1] = {0};
    char tempType[NAME_MAX+1] = {0};

    // dopoki nie dojdziemy do EOF pobieramy slowa
    do {
        c = getWord(word,f);
        // sprawdzamy czy slowo pasuje do typow zmiennych
        if(word[0]!=0) checkType(word,f,tempType,&c);
    } while (c!=EOF);
}


void checkType(char *word, FILE* f, char *tempType, int *c) {
    // jezeli jest cos w tempType to dodajemy spacje zeby oddzielic slowo ktore tam zaraz dodamy
    if(tempType[0]!=0) appendStr(tempType, " ");
    // dodajemy do tempType slowo pobrane w analyse() lub gdzies w checkType()
    appendStr(tempType, word);
    // sprawdzamy czy zostalo pobrane slowo czy tylko jakis znak specjalny
    if(word[0]!=0) {
        // jesli slowo, to sprawdzamy czy pobrane slowo nalezy do slow typow
        int isType = 0;
        int i;
        for(i=0; i<40; i++) {
            // kazdy typ porownujemy z tempType
            // bardzo wygodny jest fakt ze kazdy wielowyrazowy typ...
            // ... po odjeciu ostatniego slowa jest nazwa innego typu
            if(compareStr(tempType,typeNames[i])==0) {
                isType = 1;
                break;
            }
        }
        // jesli nalezy, to sprawdzamy jaki znak po nim stal
        if(isType==1) {
            // jesli to spacja to sprawdzamy nastepne slowo
            if(*c==' ') {
                *c = getWord(word,f);
                checkType(word,f,tempType,c);
            // jesli to * to nastepne slowo jest nastepna gwiazdka lub nazwa zmiennej
            } else if(*c=='*') {
                // sprawdzamy czy jest wiecej *, zapisujemy pointerowa zmienna
                *c = ptrVar(word,f,tempType);
                // przechodzimy do analizy potencjalnych dalszych nazw zmiennych
                checkName(word,f,tempType,c);
            };
        // jesli nie nalezy i nie jest to pierwsze slowo w tempType...
        // ... to musi byc nazwa zmiennej
        } else {
            if(hasSpace(tempType)==1) {
                // usuwamy nazwe zmiennej z tempType
                removeLastWord(tempType);
                // ladujemy typ i nazwe do tablicy
                saveVar(tempType,word);
                // przechodzimy do analizy potencjalnych dalszych nazw zmiennych
                checkName(word,f,tempType,c);
            // zerujemy nullStr bo inaczej dalej tam bedzie gdy bedzie sprawdzac nastepne slowo
            } else nullStr(tempType);
        }
    } else {
        // jesli zostal pobrany tylko znak specjalny to sprawdzmy czy to *
        if(*c=='*') {
            // sprawdzamy czy jest wiecej *, zapisujemy pointerowa zmienna
            *c = ptrVar(word,f,tempType);
            // przechodzimy do analizy potencjalnych dalszych nazw zmiennych
            checkName(word,f,tempType,c);
        };
        // jesli to nie * to musi byc to jakis blad w kodzie
        // nie moze byc nic miedzy nazwa typu a gwiazdka
    }
}


void checkName(char *word, FILE* f, char *tempType, int *c) {
    // jezeli slowo nie jest puste to jest to nazwa zmiennej ktora trzeba zapisac
    if(word[0]!=0) {
        saveVar(tempType,word);
    }

    switch(*c) { // TODO: na razie ignoruje czy cos jest tablica, problem ze znakiem rownosci
        // pointer
        case '*':
            *c = ptrVar(word,f,tempType);
            checkName(word,f,tempType,c);
            break;
        // przecinek, czyli nastepna zmienna tego samego typu
        case ',':
            *c = getWord(word,f);
            checkName(word,f,tempType,c);
            break;
        // tablica, TODO: nie ignorowac
        case '[':
            // dopoki nie znajdziemy ] to bierzemy nastepne slowa, nie ogladamy sie za siebie
            while((*c = getWord(word,f))!=']');
            // wracamy do szukania srednika lub przecinka
            *c = getWord(word,f);
            checkName(word,f,tempType,c);
            break;
        // wartosc zmiennej, ignorujemy wszystko do przecinka lub srednika
        case '=':
            do {
                *c = getWord(word,f);
                // jesli znajdziemy taki nawias to musimy ignorowac wszystko w srodku
                if(*c=='{') while((*c = getWord(word,f))!='}');
            } while(*c != ',' && *c != ';');
            // zerujemy word, bo to co tam jest to wartosc zmiennej
            // checkName nie moze tego wsadzic do tablicy
            nullStr(word);
            // wracamy do szukania srednika lub przecinka
            checkName(word,f,tempType,c);
            break;
        // definitywny koniec szukania nastepnych slow zmiennej
        // stad probujemy wrocic do analyse
        case ';':
            // zerowanie tempType - zeby mozna bylo uzyc tego nastepnym razem
            nullStr(tempType);
            break;
        // nawias domykajacy, patrz funkcja removeIgnoredText
        // jesli sie tu pojawia, to znaczy ze wlasnie do tablicy zostala wciagnieta...
        // ... deklaracja funkcji i trzeba ja usunac oraz zdekrementowac licznik zmiennych
        case ')':
            nullStr(varArray[--varCount]);
            // zerowanie tempType - siedzi tam typ zwracany przez wciagnieta funkcje
            nullStr(tempType);
            break;
        // jesli nic z powyzszych to nastepne slowo jest albo nazwa zmiennej...
        // ... albo jakims znakiem specjalnym
        default:
            *c = getWord(word,f);
            checkName(word,f,tempType,c);
            break;
    }
}


// funkcja sortujaca tablice zmiennych
void sortVarArray() {
    int i, j, minIndex;
    char temp[NAME_MAX+1] = {0};
    for(i=0; varArray[i][0]!=0; i++) {
        minIndex = i;
        for(j=i+1; varArray[j][0]!=0; j++) {
            if(compareStr(varArray[j],varArray[minIndex])==-1) minIndex = j;
        }
        if(minIndex != i) {
            appendStr(temp,varArray[i]);
            nullStr(varArray[i]);
            appendStr(varArray[i],varArray[minIndex]);
            nullStr(varArray[minIndex]);
            appendStr(varArray[minIndex],temp);
            nullStr(temp);
        }
    }
}


// funkcja pobierajaca slowo o dlugosci >=0 z pliku
// zwraca znak specjalny nastepujacy po tym slowie
int getWord(char *word, FILE* f) {
    int i;
    // tablica znakow ktorymi slowa maja sie nie zaczynac
    char whitespace[] = " \n\t";
    // tablica znakow ktorymi maja konczyc sie slowa
    char endChars[] = "\"\' \n\t{}()[];/\\*,=";

    for(i=0; i<WORD_MAX; i++) word[i]='\0';

    // pobiera kolejne znaki slowa
    for(i=0; i<WORD_MAX; i++) {
        // jesli napotka EOF to go zwraca
        if ((word[i]=getc(f))==EOF) {
            word[i] = 0;
            return EOF;
        }
        // omijamy nawiasy, apostrofy, cudzyslowy, komentarze i ich zawartosc
        removeIgnoredText(f,&word[i]);
        // jesli znak jest koncem linii to mamy nowa linie kodu
        if(word[i]=='\n') lineNumber++;
        // sprawdza czy obecny znak to jeden z endChars
        if(checkChar(word[i],endChars) == 1) {
            // jesli to pierwszy znak w slowie i jest to whitespace to wracamy na poczatek petli
            if(i==0 && checkChar(word[0],whitespace)==1) {
                i--;
            } else {
                // sprawdzamy czy to sa zawiniete nawiasy i je liczymy
                checkBraces(word[i]);
                // jesli to endChar to usuwamy go ze slowa i zwracamy go jako wynik funkcji
                int temp = word[i];
                word[i] = 0;
                return temp;
            }
        }
    }
    return 0;
}


// funkcja usuwajaca nawiasy, apostrofy, cudzyslowy, komentarze i ich zawartosc
// uwaga - nie sprawdza EOT, zaklada ze kod jest poprawny
void removeIgnoredText(FILE *f, char *c) {
    // zwykle nawiasy
    // zostawia domykajacy nawias...
    // ... zeby checkName moglo usunac deklaracje funkcji ze zmiennych
    // jesli napotka komentarz przed nawiasem zamykajacym to go usuwa
    if(*c=='(') do {
        if(*c=='\n') lineNumber++;
        *c = getc(f);
        removeComments(f,c);
    } while(*c != ')');
    // apostrofy
    // bierze znaki dopoki nie napotka nastepnego apostrofu
    if(*c=='\'') while((*c=getc(f)) != '\'') {
        if(*c=='\n') lineNumber++;
        // ale jesli napotka \ to pobierze nastepny znak...
        // ... i nawet jesli to apostrof to na niego nie zareaguje
        if(*c=='\\') *c=getc(f);
    }
    // cudzyslowy
    // bierze znaki dopoki nie napotka nastepnego cudzyslowu
    if(*c=='\"') while((*c=getc(f)) != '\"') {
        if(*c=='\n') lineNumber++;
        // ale jesli napotka \ to pobierze nastepny znak...
        // ... i nawet jesli to cudzyslow to na niego nie zareaguje
        if(*c=='\\') *c=getc(f);
    }
    // komentarze
    removeComments(f,c);
}


// funkcja usuwajaca komentarze
void removeComments(FILE *f, char *c) {
    if(*c=='/') {
        *c = getc(f);
        // komentarz w stylu "//" - bierze wszystko do '\n'
        if(*c=='/') while((*c=getc(f)) != '\n');
        // komentarz w stylu "/* ... */"
        else if(*c=='*') {
            // dopoki nie napotka gwiazdki bedzie bralo nastepne znaki
            // a gdy napotka gwiazdke po ktorej nie bedzie / to znowu szuka gwiazdki
            do {
                if(*c=='\n') lineNumber++;
                while((*c=getc(f)) != '*');
            } while((*c=getc(f)) != '/');
        }
    }
}


// funkcja zapisujaca typ i nzwe zmiennej do tablicy varArray
void saveVar(char *type, char *word) {
    appendStr(varArray[varCount], type);
    appendStr(varArray[varCount], " ");
    int i = 0;
    for(i=0; i<ptrCount; i++) appendStr(varArray[varCount],"*");
    ptrCount = 0;
    appendStr(varArray[varCount++], word);
    // zerujemy word bo to nazwa ktora wlasnie dodalismy do tablicy
    // gdyby nie to to by zostala podlana przez checkType/checkName
    nullStr(word);
}


// funkcja sprawdzajaca czy c jest zakreconym nawiasem '{' lub '}'
void checkBraces(int c) {
    switch(c) {
        case '{': leftLines[leftCount++] = lineNumber; break;
        case '}': rightLines[rightCount++] = lineNumber; break;
        default: break;
    }
}


// funkcja pobierajaca gwiazdki dopoki sie nie skoncza i ladujaca pointer do tablicy
int ptrVar(char *word, FILE* f, char *tempType) {
    int c;
    // liczymy pointery
    ptrCount++;
    while((c=getWord(word,f))=='*') ptrCount++;
    // ladujemy typ, gwiazdki i nazwe do tablicy
    saveVar(tempType,word);
    // zwracamy c
    return c;
}


// funkcja ktora zwróci:
// 1 gdy str1 jest alfabetycznie wyzej od str2
// -1 w przeciwnym wypadku
// 0 jezeli sa takie same
int compareStr(char *str1, char *str2) {
    int i;
    for(i=0;;i++) {
        if(str1[i] > str2[i]) return 1;
        if(str1[i] < str2[i]) return -1;
        if(str1[i]==0 && str2[i]==0) return 0;
    }
}


// funkcja ktora zwróci 1 gdy c zawiera sie w ciagu znaków str a 0 jezeli nie
int checkChar(char c, char *str) {
    int i = 0;
    while(str[i]!=0) {
        if(str[i] == c) return 1;
        i++;
    }
    return 0;
}


// funkcja dodajaca ciag znaków str2 na koncu str1 zakladajac ze w str1 jest miejsce
void appendStr(char *str1, char *str2) {
    int i = 0;
    int j = 0;
    while(str1[i] != '\0') i++;
    while(str2[j] != '\0') {
        str1[i] = str2[j];
        i++;
        j++;
    }
}


// funkcja zerujaca str
void nullStr(char *str) {
    int i;
    for(i=0; str[i]!=0; i++) str[i] = 0;
}


// funkcja sprawdzajaca czy str zawiera spacje i zwracajaca 1 jesli tak
int hasSpace(char *str) {
    int i;
    for(i=0; str[i]!=0; i++) if(str[i] == ' ') return 1;
    return 0;
}


// funkcja usuwajaca ostatnie slowo (wraz ze spacja) w stringu skladajacym sie ze slow i spacji
void removeLastWord(char *str) {
    int i = 0;
    int lastSpace;
    // poki nie dojdziemy do konca str miejsce kazdej kolejnej spacji zapisujemy w lastSpace
    for(i=0; str[i]!=0; i++) if(str[i]==' ') lastSpace = i;
    // od miejsca lastSpace w str zerujemy kazdy char
    for(i=lastSpace; str[i]!=0; i++) str[i] = 0;
}
