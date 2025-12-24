#include "fonctions.h"

void charger_commandes(Commande commandes[], int *count) {
    FILE *file = fopen("bin/data/commandes.txt", "r");
    *count = 0;
    
    if (file == NULL) {
        // Creer un fichier avec quelques donnees d'exemple
        file = fopen("bin/data/commandes.txt", "w");
        if (file == NULL) return;
        
        char date_aujourdhui[11];
        obtenir_date_du_jour(date_aujourdhui);
        
        fprintf(file, "CMD-20251020-001|102|Amoxicilline|100|LaboMed|Livree|%s|8\n", date_aujourdhui);
        fprintf(file, "CMD-20251025-003|203|Doliprane|80|MediMar|En attente|%s|3\n", date_aujourdhui);
        
        fclose(file);
        file = fopen("data/commandes.txt", "r");
    }
    
    while (fscanf(file, "%29[^|]|%d|%99[^|]|%d|%49[^|]|%19[^|]|%10[^|]|%d\n",
                  commandes[*count].idC,
                  &commandes[*count].idM,
                  commandes[*count].nom_medicament,
                  &commandes[*count].quantite,
                  commandes[*count].fournisseur,
                  commandes[*count].statut,
                  commandes[*count].date_commande,
                  &commandes[*count].jours_attente) == 8) {
        (*count)++;
        if (*count >= MAX_COMMANDES) break;
    }
    
    fclose(file);
    
    // Creation de la sauvegarde
    FILE *backup = fopen("bin/data/commandes.bak", "w");
    if (backup) {
        for (int i = 0; i < *count; i++) {
            fprintf(backup, "%s|%d|%s|%d|%s|%s|%s|%d\n",
                    commandes[i].idC,
                    commandes[i].idM,
                    commandes[i].nom_medicament,
                    commandes[i].quantite,
                    commandes[i].fournisseur,
                    commandes[i].statut,
                    commandes[i].date_commande,
                    commandes[i].jours_attente);
        }
        fclose(backup);
    }
}

void sauvegarder_commandes(Commande commandes[], int count) {
    FILE *file = fopen("bin/data/commandes.txt", "w");
    
    if (file == NULL) {
        printf(COLOR_RED "\nErreur : Impossible de sauvegarder les commandes.\n" COLOR_RESET);
        return;
    }
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s|%d|%s|%d|%s|%s|%s|%d\n",
                commandes[i].idC,
                commandes[i].idM,
                commandes[i].nom_medicament,
                commandes[i].quantite,
                commandes[i].fournisseur,
                commandes[i].statut,
                commandes[i].date_commande,
                commandes[i].jours_attente);
    }
    
    fclose(file);
}

void generer_id_commande(char *idC) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    sprintf(idC, "CMD-%04d%02d%02d-%03d", 
            tm_info->tm_year + 1900,
            tm_info->tm_mon + 1,
            tm_info->tm_mday,
            rand() % 1000);
}

void passer_commande(Medicament* liste, Commande commandes[], int *c_count, const char *user) {
    char nom_medicament[100];
    int quantite_demandee;
    char fournisseur[50];
    
    (void)user;  // Unused for now
    
    int continuer = 1;
    while (continuer) {
        clear_screen();
        afficher_entete();
        
        printf(COLOR_CYAN "\n\n\t\t=== PASSER UNE COMMANDE ===\n\n" COLOR_RESET);
        
        printf(COLOR_WHITE "\t\tNom du medicament : " COLOR_RESET);
        lire_chaine(nom_medicament, 100);
        
        // Rechercher le medicament
        Medicament* courant = liste;
        Medicament* trouve = NULL;
        while (courant) {
            if (strcasecmp_custom(courant->nom, nom_medicament) == 0) {
                trouve = courant;
                break;
            }
            courant = courant->next;
        }
        
        if (!trouve) {
            printf(COLOR_RED "\t\tMedicament introuvable. Verifiez le nom du medicament.\n" COLOR_RESET);
            attendre_entree();
            return;
        }
        
        // Demander le fournisseur AVANT la quantite
        printf(COLOR_WHITE "\t\tFournisseur : " COLOR_RESET);
        lire_chaine(fournisseur, 50);
        
        printf(COLOR_WHITE "\t\tQuantite demandee : " COLOR_RESET);
        quantite_demandee = lire_entier();
        
        if (quantite_demandee <= 0) {
            printf(COLOR_RED "\t\tQuantite invalide !\n" COLOR_RESET);
            attendre_entree();
            return;
        }
        
        // Verifier si ce fournisseur specifique a la quantite demandee
        FILE* stock_fournisseurs = fopen("bin/data/stock_fournisseurs.txt", "r");
        int fournisseur_ok = 0;
        int quantite_fournisseur = 0;
        float prix_fournisseur = 0;
        
        if (stock_fournisseurs) {
            char nom_fournisseur[50];
            char nom_med[100];
            int qte;
            float prix;
            
            while (fscanf(stock_fournisseurs, "%49[^|]|%99[^|]|%d|%f\n",
                         nom_fournisseur, nom_med, &qte, &prix) == 4) {
                if (strcasecmp_custom(nom_fournisseur, fournisseur) == 0 && 
                    strcasecmp_custom(nom_med, nom_medicament) == 0) {
                    quantite_fournisseur = qte;
                    prix_fournisseur = prix;
                    if (qte >= quantite_demandee) {
                        fournisseur_ok = 1;
                    }
                    break;
                }
            }
            fclose(stock_fournisseurs);
        }
        
        // Si le fournisseur n'a pas assez de stock
        if (!fournisseur_ok) {
            printf("\n");
            printf(COLOR_RED);
            centrer_texte("");
            if (quantite_fournisseur > 0) {
                char msg[200];
                snprintf(msg, sizeof(msg), "Le fournisseur %s a seulement %d unites disponibles.", 
                        fournisseur, quantite_fournisseur);
                centrer_texte(msg);
            } else {
                char msg[200];
                snprintf(msg, sizeof(msg), "Le fournisseur %s n'a pas ce medicament en stock.", fournisseur);
                centrer_texte(msg);
            }
            centrer_texte("");
            printf(COLOR_RESET);
            
            // Afficher les fournisseurs alternatifs
            printf(COLOR_GREEN);
            centrer_texte("Fournisseurs disponibles pour ce medicament :");
            printf(COLOR_RESET);
            printf("\n");
            
            stock_fournisseurs = fopen("bin/data/stock_fournisseurs.txt", "r");
            int fournisseur_trouve = 0;
            
            if (stock_fournisseurs) {
                char nom_fournisseur[50];
                char nom_med[100];
                int qte;
                float prix;
                
                while (fscanf(stock_fournisseurs, "%49[^|]|%99[^|]|%d|%f\n",
                             nom_fournisseur, nom_med, &qte, &prix) == 4) {
                    if (strcasecmp_custom(nom_med, nom_medicament) == 0 && 
                        qte >= quantite_demandee) {
                        printf(COLOR_WHITE "\t\t- %s " COLOR_RESET, nom_fournisseur);
                        printf(COLOR_GREEN "(Stock: %d unites, Prix: %.2f DH)\n" COLOR_RESET, qte, prix);
                        fournisseur_trouve = 1;
                    }
                }
                fclose(stock_fournisseurs);
            }
            
            if (!fournisseur_trouve) {
                printf(COLOR_RED);
                centrer_texte("Aucun fournisseur n'a la quantite demandee.");
                printf(COLOR_RESET);
            } else {
                printf("\n");
                printf(COLOR_CYAN);
                centrer_texte("Vous pouvez passer une commande aupres d'un de ces fournisseurs.");
                printf(COLOR_RESET);
            }
            
            printf("\n");
            printf(COLOR_YELLOW);
            centrer_texte_sans_newline("Voulez vous continuer le passage de commande (oui/non)? ");
            printf(COLOR_RESET);
            
            char reponse[10];
            lire_chaine(reponse, 10);
            
            if (strcasecmp(reponse, "oui") == 0 || 
                strcasecmp(reponse, "o") == 0 || 
                strcasecmp(reponse, "yes") == 0 || 
                strcasecmp(reponse, "y") == 0) {
                // Continuer la boucle pour recommencer
                continue;
            } else {
                // Retour au menu precedent
                return;
            }
        }
        
        // Le fournisseur a assez de stock - creer la commande
        Commande nouvelle_commande;
        generer_id_commande(nouvelle_commande.idC);
        nouvelle_commande.idM = trouve->id;
        strcpy(nouvelle_commande.nom_medicament, nom_medicament);
        nouvelle_commande.quantite = quantite_demandee;
        strcpy(nouvelle_commande.fournisseur, fournisseur);
        strcpy(nouvelle_commande.statut, "En attente");
        obtenir_date_du_jour(nouvelle_commande.date_commande);
        nouvelle_commande.jours_attente = 0;
        
        // Ajouter la commande
        commandes[*c_count] = nouvelle_commande;
        (*c_count)++;
        
        // Sauvegarder
        sauvegarder_commandes(commandes, *c_count);
        
        printf("\n");
        printf(COLOR_GREEN);
        centrer_texte("");
        centrer_texte("Commande passee avec succes !");
        centrer_texte("");
        printf(COLOR_RESET);
        printf(COLOR_YELLOW);
        char msg[100];
        snprintf(msg, sizeof(msg), "ID de commande : %s", nouvelle_commande.idC);
        centrer_texte(msg);
        snprintf(msg, sizeof(msg), "Fournisseur : %s", fournisseur);
        centrer_texte(msg);
        snprintf(msg, sizeof(msg), "Prix unitaire : %.2f DH", prix_fournisseur);
        centrer_texte(msg);
        snprintf(msg, sizeof(msg), "Statut : %s", nouvelle_commande.statut);
        centrer_texte(msg);
        centrer_texte("");
        printf(COLOR_RESET);
        attendre_entree();
        
        // Commande reussie, sortir de la boucle
        continuer = 0;
    }
}

void afficher_suivi_commandes(Commande commandes[], int count) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== SUIVI DES COMMANDES ===\n\n" COLOR_RESET);
    
    if (count == 0) {
        printf(COLOR_YELLOW "\t\tAucune commande enregistree.\n\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    printf(COLOR_GREEN "ID Commande        | Medicament       | Qté | Statut     | Jours | Fournisseur\n" COLOR_RESET);
    printf("-------------------+------------------+-----+------------+-------+------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%-18s | %-16s | %-3d | %-10s | %-5d | %s\n",
               commandes[i].idC,
               commandes[i].nom_medicament,
               commandes[i].quantite,
               commandes[i].statut,
               commandes[i].jours_attente,
               commandes[i].fournisseur);
    }
    
    printf("\n");
}

void marquer_commande_livree(Commande commandes[], int count, Medicament* liste) {
    char id_commande[30];
    int index = -1;
    
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== MARQUER COMMANDE COMME LIVREE ===\n\n" COLOR_RESET);
    
    printf(COLOR_WHITE "\t\tID de la commande : " COLOR_RESET);
    lire_chaine(id_commande, 30);
    
    // Trouver la commande
    for (int i = 0; i < count; i++) {
        if (strcmp(commandes[i].idC, id_commande) == 0) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        printf(COLOR_RED "\t\tCommande non trouvee !\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    if (strcmp(commandes[index].statut, "Livree") == 0) {
        printf(COLOR_YELLOW "\t\tCette commande est deja livree.\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    // Mettre a jour le statut
    strcpy(commandes[index].statut, "Livree");
    
    // Mettre a jour le stock
    Medicament* courant = liste;
    while (courant) {
        if (commandes[index].idM == courant->id) {
            courant->quantite += commandes[index].quantite;
            break;
        }
        courant = courant->next;
    }
    
    sauvegarder_commandes(commandes, count);
    sauvegarder_medicaments(liste);
    
    printf(COLOR_GREEN "\n\t\tCommande marquee comme livree avec succes !\n" COLOR_RESET);
    printf(COLOR_YELLOW "\t\tStock mis a jour.\n" COLOR_RESET);
    attendre_entree();
}

void annuler_commande(Commande commandes[], int count) {
    char id_commande[30];
    int index = -1;
    
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== ANNULER UNE COMMANDE ===\n\n" COLOR_RESET);
    
    printf(COLOR_WHITE "\t\tID de la commande : " COLOR_RESET);
    lire_chaine(id_commande, 30);
    
    // Trouver la commande
    for (int i = 0; i < count; i++) {
        if (strcmp(commandes[i].idC, id_commande) == 0) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        printf(COLOR_RED "\t\tCommande non trouvee !\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    if (strcmp(commandes[index].statut, "Annulee") == 0) {
        printf(COLOR_YELLOW "\t\tCette commande est deja annulee.\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    printf(COLOR_RED "\n\t\tVous allez annuler la commande : %s\n" COLOR_RESET, commandes[index].idC);
    printf(COLOR_YELLOW "\t\tMedicament : %s, Quantite : %d\n" COLOR_RESET, 
           commandes[index].nom_medicament, commandes[index].quantite);
    
    printf(COLOR_WHITE "\t\tConfirmer l'annulation (oui/non) : " COLOR_RESET);
    char confirmation[10];
    lire_chaine(confirmation, 10);
    
    if (strcmp(confirmation, "oui") == 0 || strcmp(confirmation, "OUI") == 0) {
        strcpy(commandes[index].statut, "Annulee");
        sauvegarder_commandes(commandes, count);
        printf(COLOR_GREEN "\n\t\tCommande annulee avec succes !\n" COLOR_RESET);
    } else {
        printf(COLOR_YELLOW "\n\t\tAnnulation annulee.\n" COLOR_RESET);
    }
    
    attendre_entree();
}

void menu_suivi_commandes(Commande commandes[], int *count, Medicament* liste, int is_admin, const char *user) {
    int choix;
    
    (void)user;  // Unused for now
    
    do {
        afficher_suivi_commandes(commandes, *count);
        
        if (is_admin) {
            printf(COLOR_GREEN);
            centrer_texte("[1] Marquer comme livree");
            printf(COLOR_RESET);
            printf(COLOR_RED);
            centrer_texte("[2] Annuler la commande");
            printf(COLOR_RESET);
            printf(COLOR_YELLOW);
            centrer_texte("[3] Retour au menu principal");
            printf(COLOR_RESET);
            printf("\n");
        } else {
            printf(COLOR_YELLOW);
            centrer_texte("[1] Retour au menu principal");
            printf(COLOR_RESET);
            printf("\n");
        }
        
        printf(COLOR_WHITE);
        centrer_texte_sans_newline("Votre choix : ");
        printf(COLOR_RESET);
        choix = lire_entier();
        
        switch (choix) {
            case 1:
                if (is_admin) {
                    marquer_commande_livree(commandes, *count, liste);
                } else {
                    return;
                }
                break;
            case 2:
                if (is_admin) {
                    annuler_commande(commandes, *count);
                } else {
                    printf(COLOR_RED "\n\t\tChoix invalide !\n" COLOR_RESET);
                    attendre_entree();
                }
                break;
            case 3:
                if (is_admin) {
                    return;
                } else {
                    printf(COLOR_RED "\n\t\tChoix invalide !\n" COLOR_RESET);
                    attendre_entree();
                }
                break;
            default:
                printf(COLOR_RED "\n\t\tChoix invalide !\n" COLOR_RESET);
                attendre_entree();
                break;
        }
    } while (1);
}
