#include "fonctions.h"
#ifdef _WIN32
    #include <windows.h>
#endif

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void lire_chaine(char *chaine, int taille) {
    fgets(chaine, taille, stdin);
    chaine[strcspn(chaine, "\n")] = 0;
}

int lire_entier() {
    char buffer[100];
    int nombre;
    
    fgets(buffer, sizeof(buffer), stdin);
    
    // Remove newline
    buffer[strcspn(buffer, "\n")] = 0;
    
    // Check for special "00" exit command
    if (strcmp(buffer, "00") == 0) {
        return -1;  // Return -1 for exit (00)
    }
    
    while (sscanf(buffer, "%d", &nombre) != 1) {
        printf(COLOR_RED "Veuillez entrer un nombre valide : " COLOR_RESET);
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Check again for "00"
        if (strcmp(buffer, "00") == 0) {
            return -1;
        }
    }
    
    return nombre;
}

float lire_float() {
    char buffer[100];
    float nombre;
    
    fgets(buffer, sizeof(buffer), stdin);
    
    while (sscanf(buffer, "%f", &nombre) != 1) {
        printf(COLOR_RED "Veuillez entrer un nombre valide : " COLOR_RESET);
        fgets(buffer, sizeof(buffer), stdin);
    }
    
    return nombre;
}

int obtenir_largeur_console() {
    #ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        int largeur = 80; // Default
        
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            largeur = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        }
        return largeur;
    #else
        return 80; // Default pour non-Windows
    #endif
}

void centrer_texte(const char *texte) {
    int largeur_console = obtenir_largeur_console();
    int longueur = strlen(texte);
    int espaces = (largeur_console - longueur) / 2;
    
    if (espaces > 0) {
        for (int i = 0; i < espaces; i++) {
            printf(" ");
        }
    }
    printf("%s\n", texte);
}

void centrer_texte_sans_newline(const char *texte) {
    int largeur_console = obtenir_largeur_console();
    int longueur = strlen(texte);
    int espaces = (largeur_console - longueur) / 2;
    
    if (espaces > 0) {
        for (int i = 0; i < espaces; i++) {
            printf(" ");
        }
    }
    printf("%s", texte);
}

void afficher_ligne_separatrice(char caractere, int longueur) {
    int largeur_console = obtenir_largeur_console();
    int largeur = (longueur > 0) ? longueur : largeur_console;
    
    for (int i = 0; i < largeur; i++) {
        printf("%c", caractere);
    }
    printf("\n");
}

void afficher_titre(const char *titre) {
    int largeur_console = obtenir_largeur_console();
    
    printf(COLOR_GREEN);
    afficher_ligne_separatrice('=', largeur_console);
    centrer_texte(titre);
    afficher_ligne_separatrice('=', largeur_console);
    printf(COLOR_RESET);
}

void gotoxy(int x, int y) {
    #ifdef _WIN32
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    #else
        printf("\033[%d;%dH", y, x);
    #endif
}

void setColor(int color) {
    #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
    #else
        printf("\033[%dm", color);
    #endif
}

// Valider le format d'une date (JJ-MM-AAAA)
int valider_date(const char *date) {
    if (strlen(date) != 10) return 0;
    if (date[2] != '-' || date[5] != '-') return 0;
    
    // Extract day, month, year
    int jour = (date[0] - '0') * 10 + (date[1] - '0');
    int mois = (date[3] - '0') * 10 + (date[4] - '0');
    int annee = (date[6] - '0') * 1000 + (date[7] - '0') * 100 + 
                (date[8] - '0') * 10 + (date[9] - '0');
    
    // Validate ranges (format only, not checking if date is in past)
    if (jour < 1 || jour > 31) return 0;
    if (mois < 1 || mois > 12) return 0;
    if (annee < 1900 || annee > 2100) return 0;
    
    // Days per month validation
    int jours_par_mois[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Leap year check
    if ((annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0)) {
        jours_par_mois[1] = 29;
    }
    
    if (jour > jours_par_mois[mois - 1]) return 0;
    
    return 1;  // Date format is valid

}

int valider_quantite(int qte) {
    return qte > 0;
}

int valider_prix(float prix) {
    return prix > 0;
}

void obtenir_date_du_jour(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, 11, "%d-%m-%Y", tm_info);
}

int comparer_dates(const char *date1, const char *date2) {
    // Convertir les dates en jours depuis une date de reference
    int j1, m1, a1, j2, m2, a2;
    sscanf(date1, "%d-%d-%d", &j1, &m1, &a1);
    sscanf(date2, "%d-%d-%d", &j2, &m2, &a2);
    
    int total1 = a1 * 365 + m1 * 30 + j1;
    int total2 = a2 * 365 + m2 * 30 + j2;
    
    if (total1 < total2) return -1;
    if (total1 > total2) return 1;
    return 0;
}

int jours_entre_dates(const char *date1, const char *date2) {
    int j1, m1, a1, j2, m2, a2;
    sscanf(date1, "%d-%d-%d", &j1, &m1, &a1);
    sscanf(date2, "%d-%d-%d", &j2, &m2, &a2);
    
    int total1 = a1 * 365 + m1 * 30 + j1;
    int total2 = a2 * 365 + m2 * 30 + j2;
    
    return total2 - total1;
}

void afficher_entete() {
    clear_screen();
    
    int largeur = obtenir_largeur_console();
    
    printf(COLOR_GREEN);
    afficher_ligne_separatrice('=', largeur);
    printf("\n");
    centrer_texte("   __  __          _ _  _____            _             _");
    centrer_texte("  |  \\/  |        | (_)/ ____|          | |           | |");
    centrer_texte("  | \\  / | ___  __| |_| |     ___  _ __ | |_ _ __ ___ | |");
    centrer_texte("  | |\\/| |/ _ \\/ _` | | |    / _ \\| '_ \\| __| '__/ _ \\| |");
    centrer_texte("  | |  | |  __/ (_| | | |___| (_) | | | | |_| | | (_) | |");
    centrer_texte("  |_|  |_|\\___|\\__,_|_|\\_____\\___/|_| |_|\\__|_|  \\___/|_|");
    printf("\n");
    afficher_ligne_separatrice('=', largeur);
    printf(COLOR_RESET);
}

void attendre_entree() {
    printf(COLOR_YELLOW "\n\nAppuyez sur Entree pour continuer..." COLOR_RESET);
    // Clear input buffer first
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    // Now wait for actual Enter press
    getchar();
}

// Lire mot de passe avec masquage
void lire_mot_de_passe(char *password, int taille) {
    int i = 0;
    char ch;
    
    #ifdef _WIN32
        #include <conio.h>
        while (1) {
            ch = _getch();
            
            if (ch == '\r' || ch == '\n') { // Enter
                password[i] = '\0';
                printf("\n");
                break;
            } else if (ch == '\b' && i > 0) { // Backspace
                i--;
                printf("\b \b");
            } else if (ch != '\b' && i < taille - 1) {
                password[i++] = ch;
                printf("*");
            }
        }
    #else
        // Pour Linux/Mac - desactiver echo
        system("stty -echo");
        fgets(password, taille, stdin);
        password[strcspn(password, "\n")] = 0;
        system("stty echo");
        printf("\n");
    #endif
}

// Afficher entete complet avec user et role
void afficher_entete_complet(const char *user, const char *role) {
    clear_screen();

    int largeur = obtenir_largeur_console();
    char date[11];
    obtenir_date_du_jour(date);

    printf(COLOR_GREEN);
    afficher_ligne_separatrice('=', largeur);

    centrer_texte("   __  __          _ _  _____            _             _");
    centrer_texte("  |  \\/  |        | (_)/ ____|          | |           | |");
    centrer_texte("  | \\  / | ___  __| |_| |     ___  _ __ | |_ _ __ ___ | |");
    centrer_texte("  | |\\/| |/ _ \\/ _` | | |    / _ \\| '_ \\| __| '__/ _ \\| |");
    centrer_texte("  | |  | |  __/ (_| | | |___| (_) | | | | |_| | | (_) | |");
    centrer_texte("  |_|  |_|\\___|\\__,_|_|\\_____\\___/|_| |_|\\__|_|  \\___/|_|");

    afficher_ligne_separatrice('=', largeur);
    printf(COLOR_RESET);

    // Format and center user/role info
    char user_info[100];
    snprintf(user_info, sizeof(user_info), "User: %-20s Role: %s", user, role);
    centrer_texte(user_info);
    
    char date_info[50];
    snprintf(date_info, sizeof(date_info), "Date: %s", date);
    centrer_texte(date_info);

    afficher_ligne_separatrice('-', largeur);
}

// Afficher pied de page
void afficher_pied_page() {
    int largeur = obtenir_largeur_console();

    afficher_ligne_separatrice('-', largeur);
    centrer_texte("[0] Help    [9] Retour    [00] Exit");
    afficher_ligne_separatrice('=', largeur);
}
