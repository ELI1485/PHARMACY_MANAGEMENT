#include "fonctions.h"

void afficher_profil(Admin admin) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== MON PROFIL ===\n\n" COLOR_RESET);
    
    printf(COLOR_GREEN "\t\tInformations du compte :\n\n" COLOR_RESET);
    printf("\t\t• ID : %d\n", admin.id);
    printf("\t\t• Nom : %s\n", admin.nom);
    printf("\t\t• Role : Administrateur\n");
    
    // Lire la date de création depuis un fichier si disponible
    FILE *info_file = fopen("bin/data/admin_info.txt", "r");
    if (info_file) {
        char ligne[100];
        while (fgets(ligne, sizeof(ligne), info_file)) {
            if (strstr(ligne, "Date creation")) {
                printf("\t\t• %s", ligne);
            } else if (strstr(ligne, "Derniere connexion")) {
                printf("\t\t• %s", ligne);
            }
        }
        fclose(info_file);
    } else {
        printf("\t\t• Derniere connexion : Aujourd'hui\n");
        printf("\t\t• Compte cree le : 01-01-2024\n");
    }
    
    printf(COLOR_YELLOW "\n\t\tStatistiques personnelles :\n\n" COLOR_RESET);
    
    // Get current month-year for comparison
    char today[11];
    obtenir_date_du_jour(today);
    
    char mois_courant[8];  // MM-YYYY
    if (strlen(today) >= 10) {
        snprintf(mois_courant, sizeof(mois_courant), "%c%c-%c%c%c%c", 
                 today[3], today[4], today[6], today[7], today[8], today[9]);
    }
    
    // Count logins this month from user_logins.txt filtering by "Admin"
    int connexions_mois = 0;
    FILE* login_file = fopen("bin/data/user_logins.txt", "r");
    if (login_file) {
        char ligne[100];
        int id;
        char type[20], datetime[20];
        
        while (fscanf(login_file, "%d|%19[^|]|%19[^\n]\n", &id, type, datetime) == 3) {
            // Check if login is for this admin ID and type is "Admin"
            if (id == admin.id && strcmp(type, "Admin") == 0) {
                // Extract month-year from datetime (DD-MM-YYYY HH:MM:SS)
                if (strlen(datetime) >= 10) {
                    char log_mois[8];
                    snprintf(log_mois, sizeof(log_mois), "%c%c-%c%c%c%c",
                             datetime[3], datetime[4], datetime[6], datetime[7], datetime[8], datetime[9]);
                    if (strcmp(log_mois, mois_courant) == 0) {
                        connexions_mois++;
                    }
                }
            }
        }
        fclose(login_file);
    }
    
    printf(COLOR_GREEN "\t\t• Nombre de connexions ce mois : %d\n" COLOR_RESET, connexions_mois);
    
    printf("\n");
    attendre_entree();
}

void changer_mot_de_passe(Admin *admin) {
    char ancien_mdp[30];
    char nouveau_mdp[30];
    char confirmation[30];
    
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== CHANGER LE MOT DE PASSE ===\n\n" COLOR_RESET);
    
    printf(COLOR_WHITE "\t\tAncien mot de passe : " COLOR_RESET);
    lire_chaine(ancien_mdp, 30);
    
    if (strcmp(ancien_mdp, admin->mot_de_passe) != 0) {
        printf(COLOR_RED "\n\t\tMot de passe incorrect !\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    printf(COLOR_WHITE "\t\tNouveau mot de passe (min. 6 caracteres) : " COLOR_RESET);
    lire_chaine(nouveau_mdp, 30);
    
    if (strlen(nouveau_mdp) < 6) {
        printf(COLOR_RED "\n\t\tLe mot de passe doit contenir au moins 6 caracteres !\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    printf(COLOR_WHITE "\t\tConfirmer le nouveau mot de passe : " COLOR_RESET);
    lire_chaine(confirmation, 30);
    
    if (strcmp(nouveau_mdp, confirmation) != 0) {
        printf(COLOR_RED "\n\t\tLes mots de passe ne correspondent pas !\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    // Sauvegarder l'ancien mot de passe (pour l'historique)
    char ancien_mdp_copie[30];
    strcpy(ancien_mdp_copie, admin->mot_de_passe);
    
    // Mettre a jour le mot de passe
    strcpy(admin->mot_de_passe, nouveau_mdp);
    
    // Sauvegarder dans le fichier binaire
    int succes = 0;
    FILE *file = fopen("bin/data/admins.dat", "r+b");
    if (file != NULL) {
        Admin temp;
        while (!feof(file)) {
            long position = ftell(file); // Sauvegarder la position actuelle
            size_t lus = fread(&temp, sizeof(Admin), 1, file);
            
            if (lus == 1 && temp.id == admin->id) {
                // Retourner a la position de cet enregistrement
                fseek(file, position, SEEK_SET);
                fwrite(admin, sizeof(Admin), 1, file);
                succes = 1;
                break;
            }
        }
        fclose(file);
    }
    
    if (!succes) {
        // Si la mise a jour a echoue, restaurer l'ancien mot de passe
        strcpy(admin->mot_de_passe, ancien_mdp_copie);
        printf(COLOR_RED "\n\t\tErreur lors de la mise a jour du mot de passe !\n" COLOR_RESET);
    } else {
        printf(COLOR_GREEN "\n\t\tMot de passe change avec succes !\n" COLOR_RESET);
        
        // Enregistrer le changement dans un fichier journal
        FILE *log = fopen("bin/data/password_changes.log", "a");
        if (log != NULL) {
            char date[20];
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            strftime(date, 20, "%d-%m-%Y %H:%M:%S", tm_info);
            fprintf(log, "[%s] Admin ID %d a change son mot de passe\n", date, admin->id);
            fclose(log);
        }
    }
    
    attendre_entree();
}

// Fonction alternative utilisant un fichier temporaire
void changer_mot_de_passe_alternative(Admin *admin) {
    char ancien_mdp[30];
    char nouveau_mdp[30];
    char confirmation[30];
    
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== CHANGER LE MOT DE PASSE ===\n\n" COLOR_RESET);
    
    printf(COLOR_WHITE "\t\tAncien mot de passe : " COLOR_RESET);
    lire_chaine(ancien_mdp, 30);
    
    if (strcmp(ancien_mdp, admin->mot_de_passe) != 0) {
        printf(COLOR_RED "\n\t\tMot de passe incorrect !\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    printf(COLOR_WHITE "\t\tNouveau mot de passe (min. 6 caracteres) : " COLOR_RESET);
    lire_chaine(nouveau_mdp, 30);
    
    if (strlen(nouveau_mdp) < 6) {
        printf(COLOR_RED "\n\t\tLe mot de passe doit contenir au moins 6 caracteres !\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    printf(COLOR_WHITE "\t\tConfirmer le nouveau mot de passe : " COLOR_RESET);
    lire_chaine(confirmation, 30);
    
    if (strcmp(nouveau_mdp, confirmation) != 0) {
        printf(COLOR_RED "\n\t\tLes mots de passe ne correspondent pas !\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    // Methode alternative : utiliser un fichier temporaire
    FILE *file = fopen("bin/data/admins.dat", "rb");
    FILE *temp_file = fopen("bin/data/admins_temp.dat", "wb");
    
    if (file == NULL || temp_file == NULL) {
        printf(COLOR_RED "\n\t\tErreur d'acces aux fichiers !\n" COLOR_RESET);
        if (file) fclose(file);
        if (temp_file) fclose(temp_file);
        attendre_entree();
        return;
    }
    
    Admin temp;
    int trouve = 0;
    
    while (fread(&temp, sizeof(Admin), 1, file)) {
        if (temp.id == admin->id) {
            // Mettre a jour le mot de passe pour cet admin
            strcpy(temp.mot_de_passe, nouveau_mdp);
            strcpy(admin->mot_de_passe, nouveau_mdp); // Mettre a jour en memoire aussi
            trouve = 1;
        }
        fwrite(&temp, sizeof(Admin), 1, temp_file);
    }
    
    fclose(file);
    fclose(temp_file);
    
    if (trouve) {
        // Remplacer l'ancien fichier par le nouveau
        remove("admins.dat");
        rename("admins_temp.dat", "admins.dat");
        
        printf(COLOR_GREEN "\n\t\tMot de passe change avec succes !\n" COLOR_RESET);
        
        // Enregistrer dans le journal
        FILE *log = fopen("bin/data/security.log", "a");
        if (log) {
            char date[20];
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            strftime(date, 20, "%Y-%m-%d %H:%M:%S", tm_info);
            fprintf(log, "%s - Password changed for admin ID: %d\n", date, admin->id);
            fclose(log);
        }
    } else {
        remove("admins_temp.dat");
        printf(COLOR_RED "\n\t\tAdmin non trouve dans le fichier !\n" COLOR_RESET);
    }
    
    attendre_entree();
}

// Fonction pour changer le nom de l'admin
void changer_nom_admin(Admin *admin) {
    char nouveau_nom[50];
    
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== CHANGER LE NOM ===\n\n" COLOR_RESET);
    
    printf(COLOR_WHITE "\t\tNom actuel : %s\n\n", admin->nom);
    printf("\t\tNouveau nom : " COLOR_RESET);
    lire_chaine(nouveau_nom, 50);
    
    if (strlen(nouveau_nom) < 3) {
        printf(COLOR_RED "\n\t\tLe nom doit contenir au moins 3 caracteres !\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    // Sauvegarder l'ancien nom
    char ancien_nom[50];
    strcpy(ancien_nom, admin->nom);
    
    // Mettre a jour le nom en memoire
    strcpy(admin->nom, nouveau_nom);
    
    // Sauvegarder dans le fichier binaire via fichier temporaire
    FILE *file = fopen("bin/data/admins.dat", "rb");
    FILE *temp_file = fopen("bin/data/admins_temp.dat", "wb");
    
    if (file == NULL || temp_file == NULL) {
        strcpy(admin->nom, ancien_nom);  // Restaurer
        printf(COLOR_RED "\n\t\tErreur d'acces aux fichiers !\n" COLOR_RESET);
        if (file) fclose(file);
        if (temp_file) fclose(temp_file);
        attendre_entree();
        return;
    }
    
    Admin temp;
    int trouve = 0;
    
    while (fread(&temp, sizeof(Admin), 1, file)) {
        if (temp.id == admin->id) {
            strcpy(temp.nom, nouveau_nom);
            trouve = 1;
        }
        fwrite(&temp, sizeof(Admin), 1, temp_file);
    }
    
    fclose(file);
    fclose(temp_file);
    
    if (trouve) {
        remove("bin/data/admins.dat");
        rename("bin/data/admins_temp.dat", "bin/data/admins.dat");
        
        printf(COLOR_GREEN "\n\t\tNom change avec succes : %s -> %s\n" COLOR_RESET, 
               ancien_nom, nouveau_nom);
        
        // Logger le changement
        FILE *log = fopen("bin/data/security.log", "a");
        if (log) {
            char date[20];
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            strftime(date, 20, "%Y-%m-%d %H:%M:%S", tm_info);
            fprintf(log, "%s - Name changed for admin ID %d: %s -> %s\n", 
                    date, admin->id, ancien_nom, nouveau_nom);
            fclose(log);
        }
    } else {
        remove("admins_temp.dat");
        strcpy(admin->nom, ancien_nom);  // Restaurer
        printf(COLOR_RED "\n\t\tAdmin non trouve dans le fichier !\n" COLOR_RESET);
    }
    
    attendre_entree();
}

void sauvegarder_profil(Admin admin) {

    // Cette fonction pourrait etre utilisee pour sauvegarder d'autres infos du profil
    FILE *info = fopen("bin/data/admin_info.txt", "a");
    if (info) {
        char date[20];
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        strftime(date, 20, "%d-%m-%Y %H:%M", tm_info);
        
        fprintf(info, "Derniere connexion: %s - Admin: %s (ID: %d)\n", 
                date, admin.nom, admin.id);
        fclose(info);
    }
}

void menu_profil(Admin *admin) {
    int choix;
    
    do {
        clear_screen();
        afficher_entete();
        
        printf(COLOR_CYAN);
        centrer_texte("");
        centrer_texte("=== MON PROFIL ===");
        centrer_texte("");
        printf(COLOR_RESET);
        printf(COLOR_WHITE);
        centrer_texte("[1] Voir mon profil");
        printf(COLOR_RESET);
        printf(COLOR_YELLOW);
        centrer_texte("[2] Changer le mot de passe");
        printf(COLOR_RESET);
        printf(COLOR_GREEN);
        centrer_texte("[3] Changer le nom");
        printf(COLOR_RESET);
        printf(COLOR_BLUE);
        centrer_texte("[4] Retour au menu principal");
        centrer_texte("");
        printf(COLOR_RESET);
        printf(COLOR_WHITE);
        centrer_texte_sans_newline("Votre choix : ");
        printf(COLOR_RESET);
        
        choix = lire_entier();
        
        switch (choix) {
            case 1:
                afficher_profil(*admin);
                break;
            case 2:
                changer_mot_de_passe_alternative(admin);
                break;
            case 3:
                changer_nom_admin(admin);
                break;
            case 4:
                return;
            default:
                printf(COLOR_RED "\n\t\tChoix invalide !\n" COLOR_RESET);
                attendre_entree();
                break;
        }
    } while (choix != 4);
}
