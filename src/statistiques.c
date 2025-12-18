#include "fonctions.h"

/* Format ventes.txt: date|id_medicament|quantite|total */

void afficher_gains_jour() {
    clear_screen();
    afficher_entete();
    
    centrer_texte("=== GAINS DU JOUR ===");
    printf("\n");
    
    FILE *file = fopen("data/ventes.txt", "r");
    if (!file) {
        printf("Aucune vente enregistree.\n");
        attendre_entree();
        return;
    }
    
    char date[11], today[11];
    int id, qte;
    float total, gains = 0;
    int ventes = 0;
    
    obtenir_date_du_jour(today);
    
    while (fscanf(file, "%10[^|]|%d|%d|%f\n", date, &id, &qte, &total) == 4) {
        if (strcmp(date, today) == 0) {
            gains += total;
            ventes++;
        }
    }
    
    fclose(file);
    
    printf("Date                : %s\n", today);
    printf("Nombre de ventes    : %d\n", ventes);
    printf("Gains totaux        : %.2f DH\n", gains);
    
    if (ventes > 0)
        printf("Vente moyenne       : %.2f DH\n", gains / ventes);
    
    attendre_entree();
}

void afficher_gains_mensuels() {
    clear_screen();
    afficher_entete();
    
    centrer_texte("=== GAINS MENSUELS ===");
    printf("\n");
    
    FILE *file = fopen("data/ventes.txt", "r");
    if (!file) {
        printf("Aucune vente.\n");
        attendre_entree();
        return;
    }
    
    char date[11];
    int id, qte;
    float total, gains = 0;
    char mois[8];
    
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(mois, sizeof(mois), "%m-%Y", tm_info);
    
    while (fscanf(file, "%10[^|]|%d|%d|%f\n", date, &id, &qte, &total) == 4) {
        if (strstr(date, mois))
            gains += total;
    }
    
    fclose(file);
    
    printf("Mois            : %s\n", mois);
    printf("Gains totaux    : %.2f DH\n", gains);
    
    attendre_entree();
}

void afficher_ventes_jour() {
    clear_screen();
    afficher_entete();
    
    centrer_texte("=== VENTES DU JOUR ===");
    printf("\n");
    
    FILE *file = fopen("data/ventes.txt", "r");
    if (!file) {
        printf("Aucune vente.\n");
        attendre_entree();
        return;
    }
    
    char date[11], today[11];
    int id, qte, total_qte = 0;
    float total;
    
    obtenir_date_du_jour(today);
    
    while (fscanf(file, "%10[^|]|%d|%d|%f\n", date, &id, &qte, &total) == 4) {
        if (strcmp(date, today) == 0)
            total_qte += qte;
    }
    
    fclose(file);
    
    printf("Date            : %s\n", today);
    printf("Unites vendues  : %d\n", total_qte);
    
    attendre_entree();
}

void afficher_produits_plus_vendus() {
    clear_screen();
    afficher_entete();
    
    centrer_texte("=== PRODUITS LES PLUS VENDUS ===");
    printf("\n");
    
    int ids[1000] = {0};
    int qtes[1000] = {0};
    int count = 0;
    
    FILE *file = fopen("data/ventes.txt", "r");
    if (!file) {
        printf("Aucune vente.\n");
        attendre_entree();
        return;
    }
    
    char date[11];
    int id, qte;
    float total;
    
    while (fscanf(file, "%10[^|]|%d|%d|%f\n", date, &id, &qte, &total) == 4) {
        int found = 0;
        for (int i = 0; i < count; i++) {
            if (ids[i] == id) {
                qtes[i] += qte;
                found = 1;
                break;
            }
        }
        
        if (!found) {
            ids[count] = id;
            qtes[count] = qte;
            count++;
        }
    }
    
    fclose(file);
    
    // Tri simple
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (qtes[j] > qtes[i]) {
                int tmpQ = qtes[i], tmpI = ids[i];
                qtes[i] = qtes[j];
                ids[i] = ids[j];
                qtes[j] = tmpQ;
                ids[j] = tmpI;
            }
        }
    }
    
    Medicament* medicaments = charger_medicaments();
    
    for (int i = 0; i < count && i < 5; i++) {
        Medicament* courant = medicaments;
        while (courant) {
            if (courant->id == ids[i]) {
                printf("%d. %s : %d unites\n", i + 1, courant->nom, qtes[i]);
                break;
            }
            courant = courant->next;
        }
    }
    
    liberer_liste_medicaments(medicaments);
    attendre_entree();
}

void afficher_produits_en_rupture(Medicament* liste) {
    clear_screen();
    afficher_entete();
    
    centrer_texte("=== PRODUITS EN RUPTURE ===");
    printf("\n");
    
    int trouve = 0;
    Medicament* courant = liste;
    
    while (courant) {
        if (courant->quantite == 0) {
            printf("- %s\n", courant->nom);
            trouve = 1;
        }
        courant = courant->next;
    }
    
    if (!trouve)
        printf("Aucun produit en rupture.\n");
    
    attendre_entree();
}

void afficher_chiffre_affaires_annuel() {
    clear_screen();
    afficher_entete();
    
    centrer_texte("=== CHIFFRE D'AFFAIRES ANNUEL ===");
    printf("\n");
    
    FILE *file = fopen("data/ventes.txt", "r");
    if (!file) {
        printf("Aucune vente.\n");
        attendre_entree();
        return;
    }
    
    char date[11];
    int id, qte;
    float total, ca_annuel = 0;
    char annee[5];
    
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(annee, sizeof(annee), "%Y", tm_info);
    
    while (fscanf(file, "%10[^|]|%d|%d|%f\n", date, &id, &qte, &total) == 4) {
        if (strstr(date, annee))
            ca_annuel += total;
    }
    
    fclose(file);
    
    printf("Annee                      : %s\n", annee);
    printf("Chiffre d'affaires total   : %.2f DH\n", ca_annuel);
    
    attendre_entree();
}

void afficher_pertes(Medicament* liste) {
    clear_screen();
    afficher_entete();
    
    centrer_texte("=== PERTES ===");
    printf("\n");
    
    char today[11];
    obtenir_date_du_jour(today);
    
    float pertes = 0;
    Medicament* courant = liste;
    
    while (courant) {
        if (comparer_dates(courant->date_expiration, today) < 0) {
            float perte = courant->quantite * courant->prix;
            printf("- %s : %.2f DH\n", courant->nom, perte);
            pertes += perte;
        }
        courant = courant->next;
    }
    
    printf("\nTotal pertes : %.2f DH\n", pertes);
    attendre_entree();
}

void menu_statistiques(Medicament* liste, const char *user) {
    int choix;
    
    (void)user;  // Will use later for global layout
    
    do {
        clear_screen();
        afficher_entete();
        
        printf(COLOR_CYAN);
        centrer_texte("=== STATISTIQUES ===");
        printf("\n");
        printf(COLOR_RESET);
        
        centrer_texte("[1] Gains du jour");
        centrer_texte("[2] Gains mensuels");
        centrer_texte("[3] Ventes du jour");
        centrer_texte("[4] Produits les plus vendus");
        centrer_texte("[5] Produits en rupture");
        centrer_texte("[6] Chiffre d'affaires annuel");
        printf(COLOR_RED);
        centrer_texte("[7] Pertes");
        printf(COLOR_RESET);
        centrer_texte("[8] Retour au menu principal");
        printf("\n");
        printf(COLOR_RESET);
        centrer_texte_sans_newline("Votre choix : ");
        choix = lire_entier();
        
        switch (choix) {
            case 1: afficher_gains_jour(); break;
            case 2: afficher_gains_mensuels(); break;
            case 3: afficher_ventes_jour(); break;
            case 4: afficher_produits_plus_vendus(); break;
            case 5: afficher_produits_en_rupture(liste); break;
            case 6: afficher_chiffre_affaires_annuel(); break;
            case 7: afficher_pertes(liste); break;
            case 8: return;
            default:
                printf("Choix invalide.\n");
                attendre_entree();
        }
        
    } while (1);
}
