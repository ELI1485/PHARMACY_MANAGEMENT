#include "fonctions.h"

// Helper: Create a new medication node
Medicament* creer_medicament(Medicament* liste) {
    Medicament* med = (Medicament*)malloc(sizeof(Medicament));
    if (!med) {
        printf(COLOR_RED " Erreur memoire.\n" COLOR_RESET);
        return NULL;
    }

    printf(COLOR_WHITE);
    centrer_texte_sans_newline("Entrez les details du nouveau medicament :\n\n");
    printf(COLOR_RESET);
    
    // ID with duplicate check
    int id_ok = 0;
    while (!id_ok) {
        printf(COLOR_WHITE);
        centrer_texte_sans_newline("ID : ");
        printf(COLOR_RESET);
        med->id = lire_entier();
        
        // Check for duplicate ID
        Medicament* courant = liste;
        int duplicate = 0;
        while (courant) {
            if (courant->id == med->id) {
                duplicate = 1;
                break;
            }
            courant = courant->next;
        }
        
        if (duplicate) {
            printf(COLOR_RED);
            centrer_texte("");
            char error_msg[100];
            snprintf(error_msg, sizeof(error_msg), "ID %d deja utilise. Veuillez entrer un autre ID.", med->id);
            centrer_texte(error_msg);
            centrer_texte("");
            printf(COLOR_RESET);
        } else {
            id_ok = 1;
        }
    }
    
    printf(COLOR_WHITE);
    centrer_texte_sans_newline("Nom : ");
    printf(COLOR_RESET);
    lire_chaine(med->nom, 100);
    
    // Category selection with validation
    int cat = 0;
    while (cat < 1 || cat > 4) {
        printf(COLOR_WHITE);
        centrer_texte_sans_newline("Categorie (1.Antibiotiques 2.Analgesiques 3.Anti-inflammatoires 4.Autres) : ");
        printf(COLOR_RESET);
        cat = lire_entier();
        
        if (cat < 1 || cat > 4) {
            printf(COLOR_RED);
            centrer_texte("Choix invalide! Veuillez choisir entre 1 et 4.");
            printf(COLOR_RESET);
            printf("\n");
        }
    }
    
    if (cat == 4) {
        printf(COLOR_YELLOW);
        centrer_texte_sans_newline("Nom de la categorie : ");
        printf(COLOR_RESET);
        lire_chaine(med->categorie, 50);
        sauvegarder_categorie(med->categorie);
        printf(COLOR_GREEN);
        centrer_texte("Nouvelle categorie ajoutee!\n");
        printf(COLOR_RESET);
    } else if (cat >= 1 && cat <= 3) {
        const char *categories[] = {"Antibiotiques", "Analgesiques", "Anti-inflammatoires"};
        strcpy(med->categorie, categories[cat - 1]);
    }
    
    printf(COLOR_WHITE);
    centrer_texte_sans_newline("Quantite : ");
    printf(COLOR_RESET);
    med->quantite = lire_entier();
    while (!valider_quantite(med->quantite)) {
        printf(COLOR_RED);
        centrer_texte("Quantite invalide. Reessayez : ");
        printf(COLOR_RESET);
        med->quantite = lire_entier();
    }
    
    printf(COLOR_WHITE);
    centrer_texte_sans_newline("Prix (DH) : ");
    printf(COLOR_RESET);
    med->prix = lire_float();
    while (!valider_prix(med->prix)) {
        printf(COLOR_RED);
        centrer_texte("Prix invalide. Reessayez : ");
        printf(COLOR_RESET);
        med->prix = lire_float();
    }
    
    printf(COLOR_WHITE);
    centrer_texte_sans_newline("Fournisseur : ");
    printf(COLOR_RESET);
    lire_chaine(med->fournisseur, 50);
    
    // Date validation with warning
    int date_ok = 0;
    while (!date_ok) {
        printf(COLOR_WHITE);
        centrer_texte_sans_newline("Date d'expiration (JJ-MM-AAAA) : ");
        printf(COLOR_RESET);
        fflush(stdout);
        
        lire_chaine(med->date_expiration, 11);
        
        // Skip empty input
        if (strlen(med->date_expiration) == 0) {
            printf(COLOR_RED);
            centrer_texte("Entree vide. Veuillez entrer une date.");
            printf(COLOR_RESET);
            printf("\n");
            continue;
        }
        
        // Validate date format
        if (!valider_date(med->date_expiration)) {
            printf(COLOR_RED);
            centrer_texte("Format de date invalide. Reessayez (JJ-MM-AAAA)");
            printf(COLOR_RESET);
            printf("\n");
            continue;
        }
        
        // Check if date is in the past
        char today[11];
        obtenir_date_du_jour(today);
        
        if (comparer_dates(med->date_expiration, today) < 0) {
            char warning_msg[200];
            snprintf(warning_msg, sizeof(warning_msg), "ATTENTION: La date entree (%s) est dans le passe!", med->date_expiration);
            char date_msg[100];
            snprintf(date_msg, sizeof(date_msg), "Date actuelle: %s", today);
            
            printf("\n");
            printf(COLOR_YELLOW);
            centrer_texte(warning_msg);
            centrer_texte(date_msg);
            centrer_texte_sans_newline("Voulez-vous corriger la date? ");
            printf(COLOR_RESET);
            fflush(stdout);
            
            char reponse[10];
            // Keep asking until we get a non-empty response
            do {
                lire_chaine(reponse, 10);
                if (strlen(reponse) == 0) {
                    printf(COLOR_YELLOW);
                    centrer_texte_sans_newline("Veuillez repondre (oui/non) : ");
                    printf(COLOR_RESET);
                    fflush(stdout);
                }
            } while (strlen(reponse) == 0);
            
            // If user wants to correct, loop back to ask for date again
            if (strcasecmp(reponse, "oui") == 0 || 
                strcasecmp(reponse, "o") == 0 || 
                strcasecmp(reponse, "yes") == 0 || 
                strcasecmp(reponse, "y") == 0) {
                printf("\n");
                continue;  // Ask for date again
            }
            // If user says "non", accept the past date and continue
        }
        
        // Date is valid and accepted
        date_ok = 1;
    }

    med->next = NULL;
    return med;
}

// Load medications from file into linked list
Medicament* charger_medicaments() {
    FILE* fichier = fopen("bin/data/medicaments.txt", "r");
    if (!fichier) {
        printf(COLOR_YELLOW "Fichier medicaments.txt introuvable. Creation d'un nouveau fichier.\n" COLOR_RESET);
        return NULL;
    }

    Medicament* tete = NULL;
    Medicament* courant = NULL;

    while (1) {
        Medicament* nouveau = (Medicament*)malloc(sizeof(Medicament));
        if (!nouveau) {
            printf(COLOR_RED " Erreur memoire lors du chargement des medicaments.\n" COLOR_RESET);
            liberer_liste_medicaments(tete);
            fclose(fichier);
            return NULL;
        }

        int lu = fscanf(fichier, "%d|%99[^|]|%49[^|]|%d|%f|%49[^|]|%10[^\n]\n",
                        &nouveau->id, nouveau->nom, nouveau->categorie,
                        &nouveau->quantite, &nouveau->prix, nouveau->fournisseur, nouveau->date_expiration);

        if (lu != 7) {
            free(nouveau);
            break;
        }

        nouveau->next = NULL;
        if (!tete) {
            tete = nouveau;
            courant = tete;
        } else {
            courant->next = nouveau;
            courant = nouveau;
        }
    }

    fclose(fichier);
    return tete;
}

// Save linked list to file
void sauvegarder_medicaments(Medicament* liste) {
    FILE* fichier = fopen("bin/data/medicaments.txt", "w");
    if (!fichier) {
        printf(COLOR_RED " Erreur : Impossible d'ouvrir medicaments.txt\\n" COLOR_RESET);
        return;
    }

    Medicament* courant = liste;
    while (courant) {
        fprintf(fichier, "%d|%s|%s|%d|%.2f|%s|%s\n",
                courant->id, courant->nom, courant->categorie,
                courant->quantite, courant->prix, courant->fournisseur, courant->date_expiration);
        courant = courant->next;
    }

    fclose(fichier);
}

// Free entire linked list
void liberer_liste_medicaments(Medicament* liste) {
    Medicament* suivant;
    while (liste) {
        suivant = liste->next;
        free(liste);
        liste = suivant;
    }
}

// Add medication to list
void ajouter_medicament(Medicament** liste) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN);
    centrer_texte("");
    centrer_texte("=== AJOUTER UN MEDICAMENT ===");
    centrer_texte("");
    printf(COLOR_RESET);
    
    Medicament* nouveau = creer_medicament(*liste);
    if (!nouveau) return;

    // Insert at head
    nouveau->next = *liste;
    *liste = nouveau;
    
    sauvegarder_medicaments(*liste);
    
    clear_screen();
    afficher_entete();
    
    printf("\n\n");
    printf(COLOR_GREEN);
    centrer_texte("=========================================");
    centrer_texte("");
    char success_msg[150];
    snprintf(success_msg, sizeof(success_msg), "Medicament '%s' ajoute avec succes !", nouveau->nom);
    centrer_texte(success_msg);
    centrer_texte("");
    centrer_texte("=========================================");
    centrer_texte("");
    printf(COLOR_RESET);
    
    printf(COLOR_YELLOW);
    centrer_texte("Appuyez sur Entree pour continuer...");
    printf(COLOR_RESET);
    attendre_entree();
    return;
}

// Modify medication
void modifier_medicament(Medicament* liste) {
    if (!liste) {
        printf(COLOR_YELLOW "Aucun medicament a modifier.\n" COLOR_RESET);
        attendre_entree();
        return;
    }

    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== MODIFIER UN MEDICAMENT ===\n\n" COLOR_RESET);

    int id;
    printf(COLOR_WHITE "\t\tEntrez l'ID du medicament a modifier : " COLOR_RESET);
    id = lire_entier();

    Medicament* courant = liste;
    while (courant && courant->id != id) {
        courant = courant->next;
    }

    if (!courant) {
        printf(COLOR_RED " Medicament avec ID %d non trouve.\n" COLOR_RESET, id);
        attendre_entree();
        return;
    }

    printf(COLOR_YELLOW "\n\t\tMedicament trouve : %s\n\n" COLOR_RESET, courant->nom);
    printf(COLOR_WHITE "\t\tEntrez les nouveaux details (laissez vide pour garder) :\n" COLOR_RESET);

    char buffer[100];

    printf(COLOR_WHITE "\t\tNouveau nom [%s] : " COLOR_RESET, courant->nom);
    lire_chaine(buffer, sizeof(courant->nom));
    if (strlen(buffer) > 0) strcpy(courant->nom, buffer);

    printf(COLOR_WHITE "\t\tNouvelle categorie [%s] : " COLOR_RESET, courant->categorie);
    lire_chaine(buffer, sizeof(courant->categorie));
    if (strlen(buffer) > 0) strcpy(courant->categorie, buffer);

    printf(COLOR_WHITE "\t\tNouvelle quantite [%d] : " COLOR_RESET, courant->quantite);
    int qte = lire_entier();
    if (qte >= 0) courant->quantite = qte;

    printf(COLOR_WHITE "\t\tNouveau prix (DH) [%.2f] : " COLOR_RESET, courant->prix);
    float prix = lire_float();
    if (prix >= 0) courant->prix = prix;

    printf(COLOR_WHITE "\t\tNouveau fournisseur [%s] : " COLOR_RESET, courant->fournisseur);
    lire_chaine(buffer, 50);
    if (strlen(buffer) > 0) strcpy(courant->fournisseur, buffer);

    printf(COLOR_WHITE "\t\tNouvelle date d'expiration (JJ-MM-AAAA) [%s] : " COLOR_RESET, courant->date_expiration);
    lire_chaine(buffer, 11);
    if (strlen(buffer) > 0) {
        if (valider_date(buffer)) {
            strcpy(courant->date_expiration, buffer);
        } else {
            printf(COLOR_RED " Date invalide, ancienne date conservee.\n" COLOR_RESET);
        }
    }

    sauvegarder_medicaments(liste);
    
    clear_screen();
    afficher_entete();
    
    printf("\n\n");
    printf(COLOR_GREEN);
    centrer_texte("=========================================");
    centrer_texte("");
    centrer_texte("Medicament mis a jour avec succes !");
    centrer_texte("");
    centrer_texte("=========================================");
    centrer_texte("");
    printf(COLOR_RESET);
    
    printf(COLOR_YELLOW);
    centrer_texte("Appuyez sur Entree pour continuer...");
    printf(COLOR_RESET);
    attendre_entree();
    return;
}

// Delete medication with admin password verification
int supprimer_medicament(Medicament** liste, int est_admin) {
    if (!(*liste)) {
        printf(COLOR_YELLOW "Aucun medicament a supprimer.\n" COLOR_RESET);
        attendre_entree();
        return 0;
    }

    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN "\n\n\t\t=== SUPPRIMER UN MEDICAMENT ===\n\n" COLOR_RESET);

    int id;
    char nom[100];
    printf(COLOR_WHITE "\t\tEntrez l'ID du medicament a supprimer : " COLOR_RESET);
    id = lire_entier();
    printf(COLOR_WHITE "\t\tEntrez le nom du medicament pour confirmation : " COLOR_RESET);
    lire_chaine(nom, 100);

    Medicament* courant = *liste;
    Medicament* precedent = NULL;

    while (courant && (courant->id != id || strcasecmp_custom(courant->nom, nom) != 0)) {
        precedent = courant;
        courant = courant->next;
    }

    if (!courant) {
        printf(COLOR_RED " Medicament non trouve ou nom incorrect.\n" COLOR_RESET);
        attendre_entree();
        return 0;
    }

    // Admin password verification
    if (!est_admin) {
        printf("\n");
        printf(COLOR_YELLOW);
        centrer_texte("Veuillez entrer le mot de passe admin pour confirmer la suppression.");
        centrer_texte("");
        printf(COLOR_RESET);
        
        char password[30];
        printf(COLOR_WHITE);
        centrer_texte_sans_newline("Mot de passe admin : ");
        printf(COLOR_RESET);
        lire_mot_de_passe(password, 30);
        
        // Check against admin passwords in admins.dat (binary format)
        FILE* file = fopen("bin/data/admins.dat", "rb");
        int valid_password = 0;
        if (file) {
            Admin admin;
            while (fread(&admin, sizeof(Admin), 1, file) == 1) {
                if (strcmp(admin.mot_de_passe, password) == 0) {
                    valid_password = 1;
                    break;
                }
            }
            fclose(file);
        }
        
        if (!valid_password) {
            printf("\n");
            printf(COLOR_RED);
            centrer_texte("Mot de passe incorrect. Suppression annulee.");
            printf(COLOR_RESET);
            attendre_entree();
            return 0;
        }
    }

    // Remove from list
    if (precedent) {
        precedent->next = courant->next;
    } else {
        *liste = courant->next;
    }

    free(courant);
    sauvegarder_medicaments(*liste);
    printf(COLOR_GREEN " Medicament supprime avec succes.\n" COLOR_RESET);
    attendre_entree();
    return 1;
}

// Display one medication
void afficher_medicament(Medicament* med) {
    if (!med) return;
    printf("%-5d | %-20s | %-25s | %-4d | %-8.2f | %-15s | %s\n",
           med->id, med->nom, med->categorie, med->quantite, med->prix, med->fournisseur, med->date_expiration);
}

// Display all medications
void afficher_stock(Medicament* liste) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN);
    centrer_texte("");
    centrer_texte("=== LISTE DES MEDICAMENTS ===");
    centrer_texte("");
    printf(COLOR_RESET);
    
    if (!liste) {
        printf(COLOR_YELLOW "\t\tAucun medicament en stock.\n\n" COLOR_RESET);
        attendre_entree();
        return;
    }
    
    printf(COLOR_GREEN "ID    | Nom                  | Categorie                  | Qte  | Prix (DH) | Fournisseur     | Date Exp.\n" COLOR_RESET);
    printf("------+----------------------+-------------------------+-------+-----------+-----------------+------------\n");
    
    Medicament* courant = liste;
    while (courant) {
        afficher_medicament(courant);
        courant = courant->next;
    }
    
    printf("\n");
    attendre_entree();
}

// Search medication by name (case-insensitive)
void rechercher_medicament_par_nom(Medicament* liste) {
    clear_screen();
    afficher_entete();
    
    printf(COLOR_CYAN);
    centrer_texte("");
    centrer_texte("=== RECHERCHE DE MEDICAMENT ===");
    centrer_texte("");
    printf(COLOR_RESET);
    
    char nom_recherche[100];
    printf(COLOR_WHITE);
    centrer_texte_sans_newline("Entrez le nom du medicament a rechercher : ");
    printf(COLOR_RESET);
    lire_chaine(nom_recherche, sizeof(nom_recherche));

    if (strlen(nom_recherche) == 0) {
        printf(COLOR_YELLOW "Aucun nom saisi.\n" COLOR_RESET);
        attendre_entree();
        return;
    }

    Medicament* courant = liste;
    int trouve = 0;

    printf("\n Resultats pour '%s' :\n", nom_recherche);
    printf(COLOR_GREEN "ID    | Nom                  | Categorie                  | Qte  | Prix (DH) | Fournisseur     | Date Exp.\n" COLOR_RESET);
    printf("------+----------------------+-------------------------+-------+-----------+-----------------+------------\n");

    while (courant != NULL) {
        if (strstr_case_insensitive(courant->nom, nom_recherche)) {
            afficher_medicament(courant);
            trouve = 1;
        }
        courant = courant->next;
    }

    if (!trouve) {
        printf(COLOR_YELLOW "Aucun medicament trouve avec le nom '%s'.\n" COLOR_RESET, nom_recherche);
    }

    printf("\n");
    attendre_entree();
}

// Display medications by category
void afficher_liste_par_categorie(Medicament* liste) {
    int choix;
    
    do {
        clear_screen();
        afficher_entete();
        
        printf(COLOR_CYAN);
        centrer_texte("");
        centrer_texte("=== LISTE PAR CATEGORIE ===");
        centrer_texte("");
        printf(COLOR_RESET);
        printf(COLOR_WHITE);
        centrer_texte("[1] Antibiotiques");
        centrer_texte("[2] Analgesiques");
        centrer_texte("[3] Anti-inflammatoires");
        centrer_texte("[4] Autres");
        printf(COLOR_RESET);
        printf(COLOR_YELLOW);
        centrer_texte("[5] Retour au menu precedent");
        printf(COLOR_RESET);
        printf(COLOR_RED);
        centrer_texte("[6] Retour au menu principal");
        centrer_texte("");
        printf(COLOR_RESET);
        printf(COLOR_WHITE);
        centrer_texte_sans_newline("Votre choix : ");
        printf(COLOR_RESET);
        
        choix = lire_entier();
        
        if (choix >= 1 && choix <= 4) {
            const char *categories[] = {"Antibiotiques", "Analgesiques", "Anti-inflammatoires", "Autres"};
            const char *categorie = categories[choix - 1];
            
            // Build temporary filtered list
            Medicament* liste_filtree = NULL;
            Medicament* courant = liste;
            
            while (courant) {
                int appartient_categorie = 0;
                
                if (choix == 4) {
                    // Pour "Autres", afficher tout sauf les 3 catégories principales
                    if (strcasecmp_custom(courant->categorie, "Antibiotiques") != 0 &&
                        strcasecmp_custom(courant->categorie, "Analgesiques") != 0 &&
                        strcasecmp_custom(courant->categorie, "Anti-inflammatoires") != 0) {
                        appartient_categorie = 1;
                    }
                } else {
                    // Pour les autres catégories, vérification normale
                    if (strcasecmp_custom(courant->categorie, categorie) == 0) {
                        appartient_categorie = 1;
                    }
                }
                
                if (appartient_categorie) {
                    Medicament* nouveau = (Medicament*)malloc(sizeof(Medicament));
                    if (nouveau) {
                        *nouveau = *courant;
                        nouveau->next = liste_filtree;
                        liste_filtree = nouveau;
                    }
                }
                courant = courant->next;
            }
            
            // Display filtered list
            clear_screen();
            afficher_entete();
            
            printf(COLOR_CYAN "\n\n\t\t=== %s ===\n\n" COLOR_RESET, categorie);
            printf(COLOR_GREEN "ID    | Nom                  | Categorie                  | Qte  | Prix (DH) | Fournisseur     | Date Exp.\n" COLOR_RESET);
            printf("------+------------------------+-------------------------+-------+-----------+-----------------+------------\n");
            
            int trouve = 0;
            Medicament* temp = liste_filtree;
            while (temp) {
                printf("%-5d | %-25s | %-25s | %-4d | %-9.2f | %-15s | %s\n",
                       temp->id, temp->nom, temp->categorie, temp->quantite, temp->prix, temp->fournisseur, temp->date_expiration);
                trouve = 1;
                temp = temp->next;
            }
            
            if (!trouve) {
                printf(COLOR_YELLOW "\t\tAucun medicament dans cette categorie.\n" COLOR_RESET);
            }
            
            printf("\n");
            liberer_liste_medicaments(liste_filtree);
            attendre_entree();
        } else if (choix == 5) {
            return;
        } else if (choix == 6) {
            // Return to main menu (handled by caller)
            return;
        } else {
            printf(COLOR_RED "\n\t\tChoix invalide !\n" COLOR_RESET);
            attendre_entree();
        }
    } while (choix != 5 && choix != 6);
}

// Menu for medication management (search + category display)
void menu_gestion_medicaments(Medicament** liste, int est_admin) {
    int choix;
    
    (void)est_admin;  // May be used later for permission checks
    do {
        clear_screen();
        afficher_entete();
        
        printf(COLOR_CYAN);
        centrer_texte("");
        centrer_texte("=== GESTION DES MEDICAMENTS ===");
        centrer_texte("");
        printf(COLOR_RESET);
        printf(COLOR_WHITE);
        centrer_texte("[1] Rechercher un medicament");
        centrer_texte("[2] Liste des medicaments");
        centrer_texte("[3] Retour au menu principal");
        centrer_texte("");
        printf(COLOR_RESET);
        printf(COLOR_WHITE);
        centrer_texte_sans_newline("Votre choix : ");
        printf(COLOR_RESET);
        
        choix = lire_entier();

        switch (choix) {
            case 1:
                rechercher_medicament_par_nom(*liste);
                break;
            case 2:
                {
                    int choix_cat;
                    do {
                        clear_screen();
                        afficher_entete();
                        
                        printf(COLOR_CYAN "\nCategories disponibles :\n" COLOR_RESET);
                        printf("[1] Antibiotiques\n");
                        printf("[2] Analgesiques\n");
                        printf("[3] Anti-inflammatoires\n");
                        printf("[4] Autres...\n");
                        printf("[5] Retour au menu\n");
                        printf("[6] Retour au menu principal\n");
                        printf("Votre choix : ");
                        choix_cat = lire_entier();

                        if (choix_cat >= 1 && choix_cat <= 4) {
                            const char *categories[] = {"Antibiotiques", "Analgesiques", "Anti-inflammatoires", "Autres"};
                            const char *categorie = categories[choix_cat - 1];
                            
                            // Build and display filtered list
                            Medicament* liste_filtree = NULL;
                            Medicament* courant = *liste;
                            
                            while (courant) {
                                int appartient_categorie = 0;
                                
                                if (choix_cat == 4) {
                                    // Pour "Autres", afficher tout sauf les 3 catégories principales
                                    if (strcasecmp_custom(courant->categorie, "Antibiotiques") != 0 &&
                                        strcasecmp_custom(courant->categorie, "Analgesiques") != 0 &&
                                        strcasecmp_custom(courant->categorie, "Anti-inflammatoires") != 0) {
                                        appartient_categorie = 1;
                                    }
                                } else {
                                    // Pour les autres catégories, vérification normale
                                    if (strcasecmp_custom(courant->categorie, categorie) == 0) {
                                        appartient_categorie = 1;
                                    }
                                }
                                
                                if (appartient_categorie) {
                                    Medicament* nouveau = (Medicament*)malloc(sizeof(Medicament));
                                    if (nouveau) {
                                        *nouveau = *courant;
                                        nouveau->next = liste_filtree;
                                        liste_filtree = nouveau;
                                    }
                                }
                                courant = courant->next;
                            }
                            
                            clear_screen();
                            afficher_entete();
                            printf(COLOR_CYAN "\n--- Medicaments : %s ---\n" COLOR_RESET, categorie);
                            afficher_stock(liste_filtree);
                            liberer_liste_medicaments(liste_filtree);
                        } else if (choix_cat == 5) {
                            break;
                        } else if (choix_cat == 6) {
                            choix = 3;
                            break;
                        } else {
                            printf(COLOR_RED " Option invalide.\n" COLOR_RESET);
                            attendre_entree();
                        }
                    } while (choix_cat != 5 && choix_cat != 6);
                }
                break;
            case 3:
                return;
            default:
                printf(COLOR_RED " Option invalide.\n" COLOR_RESET);
                attendre_entree();
        }
    } while (choix != 3);
}

// Menu for stock viewing
void menu_stock(Medicament* liste, int is_admin, const char *user) {
    int choix;
    
    (void)user;  // Unused for now
    
    do {
        clear_screen();
        afficher_entete();
        
        printf(COLOR_CYAN);
        centrer_texte("");
        centrer_texte("=== VOIR LE STOCK ===");
        centrer_texte("");
        printf(COLOR_RESET);
        printf(COLOR_WHITE);
        centrer_texte("[1] Rechercher un medicament");
        centrer_texte("[2] Liste des medicaments");
        centrer_texte("[3] Liste par categorie");
        printf(COLOR_RESET);
        
        if (is_admin) {
            printf(COLOR_MAGENTA);
            centrer_texte("[4] Statistiques de stock");
            printf(COLOR_RESET);
            printf(COLOR_YELLOW);
            centrer_texte("[5] Retour au menu principal");
            centrer_texte("");
            printf(COLOR_RESET);
        } else {
            printf(COLOR_YELLOW);
            centrer_texte("[4] Retour au menu principal");
            centrer_texte("");
            printf(COLOR_RESET);
        }
        
        printf(COLOR_WHITE);
        centrer_texte_sans_newline("Votre choix : ");
        printf(COLOR_RESET);
        
        choix = lire_entier();
        
        switch (choix) {
            case 1:
                rechercher_medicament_par_nom(liste);
                break;
            case 2:
                afficher_stock(liste);
                break;
            case 3:
                afficher_liste_par_categorie(liste);
                break;
            case 4:
                if (is_admin) {
                    // Stock statistics
                    clear_screen();
                    afficher_entete();
                    printf(COLOR_CYAN "\n\n\t\t=== STATISTIQUES DE STOCK ===\n\n" COLOR_RESET);
                    
                    int total_medicaments = 0;
                    float valeur_totale = 0;
                    int count = 0;
                    
                    Medicament* courant = liste;
                    while (courant) {
                        total_medicaments += courant->quantite;
                        valeur_totale += courant->quantite * courant->prix;
                        count++;
                        courant = courant->next;
                    }
                    
                    printf(COLOR_GREEN "\t\tNombre total de medicaments : %d\n" COLOR_RESET, total_medicaments);
                    printf(COLOR_GREEN "\t\tValeur totale du stock : %.2f DH\n" COLOR_RESET, valeur_totale);
                    printf(COLOR_GREEN "\t\tNombre de produits differents : %d\n\n" COLOR_RESET, count);
                    
                    // Category distribution
                    printf(COLOR_YELLOW "\t\tDistribution par categorie :\n" COLOR_RESET);
                    const char *categories[] = {"Antibiotiques", "Analgesiques", "Anti-inflammatoires", "Autres"};
                    for (int c = 0; c < 4; c++) {
                        int qte_cat = 0;
                        courant = liste;
                        while (courant) {
                            if (strcasecmp_custom(courant->categorie, categories[c]) == 0) {
                                qte_cat += courant->quantite;
                            }
                            courant = courant->next;
                        }
                        if (qte_cat > 0) {
                            printf("\t\t• %s : %d unites (%.1f%%)\n", 
                                   categories[c], qte_cat, 
                                   total_medicaments > 0 ? (float)qte_cat / total_medicaments * 100 : 0);
                        }
                    }
                    
                    attendre_entree();
                } else {
                    return;
                }
                break;
            case 5:
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
    } while (choix != (is_admin ? 5 : 4));
}

// Menu for medication CRUD operations
void menu_medicaments(Medicament** liste, const char *user) {
    int choix;
    
    (void)user;  // Unused for now
    
    do {
        clear_screen();
        afficher_entete();
        
        printf(COLOR_CYAN);
        centrer_texte("");
        centrer_texte("=== GESTION DES MEDICAMENTS ===");
        centrer_texte("");
        printf(COLOR_RESET);
        printf(COLOR_GREEN);
        centrer_texte("[1] Ajouter un medicament");
        printf(COLOR_RESET);
        printf(COLOR_YELLOW);
        centrer_texte("[2] Modifier un medicament");
        printf(COLOR_RESET);
        printf(COLOR_RED);
        centrer_texte("[3] Supprimer un medicament");
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
                ajouter_medicament(liste);
                break;
            case 2:
                modifier_medicament(*liste);
                break;
            case 3:
                supprimer_medicament(liste, 0); // Always require admin password
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
