#ifndef FONCTIONS_H
#define FONCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_ADMINS 10
#define MAX_MEDICAMENTS 1000
#define MAX_COMMANDES 500
#define MAX_EMPLOYES 50
#define MAX_FOURNISSEURS 100

// Couleurs pour Windows
#define COLOR_RESET "\033[0m"
#define COLOR_GREEN "\033[32m"
#define COLOR_CYAN "\033[36m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED "\033[31m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_WHITE "\033[37m"

// Structures
typedef struct {
    int id;
    char nom[50];
    char mot_de_passe[30];
} Admin;

typedef struct Medicament {
    int id;
    char nom[100];
    char categorie[50];
    int quantite;
    float prix;
    char fournisseur[50];
    char date_expiration[11]; // JJ-MM-AAAA
    struct Medicament* next;
} Medicament;

typedef struct {
    char idC[30];
    int idM;
    char nom_medicament[100];
    int quantite;
    char fournisseur[50];
    char statut[20]; // "En attente", "Livree", "Annulee"
    char date_commande[11];
    int jours_attente;
} Commande;

typedef struct {
    char nom_medicament[100];
    int quantite_vendue;
    char date[11];
    float revenu;
} Vente1;

typedef struct {
    int user_id;
    char user_type[20]; // "Admin" ou "Employe"
    char action[200];
    char date_time[20];
} UserAction;

typedef struct {
    char nom[50];
} Categorie;

#define MAX_CATEGORIES 100
#define MAX_VENTES 1000
#define MAX_ACTIONS 5000

typedef struct {
    char date[11];
    int id_medicament;
    char nom[100];
    int quantite;
    float total;
    int id_employe;
} Vente;

typedef struct {
    int id;
    char nom[50];
    char prenom[50];
    char poste[50];
    char date_embauche[11];
    char telephone[15];
} Employe;

typedef struct {
    int id;
    char nom[50];
    char telephone[20];
    char email[50];
    char adresse[100];
} Fournisseur;

// Fonctions d'authentification
int afficher_menu_initial();
int login_admin(Admin *admin_connecte, int *attempted_id);
int login_employe(char *prenom_employe);
void afficher_message_erreur_support();
void bloquer_compte();
int verifier_compte_bloque(int id);
void bloquer_compte_admin(int id);
void debloquer_compte_admin(int id);

// Fonctions utilitaires
void clear_screen();
void lire_chaine(char *chaine, int taille);
void lire_mot_de_passe(char *password, int taille);
int lire_entier();
float lire_float();
int obtenir_largeur_console();
void centrer_texte(const char *texte);
void centrer_texte_sans_newline(const char *texte);
void afficher_ligne_separatrice(char caractere, int longueur);
void afficher_titre(const char *titre);
void gotoxy(int x, int y);
void setColor(int color);
int valider_date(const char *date);
int valider_quantite(int qte);
int valider_prix(float prix);
void obtenir_date_du_jour(char *buffer);
int comparer_dates(const char *date1, const char *date2);
int jours_entre_dates(const char *date1, const char *date2);
void afficher_entete();
void afficher_entete_complet(const char *user, const char *role);
void afficher_pied_page();
void attendre_entree();

// Nouvelles fonctions utilitaires
int strcasecmp_custom(const char *s1, const char *s2);
int strstr_case_insensitive(const char *haystack, const char *needle);
void get_current_date(char *date_str);
void get_current_datetime(char *datetime_str);
int is_date_expired(const char *date_str);
void log_user_action(int user_id, const char *user_type, const char *action);
void log_user_login(int user_id, const char *user_type);
int charger_categories(Categorie categories[], int max_categories);
void sauvegarder_categorie(const char *nom_categorie);

// Fonctions de gestion des médicaments
Medicament* charger_medicaments();
void sauvegarder_medicaments(Medicament* liste);
void afficher_liste_par_categorie(Medicament* liste);
void rechercher_medicament_par_nom(Medicament* liste);
void ajouter_medicament(Medicament** liste);
void modifier_medicament(Medicament* liste);
int supprimer_medicament(Medicament** liste, int est_admin);
void afficher_stock(Medicament* liste);
void afficher_medicament(Medicament* med);
Medicament* creer_medicament();
void liberer_liste_medicaments(Medicament* liste);
void menu_gestion_medicaments(Medicament** liste, int est_admin);

// Fonctions de gestion des commandes
void charger_commandes(Commande commandes[], int *count);
void sauvegarder_commandes(Commande commandes[], int count);
void passer_commande(Medicament* liste, Commande commandes[], int *c_count, const char *user);
void marquer_commande_livree(Commande commandes[], int count, Medicament* liste);
void annuler_commande(Commande commandes[], int count);
void afficher_suivi_commandes(Commande commandes[], int count);
void generer_id_commande(char *idC);

// Fonctions de notifications
void verifier_ruptures(Medicament* liste);
void verifier_stocks_faibles(Medicament* liste);
void verifier_commandes_en_retard(Commande commandes[], int count);
void verifier_medicaments_perimes(Medicament* liste);
void afficher_notifications(Medicament* liste, Commande commandes[], int c_count);

// Fonctions statistiques
void afficher_gains_jour();
void afficher_gains_mensuels();
void afficher_ventes_jour();
void afficher_produits_plus_vendus();
void afficher_produits_en_rupture(Medicament* liste);
void afficher_chiffre_affaires_annuel();
void afficher_pertes(Medicament* liste);
void menu_statistiques(Medicament* liste, const char *user);

// Fonctions profil
void afficher_profil(Admin admin);
void changer_mot_de_passe(Admin *admin);
void changer_nom_admin(Admin *admin);
void sauvegarder_profil(Admin admin);
void menu_profil(Admin *admin);

// Fonctions employes
void charger_employes(Employe employes[], int *count);
void sauvegarder_employes(Employe employes[], int count);
void ajouter_employe(Employe employes[], int *count);
void modifier_employe(Employe employes[], int count);
void supprimer_employe(Employe employes[], int *count);
void afficher_liste_employes(Employe employes[], int count);
void envoyer_message_employe();
void afficher_messages_employe(const char *prenom_employe);
void menu_gerer_employes(Employe employes[], int *count, const char *user);

// Fonctions fournisseurs
void charger_fournisseurs(Fournisseur fournisseurs[], int *count);
void sauvegarder_fournisseurs(Fournisseur fournisseurs[], int count);
void afficher_fournisseurs(Fournisseur fournisseurs[], int count);

// Menus
void menu_admin(Admin admin);
void menu_employe(const char *prenom_employe);
void menu_stock(Medicament* liste, int is_admin, const char *user);
void menu_medicaments(Medicament** liste, const char *user);
void menu_notifications(Medicament* liste, Commande commandes[], int c_count, const char *user);
void menu_suivi_commandes(Commande commandes[], int *count, Medicament* liste, int is_admin, const char *user);

#endif
