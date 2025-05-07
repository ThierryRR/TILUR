//
// Created by thier on 22/04/2025.
//

#ifndef BONUS_H
#define BONUS_H

#include <allegro.h>
#include "menu.h"
#include "personnage.h"
#include "scroll.h"
typedef struct {
    int x, y;
    int largeur, hauteur;
    BITMAP *sprite;
    BITMAP *sprite_explosion[2];
    int actif;
    int explosion_timer;

} BonusPosition;
BonusPosition creer_bonus(int x, int y, BITMAP *sprite, BITMAP *explosion);
typedef struct {
    int x, y;
    int largeur, hauteur;
    BITMAP *sprite[2]; // sprite[0] = drapeau rouge, sprite[1] = drapeau vert
    int active; // 0 = pas activé, 1 = activé
} checkpoint;
void gerer_bonus_saut(BonusPosition mon_bonus3[], GrpPersonnages *groupe, int screenx, int *dragon_acceleration_timer) ;
void gerer_malus_vitesse(BonusPosition mon_bonus4[], GrpPersonnages *groupe, int screenx, int *dragon_malus_timer) ;
void gerer_malus_clones(BonusPosition mon_bonus2[], GrpPersonnages *groupe, int screenx);
checkpoint creer_checkpoint(int x, int y, const char* path_rouge, const char* path_vert);
void afficher_checkpoint(BITMAP *buffer, checkpoint cp, int screenx);int collision_checkpoint(checkpoint *cp, GrpPersonnages *groupe, int *reprendre_x, int *reprendre_y, float screenx);
void afficher_bonus(BonusPosition bonus, BITMAP *buffer, float screenx);
int collision_bonus(BonusPosition *bonus, int perso_x, int perso_y, int perso_w, int perso_h, float screenx);
void gerer_bonus_clones(BonusPosition mon_bonus1[], GrpPersonnages *groupe, int screenx, int *timer_clones);
void gerer_malus_taille(BonusPosition malust[], GrpPersonnages*groupe, int screenx, int *timer_malus_taille);
void afficher_bonus_explosion(BonusPosition bonus, BITMAP *buffer, float screenx);
void gerer_malus_deplacement(BonusPosition deplacement[], GrpPersonnages*groupe, int screenx, int *timer_malus_deplacement);
void gerer_bonus_deplacement(BonusPosition deplacement[], GrpPersonnages *groupe, int screenx, int *timer_bonus_deplacement);
#endif //BONUS_H
