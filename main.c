#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <time.h>
#include <string.h>
    
// Terminal renk kodlari (renkli cikti icin)
#define RESET  "\033[0m"
#define GREEN  "\033[32m"
#define WHITE  "\033[37m"
#define CYAN   "\033[36m"
#define RED    "\033[31m"
#define YELLOW "\033[33m"

// Fonksiyon bildirimleri
void displayMonth(int year, int month);
void displayFullYear(int year);
int getDaysInMonth(int year, int month);
bool isLeapYear(int year);
void printMonthHeader(int year, int month);
void printMonthBody(int year, int month);
void addNoteForDay(int year, int month, int day);
void viewNotesForDay(int year, int month, int day);
bool hasNoteForDay(int year, int month, int day);
void loadNotes(int year);
void appendNoteToFile(int year, int month, int day, const char* noteText);

// Notlar dizisi (12 ay, 31 gun, 100 karakterlik notlar)
char notes[12][31][100];

int main() {
    int year, choice;

    printf("Takvim Uygulamasi\n");

    // Kullanıcıdan yıl bilgisi alınıyor
    printf("Lutfen yil giriniz (ornek: 2024): ");
    scanf("%d", &year);

    // Notlar dizisini temizleme
    for (int m = 0; m < 12; m++) {
        for (int d = 0; d < 31; d++) {
            notes[m][d][0] = '\0'; // Her hücreye boş string atanıyor
        }
    }

    // Dosyadan notları yükleme
    loadNotes(year);

    // Ana menü döngüsü
    do {
        printf("\n1. Bir ayi goruntule\n");
        printf("2. Tum yili goruntule\n");
        printf("3. Gun icin not ekle\n");
        printf("4. Gun icin not goruntule\n");
        printf("5. Cikis\n");
        printf("Seciminiz: ");
        scanf("%d", &choice);

        if (choice == 1) {
            int month;
            printf("Lutfen ayi giriniz (1-12): ");
            scanf("%d", &month);

            // Ay kontrolü
            if (month < 1 || month > 12) {
                printf(RED"Gecersiz ay girdiniz. Lutfen 1 ile 12 arasinda bir deger girin.\n"RESET);
                continue;
            }

            // Belirtilen ayı görüntüle
            displayMonth(year, month);

        } else if (choice == 2) {
            // Tüm yılı görüntüle
            displayFullYear(year);

        } else if (choice == 3 || choice == 4) {
            // Gün ve ay bilgisi al
            int month, day;
            printf("Lutfen ayi giriniz (1-12): ");
            scanf("%d", &month);

            // Ay kontrolü
            if (month < 1 || month > 12) {
                printf(RED"Gecersiz ay girdiniz.\n"RESET);
                continue;
            }

            printf("Gun seciniz (1-%d): ", getDaysInMonth(year, month));
            scanf("%d", &day);

            // Gün kontrolü
            if (day < 1 || day > getDaysInMonth(year, month)) {
                printf(RED"Gecersiz gun secimi!\n"RESET);
                continue;
            }

            // Seçime göre işlem yap
            if (choice == 3) {
                addNoteForDay(year, month, day);
            } else if (choice == 4) {
                viewNotesForDay(year, month, day);
            }

        } else if (choice != 5) {
            // Geçersiz seçim
            printf(RED"Gecersiz secim!\n"RESET);
        }

    } while (choice != 5);

    // Programdan çıkış
    printf(GREEN"Iyi gunler dileriz!\n"RESET);
    return 0;
}

// Dosyadan notları yükler
void loadNotes(int year) {
    FILE* fp = fopen("notes.txt", "r");
    if (!fp) {
        // Dosya yoksa notlar boş kalır
        return;
    }

    int y, m, d;
    char noteLine[200];
    // "year month day note..." formatında satırları okur
    while (fgets(noteLine, sizeof(noteLine), fp)) {
        char noteText[100];
        if (sscanf(noteLine, "%d %d %d %[^\n]", &y, &m, &d, noteText) == 4) {
            if (y == year && m >= 1 && m <= 12 && d >= 1 && d <= getDaysInMonth(y, m)) {
                strncpy(notes[m-1][d-1], noteText, 100);
                notes[m-1][d-1][99] = '\0';
            }
        }
    }

    fclose(fp);
}

// Notları dosyaya ekler
void appendNoteToFile(int year, int month, int day, const char* noteText) {
    FILE* fp = fopen("notes.txt", "a");
    if (!fp) {
        printf(RED"Notlar dosyaya yazilamadi!\n"RESET);
        return;
    }
    fprintf(fp, "%d %d %d %s\n", year, month, day, noteText);
    fclose(fp);
}

// Belirtilen ayı görüntüler
void displayMonth(int year, int month) {
    printMonthHeader(year, month);
    printMonthBody(year, month);
}

// Tüm yılı görüntüler
void displayFullYear(int year) {
    for (int month = 1; month <= 12; month++) {
        displayMonth(year, month);
    }
}

// Artık yıl kontrolü
bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Ayın toplam gün sayısını döndürür
int getDaysInMonth(int year, int month) {
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return daysInMonth[month - 1];
}

// Ay başlığını yazdırır
void printMonthHeader(int year, int month) {
    printf("\n"CYAN" %4d Yili - %2d. Ay"RESET"\n", year, month);
    printf(" Pzt  Sal  Car  Per  Cum  Cmt  Paz\n");
}

// Ayın gövdesini yazdırır
void printMonthBody(int year, int month) {
    struct tm firstDay = {0};
    firstDay.tm_year = year - 1900;
    firstDay.tm_mon = month - 1;
    firstDay.tm_mday = 1;
    mktime(&firstDay);

    int startDay = firstDay.tm_wday;
    if (startDay == 0) startDay = 7; // Pazar'ı haftanın son günü yap

    int daysInMonth = getDaysInMonth(year, month);

    // İlk gün öncesindeki boşlukları yazdır
    for (int i = 1; i < startDay; i++) {
        printf("     ");
    }

    // Ayın günlerini yazdır
    for (int day = 1; day <= daysInMonth; day++) {
        if (hasNoteForDay(year, month, day)) {
            printf(GREEN"%4d "RESET, day); // Not varsa yeşil renkte yazdır
        } else {
            printf("%4d ", day); // Normal renkte yazdır
        }

        // Haftanın sonunda yeni satır
        if ((startDay + day - 1) % 7 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

// Belirli bir gün için not ekler
void addNoteForDay(int year, int month, int day) {
    printf("Not giriniz (100 karakterden az): ");
    getchar(); // Önceki satır karakterini temizler
    char noteText[100];
    fgets(noteText, 100, stdin);
    noteText[strcspn(noteText, "\n")] = '\0'; // Yeni satır karakterini kaldır

    // Notu bellekte sakla
    strncpy(notes[month - 1][day - 1], noteText, 100);
    notes[month - 1][day - 1][99] = '\0';

    // Notu dosyaya ekle
    appendNoteToFile(year, month, day, noteText);

    printf(GREEN"Not eklendi!\n"RESET);
}

// Belirli bir gün için notları görüntüler
void viewNotesForDay(int year, int month, int day) {
    if (hasNoteForDay(year, month, day)) {
        printf(YELLOW"Not: %s\n"RESET, notes[month - 1][day - 1]);
    } else {
        printf(RED"Bu gun icin not bulunamadi.\n"RESET);
    }
}

// Belirli bir gün için not olup olmadığını kontrol eder
bool hasNoteForDay(int year, int month, int day) {
    return (strlen(notes[month - 1][day - 1]) > 0);
}
