#include "fonctions.h"

int afficher_menu_initial() {
    int choix;
    
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN);
    centrer_texte("");
    centrer_texte("=== PAGE DE SELECTION ===");
    centrer_texte("");
    printf(COLOR_RESET);
    
    printf(COLOR_GREEN);
    centrer_texte("1. Pharmacien (Admin)");
    printf(COLOR_RESET);
    
    printf(COLOR_YELLOW);
    centrer_texte("2. Employe");
    printf(COLOR_RESET);
    
    printf(COLOR_RED);
    centrer_texte("0. Quitter");
    centrer_texte("");
    printf(COLOR_RESET);
    
    printf(COLOR_WHITE);
    centrer_texte_sans_newline("Votre choix : ");
    printf(COLOR_RESET);
    
    choix = lire_entier();
    
    while (choix < 0 || choix > 2) {
        printf(COLOR_RED);
        centrer_texte_sans_newline("Choix invalide. Veuillez reessayer : ");
        printf(COLOR_RESET);
        choix = lire_entier();
    }
    
    return choix;
}

int login_admin(Admin *admin_connecte, int *attempted_id) {
    FILE *file;
    Admin admin;
    int id;
    char password[30];
    int trouve = 0;
    
    printf(COLOR_CYAN);
    centrer_texte("");
    centrer_texte("=== CONNEXION ADMIN ===");
    centrer_texte("");
    printf(COLOR_RESET);
    
    printf(COLOR_WHITE);
    centrer_texte_sans_newline("Identifiant : ");
    printf(COLOR_RESET);
    id = lire_entier();
    
    if (attempted_id != NULL) {
        *attempted_id = id;
    }
    
    if (verifier_compte_bloque(id)) {
        printf(COLOR_RED);
        centrer_texte("");
        centrer_texte("Ce compte est bloque !");
        printf(COLOR_RESET);
        printf(COLOR_YELLOW);
        centrer_texte("Pour debloquer, contactez le support");
        centrer_texte("");
        printf(COLOR_RESET);
        attendre_entree();
        return 0;
    }
    
    printf(COLOR_WHITE);
    centrer_texte_sans_newline("Mot de passe : ");
    printf(COLOR_RESET);
    lire_mot_de_passe(password, 30);  // PASSWORD MASKING
    
    file = fopen("data/admins.dat", "rb");
    if (file == NULL) {
        file = fopen("data/admins.dat", "wb");
        if (file == NULL) {
            printf(COLOR_RED "\n\t\t\tErreur de creation du fichier admin.\n" COLOR_RESET);
            return 0;
        }
        
        Admin admin_defaut = {1, "Admin", "admin123"};
        fwrite(&admin_defaut, sizeof(Admin), 1, file);
        fclose(file);
        
        file = fopen("data/admins.dat", "rb");
    }
    
    while (fread(&admin, sizeof(Admin), 1, file)) {
        if (admin.id == id && strcmp(admin.mot_de_passe, password) == 0) {
            *admin_connecte = admin;
            trouve = 1;
            break;
        }
    }
    
    fclose(file);
    
    if (trouve) {
        printf(COLOR_GREEN);
        centrer_texte("");
        centrer_texte("Connexion reussie !");
        printf(COLOR_RESET);
        
        char message[100];
        snprintf(message, sizeof(message), "Bienvenue, %s !", admin_connecte->nom);
        printf(COLOR_YELLOW);
        centrer_texte(message);
        centrer_texte("");
        printf(COLOR_RESET);
        
        log_user_login(admin_connecte->id, "Admin");
        attendre_entree();
        return 1;
    }
    
    return 0;
}

void afficher_message_erreur_support() {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_RED "\n\n\t\t\t=== ERREUR DE CONNEXION ===\n\n" COLOR_RESET);
    printf("\t\t\tVeuillez contacter support@pharmacie-systeme.com\n\n");
    printf(COLOR_YELLOW "\t\t\t1. Retour a la connexion\n" COLOR_RESET);
    printf(COLOR_RED "\t\t\t0. Quitter\n\n" COLOR_RESET);
    printf(COLOR_WHITE "\t\t\tVotre choix : " COLOR_RESET);
    
    int choix = lire_entier();
    
    if (choix == 0) {
        printf(COLOR_GREEN "\n\n\t\t\tMerci d'avoir utilise MediControl !\n\n" COLOR_RESET);
        exit(0);
    }
}

void bloquer_compte() {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_RED "\n\n\t\t\t=== COMPTE BLOQUE ===\n\n" COLOR_RESET);
    printf("\t\t\tTrop de tentatives de connexion echouees.\n");
    printf("\t\t\tVotre compte est bloque temporairement.\n\n");
    
    printf(COLOR_YELLOW "\t\t\t1. Besoin d'aide ?\n" COLOR_RESET);
    printf(COLOR_RED "\t\t\t0. Quitter\n\n" COLOR_RESET);
    printf(COLOR_WHITE "\t\t\tVotre choix : " COLOR_RESET);
    
    int choix = lire_entier();
    
    if (choix == 1) {
        clear_screen();
        afficher_entete();
        
        printf(COLOR_YELLOW "\n\n\t\t\t=== SUPPORT TECHNIQUE ===\n\n" COLOR_RESET);
        printf("\t\t\tVeuillez contacter support@pharmacie-systeme.com\n");
        printf("\t\t\tpour debloquer votre compte.\n\n");
        attendre_entree();
    } else {
        printf(COLOR_GREEN "\n\n\t\t\tMerci d'avoir utilise MediControl !\n\n" COLOR_RESET);
        exit(0);
    }
}

int verifier_compte_bloque(int id) {
    FILE *file = fopen("data/blocked_admins.txt", "r");
    if (file == NULL) {
        return 0;
    }
    
    int blocked_id;
    while (fscanf(file, "%d\n", &blocked_id) == 1) {
        if (blocked_id == id) {
            fclose(file);
            return 1;
        }
    }
    
    fclose(file);
    return 0;
}

void bloquer_compte_admin(int id) {
    #ifdef _WIN32
        system("if not exist data mkdir data");
    #else
        system("mkdir -p data");
    #endif
    
    FILE *file = fopen("data/blocked_admins.txt", "a");
    if (file != NULL) {
        fprintf(file, "%d\n", id);
        fclose(file);
        
        FILE *log = fopen("data/security.log", "a");
        if (log) {
            char date[20];
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            strftime(date, 20, "%Y-%m-%d %H:%M:%S", tm_info);
            fprintf(log, "%s - Admin ID %d bloque pour tentatives de connexion echouees\n", date, id);
            fclose(log);
        }
    }
}

void debloquer_compte_admin(int id) {
    FILE *file = fopen("data/blocked_admins.txt", "r");
    FILE *temp = fopen("data/blocked_admins_temp.txt", "w");
    
    if (file == NULL || temp == NULL) {
        if (file) fclose(file);
        if (temp) fclose(temp);
        return;
    }
    
    int blocked_id;
    while (fscanf(file, "%d\n", &blocked_id) == 1) {
        if (blocked_id != id) {
            fprintf(temp, "%d\n", blocked_id);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    remove("data/blocked_admins.txt");
    rename("data/blocked_admins_temp.txt", "data/blocked_admins.txt");
    
    FILE *log = fopen("data/security.log", "a");
    if (log) {
        char date[20];
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        strftime(date, 20, "%Y-%m-%d %H:%M:%S", tm_info);
        fprintf(log, "%s - Admin ID %d debloque manuellement\n", date, id);
        fclose(log);
    }
}

// Ne login employe function - moved to employe.c
int login_employe(char *prenom_employe) {
    Employe employes[MAX_EMPLOYES];
    int count = 0;
    charger_employes(employes, &count);
    
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== CONNEXION EMPLOYE ===\n\n" COLOR_RESET);
    printf(COLOR_WHITE "\t\tPrenom : " COLOR_RESET);
    
    char prenom[50];
    lire_chaine(prenom, 50);
    
    for (int i = 0; i < count; i++) {
        if (strcasecmp_custom(employes[i].prenom, prenom) == 0) {
            strcpy(prenom_employe, employes[i].prenom);
            printf(COLOR_GREEN "\n\t\tBienvenue, %s %s !\n\n" COLOR_RESET, 
                   employes[i].prenom, employes[i].nom);
            log_user_login(employes[i].id, "Employe");
            attendre_entree();
            return 1;
        }
    }
    
    printf(COLOR_RED "\n\t\tEmploye introuvable.\n\n" COLOR_RESET);
    attendre_entree();
    return 0;
}