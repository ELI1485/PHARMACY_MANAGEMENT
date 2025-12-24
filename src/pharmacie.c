#include "fonctions.h"

void menu_admin(Admin admin) {
    Medicament* medicaments = NULL;
    Commande commandes[MAX_COMMANDES];
    Employe employes[MAX_EMPLOYES];
    int c_count = 0, e_count = 0;
    int choix;
    
    medicaments = charger_medicaments();
    charger_commandes(commandes, &c_count);
    charger_employes(employes, &e_count);
    
    do {
        afficher_entete_complet(admin.nom, "Administrator");
        
        printf("\n");
        printf(COLOR_CYAN);
        centrer_texte("STOCK MANAGEMENT");
        printf(COLOR_RESET);
        centrer_texte("[1] Voir stock");
        centrer_texte("[2] Gestion des medicaments");
        printf("\n");
        
        printf(COLOR_YELLOW);
        centrer_texte("ORDER MANAGEMENT");
        printf(COLOR_RESET);
        centrer_texte("[3] Passer une commande");
        centrer_texte("[4] Suivi des commandes");
        printf("\n");
        
        printf(COLOR_MAGENTA);
        centrer_texte("ALERTS & REPORTS");
        printf(COLOR_RESET);
        centrer_texte("[5] Notifications");
        centrer_texte("[6] Statistiques");
        printf("\n");
        
        printf(COLOR_GREEN);
        centrer_texte("ADMINISTRATION");
        printf(COLOR_RESET);
        centrer_texte("[7] Mon profile");
        centrer_texte("[8] Gestion des employees");
        centrer_texte("[9] Voir les fournisseurs");
        printf("\n");
        
        afficher_pied_page();
        centrer_texte_sans_newline("Your choice : ");
        
        choix = lire_entier();
        
        // Handle special commands first
        if (choix == -1) {  // 00 = Exit
            clear_screen();
            afficher_entete();
            printf(COLOR_GREEN);
            centrer_texte("");
            centrer_texte("Deconnexion reussie !");
            centrer_texte("");
            centrer_texte("Merci d'avoir utilise MediControl !");
            centrer_texte("");
            printf(COLOR_RESET);
            attendre_entree();
            liberer_liste_medicaments(medicaments);
            return;
        }
        
        switch (choix) {
            case 1:
                menu_stock(medicaments, 1, admin.nom);
                break;
            case 2:
                menu_medicaments(&medicaments, admin.nom);
                break;
            case 3:
                passer_commande(medicaments, commandes, &c_count, admin.nom);
                break;
            case 4:
                menu_suivi_commandes(commandes, &c_count, medicaments, 1, admin.nom);
                break;
            case 5:
                menu_notifications(medicaments, commandes, c_count, admin.nom);
                break;
            case 6:
                menu_statistiques(medicaments, admin.nom);
                break;
            case 7:
                menu_profil(&admin);
                break;
            case 8:
                menu_gerer_employes(employes, &e_count, admin.nom);
                break;
            case 9: {
                Fournisseur fournisseurs[MAX_FOURNISSEURS];
                int f_count = 0;
                charger_fournisseurs(fournisseurs, &f_count);
                afficher_fournisseurs(fournisseurs, f_count);
                break;
            }
            case 0:
                afficher_entete_complet(admin.nom, "Administrator");
                printf(COLOR_BLUE);
                centrer_texte("=== A PROPOS ===\n\n");
                printf(COLOR_RESET);
                centrer_texte("Nom du projet : MediControl\n");
                centrer_texte("Version : 1.0\n");
                printf(COLOR_GREEN);
                centrer_texte("@ENSAH - Developpe par : Arbouni Maroua & Benzerkane Jihane\n");
                printf(COLOR_RESET);
                centrer_texte("Contact support : support@pharmacie-systeme.com\n\n");
                afficher_pied_page();
                attendre_entree();
                break;
            default:
                printf(COLOR_RED "\nChoix invalide !\n" COLOR_RESET);
                attendre_entree();
                break;
        }
        
        // Reload data after operations
        liberer_liste_medicaments(medicaments);
        medicaments = charger_medicaments();
        charger_commandes(commandes, &c_count);
        
    } while (1);
}

void menu_employe(const char *prenom_employe) {
    Medicament* medicaments = NULL;
    Commande commandes[MAX_COMMANDES];
    int c_count = 0;
    int choix;
    
    medicaments = charger_medicaments();
    charger_commandes(commandes, &c_count);
    
    do {
        char titre[100];
        snprintf(titre, sizeof(titre), "%s", prenom_employe);
        afficher_entete_complet(titre, "Employee");
        
        printf("\n");
        printf(COLOR_CYAN);
        centrer_texte("STOCK MANAGEMENT");
        printf(COLOR_RESET);
        centrer_texte("[1] Voir stock");
        centrer_texte("[2] Gestion des medicaments");
        printf("\n");
        
        printf(COLOR_YELLOW);
        centrer_texte("ORDERS");
        printf(COLOR_RESET);
        centrer_texte("[3] Passer une commande");
        centrer_texte("[4] Suivi des commandes");
        printf("\n");
        
        printf(COLOR_MAGENTA);
        centrer_texte("ALERTS");
        printf(COLOR_RESET);
        centrer_texte("[5] Voir les notifications");
        centrer_texte("[6] Mes messages");
        printf(COLOR_RESET);
        
        afficher_pied_page();
        centrer_texte_sans_newline("Your choice : ");
        
        choix = lire_entier();
        
        switch (choix) {
            case 1:
                menu_stock(medicaments, 0, prenom_employe);
                break;
            case 2: {
                char user[100];
                snprintf(user, sizeof(user), "%s", prenom_employe);
                menu_medicaments(&medicaments, user);
                break;
            }
            case 3: {
                char user[100];
                snprintf(user, sizeof(user), "%s", prenom_employe);
                passer_commande(medicaments, commandes, &c_count, user);
                break;
            }
            case 4: {
                char user[100];
                snprintf(user, sizeof(user), "%s", prenom_employe);
                menu_suivi_commandes(commandes, &c_count, medicaments, 0, user);
                break;
            }
            case 5: {
                char user[100];
                snprintf(user, sizeof(user), "%s", prenom_employe);
                menu_notifications(medicaments, commandes, c_count, user);
                break;
            }
            case 6:
                afficher_messages_employe(prenom_employe);
                break;
            case 0:
                afficher_entete_complet(prenom_employe, "Employee");
                printf(COLOR_BLUE);
                centrer_texte("=== A PROPOS ===\n\n");
                printf(COLOR_RESET);
                centrer_texte("Nom du projet : MediControl\n");
                centrer_texte("Version : 1.0\n");
                centrer_texte("Contact support : support@pharmacie-systeme.com\n\n");
                printf(COLOR_GREEN);
                centrer_texte("@ENSAH - Developpe par : Arbouni Maroua & Benzerkane Jihane\n");
                printf(COLOR_RESET);
                afficher_pied_page();
                attendre_entree();
                break;
            default:
                if (choix == -1) {  // 00 = Exit
                    clear_screen();
                    afficher_entete();
                    printf(COLOR_GREEN);
                    centrer_texte("");
                    centrer_texte("Deconnexion reussie !");
                    centrer_texte("");
                    centrer_texte("Merci d'avoir utilise MediControl !");
                    centrer_texte("");
                    printf(COLOR_RESET);
                    attendre_entree();
                    liberer_liste_medicaments(medicaments);
                    return;
                } else if (choix == 9) {  // 9 = Back
                    clear_screen();
                    afficher_entete();
                    printf(COLOR_GREEN);
                    centrer_texte("");
                    centrer_texte("Au revoir !");
                    centrer_texte("");
                    centrer_texte("Merci d'avoir utilise MediControl !");
                    centrer_texte("");
                    printf(COLOR_RESET);
                    attendre_entree();
                    liberer_liste_medicaments(medicaments);
                    return;
                }
                printf(COLOR_RED "\nChoix invalide !\n" COLOR_RESET);
                attendre_entree();
                break;
        }
        
        // Reload data after operations
        liberer_liste_medicaments(medicaments);
        medicaments = charger_medicaments();
        charger_commandes(commandes, &c_count);
        
    } while (1);
}
