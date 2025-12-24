#include "fonctions.h"

void charger_employes(Employe employes[], int *count) {
    FILE *file = fopen("bin/data/employes.txt", "r");
    *count = 0;
    
    if (file == NULL) {
        file = fopen("bin/data/employes.txt", "w");
        if (file == NULL) return;
        
        fprintf(file, "1|Bennani|Yassir|Vendeur|15-01-2023|0612345678\n");
        fprintf(file, "2|Alaoui|Karim|Vendeur|20-03-2023|0623456789\n");
        fprintf(file, "3|Zahra|Fatima|Caissiere|10-05-2023|0634567890\n");
        
        fclose(file);
        file = fopen("bin/data/employes.txt", "r");
    }
    
    char ligne[300];
    while (fgets(ligne, sizeof(ligne), file) && *count < MAX_EMPLOYES) {
        // Remove newline
        ligne[strcspn(ligne, "\n")] = 0;
        
        // Skip empty lines
        if (strlen(ligne) == 0) continue;
        
        // Parse line manually to handle empty telephone field
        int id;
        char nom[50], prenom[50], poste[50], date[11], tel[15];
        
        // Reset tel to empty
        tel[0] = '\0';
        
        // Try to parse with telephone
        int fields = sscanf(ligne, "%d|%49[^|]|%49[^|]|%49[^|]|%10[^|]|%14[^\n]",
                           &id, nom, prenom, poste, date, tel);
        
        // If 5 fields (no telephone), that's still valid
        if (fields >= 5) {
            employes[*count].id = id;
            strcpy(employes[*count].nom, nom);
            strcpy(employes[*count].prenom, prenom);
            strcpy(employes[*count].poste, poste);
            strcpy(employes[*count].date_embauche, date);
            strcpy(employes[*count].telephone, tel);
            (*count)++;
        }
    }
    
    fclose(file);
}

void sauvegarder_employes(Employe employes[], int count) {
    FILE *file = fopen("bin/data/employes.txt", "w");
    
    if (file == NULL) {
        printf(COLOR_RED "\nErreur : Impossible de sauvegarder les employes.\n" COLOR_RESET);
        return;
    }
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d|%s|%s|%s|%s|%s\n",
                employes[i].id,
                employes[i].nom,
                employes[i].prenom,
                employes[i].poste,
                employes[i].date_embauche,
                employes[i].telephone);
    }
    
    fclose(file);
}

void ajouter_employe(Employe employes[], int *count) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== AJOUTER UN EMPLOYE ===\n\n" COLOR_RESET);
    
    if (*count >= MAX_EMPLOYES) {
        printf(COLOR_RED "\t\tLa liste des employes est pleine !\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    Employe nouveau;
    
    printf(COLOR_WHITE "\t\tID : " COLOR_RESET);
    nouveau.id = lire_entier();
    
    // Verifier si l'ID existe deja
    for (int i = 0; i < *count; i++) {
        if (employes[i].id == nouveau.id) {
            printf(COLOR_RED "\t\tCet ID existe deja !\n" COLOR_RESET);
            attendre_entree();
            return;
        }
    }
    
    printf(COLOR_WHITE "\t\tNom : " COLOR_RESET);
    lire_chaine(nouveau.nom, 50);
    
    printf(COLOR_WHITE "\t\tPrenom : " COLOR_RESET);
    lire_chaine(nouveau.prenom, 50);
    
    printf(COLOR_WHITE "\t\tPoste (1.Vendeur 2.Caissier 3.Pharmacien 4.Autre) : " COLOR_RESET);
    int poste_choix = lire_entier();
    
    const char *postes[] = {"Vendeur", "Caissier", "Pharmacien", "Autre"};
    if (poste_choix >= 1 && poste_choix <= 3) {
        strcpy(nouveau.poste, postes[poste_choix - 1]);
    } else if (poste_choix == 4) {
        printf(COLOR_WHITE "\t\tEntrez le poste personnalise : " COLOR_RESET);
        lire_chaine(nouveau.poste, 50);
    } else {
        strcpy(nouveau.poste, "Autre");
    }
    
    printf(COLOR_WHITE "\t\tDate d'embauche (JJ-MM-AAAA) : " COLOR_RESET);
    lire_chaine(nouveau.date_embauche, 11);
    while (!valider_date(nouveau.date_embauche)) {
        printf(COLOR_RED "\t\tDate invalide. Reessayez (JJ-MM-AAAA) : " COLOR_RESET);
        lire_chaine(nouveau.date_embauche, 11);
    }
    
    printf(COLOR_WHITE "\t\tTelephone : " COLOR_RESET);
    lire_chaine(nouveau.telephone, 15);
    
    employes[*count] = nouveau;
    (*count)++;
    
    sauvegarder_employes(employes, *count);
    
    printf(COLOR_GREEN "\n\t\tEmploye ajoute avec succes !\n" COLOR_RESET);
    attendre_entree();
}

void modifier_employe(Employe employes[], int count) {
    int id, index = -1;
    
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== MODIFIER UN EMPLOYE ===\n\n" COLOR_RESET);
    
    printf(COLOR_WHITE "\t\tID de l'employe a modifier : " COLOR_RESET);
    id = lire_entier();
    
    // Trouver l'employe
    for (int i = 0; i < count; i++) {
        if (employes[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        printf(COLOR_RED "\t\tEmploye non trouve !\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    printf(COLOR_YELLOW "\n\t\tEmploye trouve : %s %s (%s)\n\n" COLOR_RESET, 
           employes[index].prenom, employes[index].nom, employes[index].poste);
    
    printf(COLOR_WHITE "\t\tNouveau nom (laisser vide pour ne pas modifier) : " COLOR_RESET);
    char buffer[50];
    lire_chaine(buffer, 50);
    if (strlen(buffer) > 0) {
        strcpy(employes[index].nom, buffer);
    }
    
    printf(COLOR_WHITE "\t\tNouveau prenom (laisser vide pour ne pas modifier) : " COLOR_RESET);
    lire_chaine(buffer, 50);
    if (strlen(buffer) > 0) {
        strcpy(employes[index].prenom, buffer);
    }
    
    printf(COLOR_WHITE "\t\tNouveau poste (1.Vendeur 2.Caissier 3.Pharmacien 4.Autre, 0 pour ne pas modifier) : " COLOR_RESET);
    int poste_choix = lire_entier();
    if (poste_choix >= 1 && poste_choix <= 3) {
        const char *postes[] = {"Vendeur", "Caissier", "Pharmacien", "Autre"};
        strcpy(employes[index].poste, postes[poste_choix - 1]);
    } else if (poste_choix == 4) {
        printf(COLOR_WHITE "\t\tEntrez le poste personnalise : " COLOR_RESET);
        lire_chaine(employes[index].poste, 50);
    }
    
    printf(COLOR_WHITE "\t\tNouveau telephone (laisser vide pour ne pas modifier) : " COLOR_RESET);
    lire_chaine(buffer, 15);
    if (strlen(buffer) > 0) {
        strcpy(employes[index].telephone, buffer);
    }
    
    sauvegarder_employes(employes, count);
    
    printf(COLOR_GREEN "\n\t\tEmploye modifie avec succes !\n" COLOR_RESET);
    attendre_entree();
}

void supprimer_employe(Employe employes[], int *count) {
    int id, index = -1;
    
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== SUPPRIMER UN EMPLOYE ===\n\n" COLOR_RESET);
    
    printf(COLOR_WHITE "\t\tID de l'employe a supprimer : " COLOR_RESET);
    id = lire_entier();
    
    // Trouver l'employe
    for (int i = 0; i < *count; i++) {
        if (employes[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        printf(COLOR_RED "\t\tEmploye non trouve !\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    printf(COLOR_RED "\n\t\tVous allez supprimer : %s %s (%s)\n" COLOR_RESET, 
           employes[index].prenom, employes[index].nom, employes[index].poste);
    
    printf(COLOR_WHITE "\t\tConfirmer la suppression (oui/non) : " COLOR_RESET);
    char confirmation[10];
    lire_chaine(confirmation, 10);
    
    if (strcmp(confirmation, "oui") == 0 || strcmp(confirmation, "OUI") == 0) {
        // Decaler les elements
        for (int i = index; i < *count - 1; i++) {
            employes[i] = employes[i + 1];
        }
        
        (*count)--;
        sauvegarder_employes(employes, *count);
        
        printf(COLOR_GREEN "\n\t\tEmploye supprime avec succes !\n" COLOR_RESET);
    } else {
        printf(COLOR_YELLOW "\n\t\tSuppression annulee.\n" COLOR_RESET);
    }
    
    attendre_entree();
}

void afficher_liste_employes(Employe employes[], int count) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== LISTE DES EMPLOYES ===\n\n" COLOR_RESET);
    
    if (count == 0) {
        printf(COLOR_YELLOW "\t\tAucun employe enregistre.\n\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    printf(COLOR_GREEN "ID | Nom             | Prenom          | Poste       | Date embauche | Telephone\n" COLOR_RESET);
    printf("---+-----------------+-----------------+-------------+---------------+--------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-2d | %-15s | %-15s | %-11s | %-13s | %s\n",
               employes[i].id,
               employes[i].nom,
               employes[i].prenom,
               employes[i].poste,
               employes[i].date_embauche,
               employes[i].telephone);
    }
    
    printf("\n");
    attendre_entree();
}

void envoyer_message_employe() {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== ENVOYER UN MESSAGE ===\n\n" COLOR_RESET);
    
    printf(COLOR_WHITE "\t\tDestinataire (1.Tous 2.Vendeurs 3.Caissiers 4.Managers) : " COLOR_RESET);
    int destinataire = lire_entier();
    
    printf(COLOR_WHITE "\t\tSujet du message : " COLOR_RESET);
    char sujet[100];
    lire_chaine(sujet, 100);
    
    printf(COLOR_WHITE "\t\tMessage :\n\t\t" COLOR_RESET);
    char message[500];
    lire_chaine(message, 500);
    
    printf(COLOR_GREEN "\n\t\tMessage envoye avec succes !\n" COLOR_RESET);
    
    // Simuler l'enregistrement du message
    FILE *file = fopen("bin/data/messages.txt", "a");
    if (file != NULL) {
        char date[11];
        obtenir_date_du_jour(date);
        fprintf(file, "%s|%d|%s|%s\n", date, destinataire, sujet, message);
        fclose(file);
    }
    
    attendre_entree();
}

void afficher_messages_employe(const char *prenom_employe) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN);
    centrer_texte("");
    centrer_texte("=== MES MESSAGES ===");
    centrer_texte("");
    printf(COLOR_RESET);
    
    // Get employee's position
    Employe employes[MAX_EMPLOYES];
    int emp_count = 0;
    charger_employes(employes, &emp_count);
    
    char employee_position[50] = "";
    for (int i = 0; i < emp_count; i++) {
        if (strcasecmp_custom(employes[i].prenom, prenom_employe) == 0) {
            strcpy(employee_position, employes[i].poste);
            break;
        }
    }
    
    FILE *file = fopen("bin/data/messages.txt", "r");
    if (file == NULL) {
        printf(COLOR_YELLOW);
        centrer_texte("Aucun message.");
        printf(COLOR_RESET);
        attendre_entree();
        return;
    }
    
    // Get today's date for comparison
    char today[11];
    obtenir_date_du_jour(today);
    
    // Temporary storage for valid messages
    typedef struct {
        char date[11];
        int destinataire;
        char sujet[100];
        char message[500];
    } MessageTemp;
    
    MessageTemp messages[100];
    int msg_count = 0;
    int valides_count = 0;
    
    char date[11];
    int destinataire;
    char sujet[100], message[500];
    
    // Read all messages
    while (fscanf(file, "%10[^|]|%d|%99[^|]|%499[^\n]\n", date, &destinataire, sujet, message) == 4) {
        if (msg_count < 100) {
            strcpy(messages[msg_count].date, date);
            messages[msg_count].destinataire = destinataire;
            strcpy(messages[msg_count].sujet, sujet);
            strcpy(messages[msg_count].message, message);
            msg_count++;
        }
    }
    fclose(file);
    
    // Determine if message is for this employee based on position
    // 1 = Tous, 2 = Vendeurs, 3 = Caissiers, 4 = Managers
    int trouve = 0;
    printf("\n");
    
    for (int i = 0; i < msg_count; i++) {
        // Check if message is from today
        int est_aujourdhui = (strcmp(messages[i].date, today) == 0);
        
        int afficher_ce_message = 0;
        if (messages[i].destinataire == 1) {  // Pour tous
            afficher_ce_message = 1;
        } else if (messages[i].destinataire == 2 && strcasecmp_custom(employee_position, "Vendeur") == 0) {
            afficher_ce_message = 1;
        } else if (messages[i].destinataire == 3 && strcasecmp_custom(employee_position, "Caissier") == 0) {
            afficher_ce_message = 1;
        } else if (messages[i].destinataire == 4 && strcasecmp_custom(employee_position, "Manager") == 0) {
            afficher_ce_message = 1;
        }
        
        if (afficher_ce_message) {
            printf(COLOR_GREEN "Date: %s\n" COLOR_RESET, messages[i].date);
            printf(COLOR_YELLOW "Sujet: %s\n" COLOR_RESET, messages[i].sujet);
            printf("Message: %s\n", messages[i].message);
            
            if (est_aujourdhui) {
                printf(COLOR_CYAN "(Nouveau message)\n" COLOR_RESET);
            } else {
                printf(COLOR_RED "(Ce message sera supprime)\n" COLOR_RESET);
            }
            
            printf("-----------------------------------\n");
            trouve = 1;
        }
        
        // Keep only today's messages
        if (est_aujourdhui) {
            valides_count++;
        }
    }
    
    if (!trouve) {
        printf(COLOR_YELLOW);
        centrer_texte("Aucun message pour vous.");
        printf(COLOR_RESET);
    }
    
    // Rewrite file with only today's messages
    file = fopen("bin/data/messages.txt", "w");
    if (file != NULL) {
        for (int i = 0; i < msg_count; i++) {
            if (strcmp(messages[i].date, today) == 0) {
                fprintf(file, "%s|%d|%s|%s\n",
                        messages[i].date,
                        messages[i].destinataire,
                        messages[i].sujet,
                        messages[i].message);
            }
        }
        fclose(file);
    }
    
    printf("\n");
    attendre_entree();
}

void menu_gerer_employes(Employe employes[], int *count, const char *user) {
    int choix;
    
    (void)user; // Unused for now
    
    do {
        clear_screen();
        afficher_entete();
        
        printf(COLOR_CYAN);
        centrer_texte("");
        centrer_texte("=== GERER LES EMPLOYES ===");
        centrer_texte("");
        printf(COLOR_RESET);
        
        printf(COLOR_GREEN);
        centrer_texte("[1] Ajouter un employe");
        printf(COLOR_RESET);
        printf(COLOR_YELLOW);
        centrer_texte("[2] Modifier un employe");
        printf(COLOR_RESET);
        printf(COLOR_RED);
        centrer_texte("[3] Supprimer un employe");
        printf(COLOR_RESET);
        printf(COLOR_GREEN);
        centrer_texte("[4] Liste des employes");
        printf(COLOR_RESET);
        printf(COLOR_BLUE);
        centrer_texte("[5] Envoyer un message");
        printf(COLOR_RESET);
        printf(COLOR_CYAN);
        centrer_texte("[6] Retour au menu principal");
        centrer_texte("");
        printf(COLOR_RESET);
        
        printf(COLOR_WHITE);
        centrer_texte_sans_newline("Votre choix : ");
        printf(COLOR_RESET);
        
        choix = lire_entier();
        
        switch (choix) {
            case 1:
                ajouter_employe(employes, count);
                break;
            case 2:
                modifier_employe(employes, *count);
                break;
            case 3:
                supprimer_employe(employes, count);
                break;
            case 4:
                afficher_liste_employes(employes, *count);
                break;
            case 5:
                envoyer_message_employe();
                break;
            case 6:
                return;
            default:
                printf(COLOR_RED "\n\t\tChoix invalide !\n" COLOR_RESET);
                attendre_entree();
                break;
        }
    } while (choix != 6);
}
