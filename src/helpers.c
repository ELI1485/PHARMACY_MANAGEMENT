#include "fonctions.h"

// Fonction pour comparaison insensible a la casse
int strcasecmp_custom(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = tolower((unsigned char)*s1);
        char c2 = tolower((unsigned char)*s2);
        if (c1 != c2) return c1 - c2;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

// Recherche insensible a la casse
int strstr_case_insensitive(const char *haystack, const char *needle) {
    if (!*needle) return 1;
    
    for (; *haystack; haystack++) {
        const char *h = haystack;
        const char *n = needle;
        
        while (*h && *n && tolower((unsigned char)*h) == tolower((unsigned char)*n)) {
            h++;
            n++;
        }
        
        if (!*n) return 1;
    }
    return 0;
}

// Obtenir la date actuelle
void get_current_date(char *date_str) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    sprintf(date_str, "%02d-%02d-%04d", 
            tm_info->tm_mday, 
            tm_info->tm_mon + 1, 
            tm_info->tm_year + 1900);
}

// Obtenir date et heure actuelles
void get_current_datetime(char *datetime_str) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    sprintf(datetime_str, "%02d-%02d-%04d %02d:%02d:%02d",
            tm_info->tm_mday,
            tm_info->tm_mon + 1,
            tm_info->tm_year + 1900,
            tm_info->tm_hour,
            tm_info->tm_min,
            tm_info->tm_sec);
}

// Verifier si une date est expiree
int is_date_expired(const char *date_str) {
    int jour, mois, annee;
    if (sscanf(date_str, "%d-%d-%d", &jour, &mois, &annee) != 3) {
        return 0; // Format invalide
    }
    
    time_t t = time(NULL);
    struct tm *now = localtime(&t);
    
    int current_year = now->tm_year + 1900;
    int current_month = now->tm_mon + 1;
    int current_day = now->tm_mday;
    
    if (annee < current_year) return 1;
    if (annee > current_year) return 0;
    if (mois < current_month) return 1;
    if (mois > current_month) return 0;
    if (jour < current_day) return 1;
    
    return 0; // Meme jour ou futur
}

// Logger une action utilisateur
void log_user_action(int user_id, const char *user_type, const char *action) {
    FILE *file = fopen("bin/data/user_actions.txt", "a");
    if (!file) {
        #ifdef _WIN32
            system("if not exist bin\\data mkdir bin\\data");
        #else
            system("mkdir -p bin/data");
        #endif
        file = fopen("bin/data/user_actions.txt", "a");
        if (file == NULL) return;
    }
    
    char datetime[20];
    get_current_datetime(datetime);
    
    fprintf(file, "%d|%s|%s|%s\n", user_id, user_type, action, datetime);
    fclose(file);
}

// Logger une connexion
void log_user_login(int user_id, const char *user_type) {
    FILE *file = fopen("bin/data/user_logins.txt", "a");
    if (!file) {
        #ifdef _WIN32
            system("if not exist bin\\data mkdir bin\\data");
        #else
            system("mkdir -p bin/data");
        #endif
        file = fopen("bin/data/user_logins.txt", "a");
        if (file == NULL) return;
    }
    
    char datetime[20];
    get_current_datetime(datetime);
    
    fprintf(file, "%d|%s|%s\n", user_id, user_type, datetime);
    fclose(file);
}

// Charger les categories personnalisees
int charger_categories(Categorie categories[], int max_categories) {
    int count = 0;
    
    // Categories par defaut
    strcpy(categories[count++].nom, "Antibiotiques");
    strcpy(categories[count++].nom, "Analgesiques");
    strcpy(categories[count++].nom, "Anti-inflammatoires");
    
    // Charger les categories personnalisees
    FILE *file = fopen("bin/data/categories.txt", "r");
    if (file != NULL) {
        char ligne[50];
        while (fgets(ligne, sizeof(ligne), file) && count < max_categories) {
            ligne[strcspn(ligne, "\n")] = 0;
            if (strlen(ligne) > 0) {
                // Check for duplicates
                int existe = 0;
                for (int i = 0; i < count; i++) {
                    if (strcasecmp_custom(categories[i].nom, ligne) == 0) {
                        existe = 1;
                        break;
                    }
                }
                
                if (!existe) {
                    strcpy(categories[count++].nom, ligne);
                }
            }
        }
        fclose(file);
    }
    
    return count;
}

// Sauvegarder une nouvelle categorie
void sauvegarder_categorie(const char *nom_categorie) {
    FILE *file = fopen("bin/data/categories.txt", "a");
    if (!file) {
        #ifdef _WIN32
            system("if not exist bin\\data mkdir bin\\data");
        #else
            system("mkdir -p bin/data");
        #endif
        file = fopen("bin/data/categories.txt", "a");
        if (file == NULL) return;
    }
    
    fprintf(file, "%s\n", nom_categorie);
    fclose(file);
}
