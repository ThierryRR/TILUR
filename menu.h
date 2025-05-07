#ifndef MENU_H
#define MENU_H
void affichage_ecran_dacceuil();
void scrolling(BITMAP *buffer);
void ecran_menu();
typedef struct {
    char nom[50];
    int niveau;
    int reprise_x;
    int reprise_y;
} Joueur;

int charger_Joueur(Joueur *j);
void sauvegarder_joueur(Joueur *j);
Joueur *saisir_joueur(BITMAP *screen);
int recherche_dans_fichier(Joueur *j);
Joueur *chargement_du_joueur(BITMAP *screen);
Joueur* nouveau_joueur(BITMAP *screen);
void ecran_defaite(Joueur *j);
int ecran_victoireniv1();
int ecran_defaiteniv1();
#endif //MENU_H
