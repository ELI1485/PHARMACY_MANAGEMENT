#include "fonctions.h"

void charger_fournisseurs(Fournisseur fournisseurs[], int *count) {
    FILE *file = fopen("bin/data/fournisseurs.txt", "r");
    *count = 0;
    
    if (file == NULL) {
        // Si le fichier n'existe pas, le creer avec des donnees par defaut
        file = fopen("bin/data/fournisseurs.txt", "w");
        if (file == NULL) return;
        
        fprintf(file, "1|LaboMed|0522334455|contact@labomed.ma|12 Rue Mohamed V, Casablanca\n");
        fprintf(file, "2|LaboMaroc|0522445566|info@labomaroc.ma|45 Avenue Hassan II, Rabat\n");
        fprintf(file, "3|MediMar|0522556677|ventes@medimar.ma|78 Boulevard Zerktouni, Casablanca\n");
        fprintf(file, "4|PharmaPlus|0522667788|pharmaplus@gmail.com|23 Rue Allal Ben Abdellah, Fes\n");
        fprintf(file, "5|MediCare|0522778899|contact@medicare.ma|56 Avenue Mohammed VI, Marrakech\n");
        
        fclose(file);
        file = fopen("bin/data/fournisseurs.txt", "r");
    }
    
    while (fscanf(file, "%d|%49[^|]|%19[^|]|%49[^|]|%99[^\n]\n",
                  &fournisseurs[*count].id,
                  fournisseurs[*count].nom,
                  fournisseurs[*count].telephone,
                  fournisseurs[*count].email,
                  fournisseurs[*count].adresse) == 5) {
        (*count)++;
        if (*count >= MAX_FOURNISSEURS) break;
    }
    
    fclose(file);
}

void sauvegarder_fournisseurs(Fournisseur fournisseurs[], int count) {
    FILE *file = fopen("bin/data/fournisseurs.txt", "w");
    
    if (file == NULL) {
        printf(COLOR_RED "\nErreur : Impossible de sauvegarder les fournisseurs.\n" COLOR_RESET);
        return;
    }
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d|%s|%s|%s|%s\n",
                fournisseurs[i].id,
                fournisseurs[i].nom,
                fournisseurs[i].telephone,
                fournisseurs[i].email,
                fournisseurs[i].adresse);
    }
    
    fclose(file);
}

void afficher_fournisseurs(Fournisseur fournisseurs[], int count) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN);
    centrer_texte("");
    centrer_texte("=== LISTE DES FOURNISSEURS ===");
    centrer_texte("");
    printf(COLOR_RESET);
    
    if (count == 0) {
        printf(COLOR_YELLOW);
        centrer_texte("Aucun fournisseur enregistre.");
        printf(COLOR_RESET);
    } else {
        printf(COLOR_WHITE);
        printf("\n\t%-5s %-25s %-15s %-35s %-40s\n", "ID", "Nom", "Telephone", "Email", "Adresse");
        printf("\t");
        for (int i = 0; i < 120; i++) printf("-");
        printf("\n");
        printf(COLOR_RESET);
        
        for (int i = 0; i < count; i++) {
            printf(COLOR_GREEN);
            printf("\t%-5d ", fournisseurs[i].id);
            printf(COLOR_RESET);
            printf("%-25s %-15s %-35s %-40s\n",
                   fournisseurs[i].nom,
                   fournisseurs[i].telephone,
                   fournisseurs[i].email,
                   fournisseurs[i].adresse);
        }
    }
    
    printf("\n");
    afficher_pied_page();
    attendre_entree();
}
