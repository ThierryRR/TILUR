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

    affichage_ecran_dacceuil();
    while (!quitter_jeu) {

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


