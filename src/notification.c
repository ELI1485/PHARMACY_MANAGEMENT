#include "fonctions.h"

void verifier_ruptures(Medicament* liste) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_RED "\n\n\t\t=== RUPTURES DE STOCK ===\n\n" COLOR_RESET);
    
    int rupture_trouvee = 0;
    Medicament* courant = liste;
    while (courant) {
        if (courant->quantite == 0) {
            printf(COLOR_RED "\t\t• %s – 0 unite\n" COLOR_RESET, courant->nom);
            rupture_trouvee = 1;
        }
        courant = courant->next;
    }
    
    if (!rupture_trouvee) {
        printf(COLOR_GREEN "\t\tAucune rupture de stock.\n" COLOR_RESET);
    }
    
    printf("\n");
    attendre_entree();
}

void verifier_stocks_faibles(Medicament* liste) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_YELLOW "\n\n\t\t=== STOCKS FAIBLES ===\n\n" COLOR_RESET);
    
    int faible_trouve = 0;
    Medicament* courant = liste;
    while (courant) {
        if (courant->quantite > 0 && courant->quantite <= 10) {
            printf(COLOR_YELLOW "\t\t• %s – %d unites\n" COLOR_RESET, 
                   courant->nom, courant->quantite);
            faible_trouve = 1;
        }
        courant = courant->next;
    }
    
    if (!faible_trouve) {
        printf(COLOR_GREEN "\t\tAucun stock faible.\n" COLOR_RESET);
    }
    
    printf("\n");
    attendre_entree();
}

void verifier_commandes_en_retard(Commande commandes[], int count) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_RED "\n\n\t\t=== COMMANDES EN RETARD ===\n\n" COLOR_RESET);
    
    char date_aujourdhui[11];
    obtenir_date_du_jour(date_aujourdhui);
    
    int retard_trouve = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(commandes[i].statut, "En attente") == 0) {
            int jours = jours_entre_dates(commandes[i].date_commande, date_aujourdhui);
            if (jours > 4) { // Considere en retard apres 4 jours
                printf(COLOR_RED "\t\t• %s – en attente depuis %d jours\n" COLOR_RESET,
                       commandes[i].idC, jours);
                retard_trouve = 1;
            }
        }
    }
    
    if (!retard_trouve) {
        printf(COLOR_GREEN "\t\tAucune commande en retard.\n" COLOR_RESET);
    }
    
    printf("\n");
    attendre_entree();
}

void verifier_medicaments_perimes(Medicament* liste) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_RED "\n\n\t\t=== MEDICAMENTS PERIMES ===\n\n" COLOR_RESET);
    
    char date_aujourdhui[11];
    obtenir_date_du_jour(date_aujourdhui);
    
    int perime_trouve = 0;
    Medicament* courant = liste;
    while (courant) {
        if (comparer_dates(courant->date_expiration, date_aujourdhui) < 0) {
            printf(COLOR_RED "\t\t• %s – expire le %s\n" COLOR_RESET,
                   courant->nom, courant->date_expiration);
            perime_trouve = 1;
        }
        courant = courant->next;
    }
    
    if (!perime_trouve) {
        printf(COLOR_GREEN "\t\tAucun medicament perime.\n" COLOR_RESET);
    }
    
    printf("\n");
    attendre_entree();
}

void afficher_notifications(Medicament* liste, Commande commandes[], int c_count) {
    int choix;
    
    do {
        clear_screen();
        afficher_entete();
        
        printf(COLOR_CYAN "\n\n\t\t=== NOTIFICATIONS ===\n\n" COLOR_RESET);
        
        // Apercu des notifications
        int ruptures = 0, faibles = 0, retards = 0, perimes = 0;
        char date_aujourdhui[11];
        obtenir_date_du_jour(date_aujourdhui);
        
        Medicament* courant = liste;
        while (courant) {
            if (courant->quantite == 0) ruptures++;
            if (courant->quantite > 0 && courant->quantite <= 10) faibles++;
            if (comparer_dates(courant->date_expiration, date_aujourdhui) < 0) perimes++;
            courant = courant->next;
        }
        
        for (int i = 0; i < c_count; i++) {
            if (strcmp(commandes[i].statut, "En attente") == 0) {
                int jours = jours_entre_dates(commandes[i].date_commande, date_aujourdhui);
                if (jours > 4) retards++;
            }
        }
        
        printf(COLOR_RED "\t\tRuptures de stock : %d\n" COLOR_RESET, ruptures);
        printf(COLOR_YELLOW "\t\tStocks faibles : %d\n" COLOR_RESET, faibles);
        printf(COLOR_MAGENTA "\t\tCommandes en retard : %d\n" COLOR_RESET, retards);
        printf(COLOR_RED "\t\tMedicaments perimes : %d\n" COLOR_RESET, perimes);
        
        printf(COLOR_CYAN);
        centrer_texte("");
        centrer_texte("=== MENU NOTIFICATIONS ===");
        centrer_texte("");
        printf(COLOR_RESET);
        printf(COLOR_RED);
        centrer_texte("1. Ruptures de stock");
        printf(COLOR_RESET);
        printf(COLOR_YELLOW);
        centrer_texte("2. Stocks faibles");
        printf(COLOR_RESET);
        printf(COLOR_MAGENTA);
        centrer_texte("3. Commandes en retard");
        printf(COLOR_RESET);
        printf(COLOR_RED);
        centrer_texte("4. Medicaments perimes");
        printf(COLOR_RESET);
        printf(COLOR_BLUE);
        centrer_texte("5. Retour au menu principal");
        printf(COLOR_RESET);
        printf("\n");
        printf(COLOR_WHITE);
        centrer_texte_sans_newline("Votre choix : ");
        printf(COLOR_RESET);
        
        choix = lire_entier();
        
        switch (choix) {
            case 1:
                verifier_ruptures(liste);
                break;
            case 2:
                verifier_stocks_faibles(liste);
                break;
            case 3:
                verifier_commandes_en_retard(commandes, c_count);
                break;
            case 4:
                verifier_medicaments_perimes(liste);
                break;
            case 5:
                return;
            default:
                printf(COLOR_RED "\n\t\tChoix invalide !\n" COLOR_RESET);
                attendre_entree();
                break;
        }
    } while (choix != 5);
}

void menu_notifications(Medicament* liste, Commande commandes[], int c_count, const char *user) {
    afficher_notifications(liste, commandes, c_count);
    (void)user;  // Unused for now
}
