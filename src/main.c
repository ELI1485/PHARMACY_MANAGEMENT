#include "fonctions.h"
#ifdef _WIN32
    #include <windows.h>
#endif

int main() {
    // Activer les couleurs dans la console Windows
    system("chcp 65001 > nul");
    
    #ifdef _WIN32
    // Centrer la fenetre console pour resolution 1920x1080
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    
    int console_width = r.right - r.left;
    int console_height = r.bottom - r.top;
    
    // Position pour centrer sur ecran 1920x1080
    int pos_x = (1920 - console_width) / 2;
    int pos_y = (1080 - console_height) / 2;
    
    SetWindowPos(console, 0, pos_x, pos_y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    #endif
    
    Admin admin_connecte;
    int role;
    int tentative = 0;
    int attempted_id = -1;
    
    while (1) {
        clear_screen();
        afficher_entete();
        
        role = afficher_menu_initial();
        
        if (role == 1) { // Pharmacien (Admin)
            tentative = 0;
            attempted_id = -1;
            
            while (tentative < 3) {
                clear_screen();
                afficher_entete();
                
                if (login_admin(&admin_connecte, &attempted_id)) {
                    menu_admin(admin_connecte);
                    break;
                } else {
                    tentative++;
                    printf(COLOR_RED "\n\nIdentifiant ou mot de passe incorrect.\n" COLOR_RESET);
                    
                    if (tentative < 3) {
                        printf("Il vous reste %d tentative(s).\n", 3 - tentative);
                        attendre_entree();
                    }
                }
            }
            
            if (tentative == 3 && attempted_id != -1) {
                bloquer_compte_admin(attempted_id);
                
                clear_screen();
                afficher_entete();
                
                printf(COLOR_RED "\n\n\t\t\t=== COMPTE BLOQUE ===\n\n" COLOR_RESET);
                printf("\t\t\tVotre compte (ID: %d) a ete bloque apres 3 tentatives echouees.\n\n", attempted_id);
                printf(COLOR_YELLOW "\t\t\tPour debloquer votre compte, contactez le support:\n" COLOR_RESET);
                printf("\t\t\tsupport@pharmacie-systeme.com\n\n");
                attendre_entree();
            }
            
        } else if (role == 2) { // Employe
            char prenom_employe[50];
            if (login_employe(prenom_employe)) {
                menu_employe(prenom_employe);
            } else {
                printf(COLOR_RED "\n\nEmploye non trouve!\n" COLOR_RESET);
                attendre_entree();
            }
        } else if (role == 0) { // Quitter
            clear_screen();
            afficher_entete();
            printf(COLOR_GREEN "\n\n\n\t\t\tMerci d'avoir utilise MediControl !\n\n" COLOR_RESET);
            printf("\t\t\tA bientot !\n\n");
            exit(0);
        }
    }
    
    return 0;
}
