#include <stdio.h>
#include <allegro.h>
#include "menu.h"
#include "scroll.h"
#include "boolean.h"
#include "bonus.h"
#include "personnage.h"
void initialisation_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(desktop_color_depth());
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0) != 0) {
        allegro_message("Problème mode graphique");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
}
int main() {
    initialisation_allegro();
    int quitter_jeu = 0;
    Joueur *j = NULL;
 /*   // Création d'un checkpoint
    checkpoint cp = creer_checkpoint(500, 300, "drapeau0.bmp", "drapeau1.bmp");
    cp.largeur = cp.sprite[0]->w / 12;
    cp.hauteur = cp.sprite[0]->h / 12;
    int perso_x=405;
    int perso_y= 305;
    GrpPersonnages groupe;
    int  reprise_x =500;
    int  reprise_y =300;
    collision_checkpoint(&cp, &groupe, &reprise_x, &reprise_y);
    quitter_jeu = 0;
    getchar();getchar();*/

    while (!quitter_jeu) {
        affichage_ecran_dacceuil();
        ecran_menu();
        while (!key[KEY_ESC]) {
            if (game_over) {
                ecran_defaite(j);
                game_over = false;
            }
            rest(10);
        }
        if (key[KEY_ESC]) {
            quitter_jeu = 1;
        }
    }
    allegro_exit();
    return 0;
}END_OF_MAIN();


