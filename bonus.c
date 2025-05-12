#include "bonus.h"
#include <math.h>
#include <stdio.h>//
#include "jeu.h"
#include "bonus.h"
#include <allegro.h>
#include <stdio.h>

BonusPosition creer_bonus(int x, int y, BITMAP *sprite, BITMAP *explosion) {
    BonusPosition bonus;
    bonus.x = x;
    bonus.y = y;
    bonus.largeur = sprite->w / 4;
    bonus.hauteur = sprite->h / 4;
    bonus.sprite = sprite;
    bonus.sprite_explosion[0] = sprite;
    bonus.sprite_explosion[1] = explosion;
    bonus.explosion_timer = 0;
    bonus.actif = 1;
    return bonus;
}

void afficher_bonus(BonusPosition bonus, BITMAP *buffer, float screenx) {
    if (!bonus.actif && bonus.explosion_timer <= 0) return;

    int ecran_x = bonus.x - (int)screenx;
    int largeur, hauteur;
    BITMAP *img = NULL;

    if (bonus.explosion_timer > 0 && bonus.sprite_explosion[1]) {
        img = bonus.sprite_explosion[1];  // Explosion
        largeur = bonus.largeur * 2;
        hauteur = bonus.hauteur * 2;
        ecran_x -= (largeur - bonus.largeur) / 2;
    } else if (bonus.actif && bonus.sprite) {
        img = bonus.sprite;  // Bombe normale
        largeur = bonus.largeur;
        hauteur = bonus.hauteur;
    } else {
        return;
    }

    if (img) {
        stretch_sprite(buffer, img, ecran_x, bonus.y, largeur, hauteur);
    }
}

int collision_bonus(BonusPosition *bonus, int perso_x, int perso_y, int perso_w, int perso_h, float screenx) {
    if (!bonus->actif) return 0;

    int bonus_ecran_x = bonus->x - (int)screenx;
    int marge = 20;
    int zone_x = bonus_ecran_x - marge / 2;
    int zone_y = bonus->y - marge / 2;
    int zone_largeur = bonus->largeur + marge;
    int zone_hauteur = bonus->hauteur + marge;

    if (perso_x + perso_w > zone_x &&
        perso_x < zone_x + zone_largeur &&
        perso_y + perso_h > zone_y &&
        perso_y < zone_y + zone_hauteur) {
        bonus->actif = 0;
        bonus->explosion_timer = 60; // 1 seconde
        return 1;
        }

    return 0;
}


checkpoint creer_checkpoint(int x, int y, const char* path_rouge, const char* path_vert) {
    checkpoint cp;
    cp.x = x;
    cp.y = y;
    cp.sprite[0] = load_bitmap(path_rouge, NULL);
    cp.sprite[1] = load_bitmap(path_vert, NULL);
    cp.active = 0;

    if (cp.sprite[0]) {
        cp.largeur = cp.sprite[0]->w / 12;
        cp.hauteur = cp.sprite[0]->h / 12;
    } else {
        cp.largeur = 20;
        cp.hauteur = 40;
    }

    return cp;
}

void afficher_checkpoint(BITMAP *buffer, checkpoint cp, int screenx) {
    int frame = cp.active ? 1 : 0;
    if (cp.sprite[frame]) {
        int largeur = cp.sprite[frame]->w / 12;
        int hauteur = cp.sprite[frame]->h / 12;
        stretch_sprite(buffer, cp.sprite[frame], cp.x - screenx, cp.y, largeur, hauteur);
    }
}
int collision_checkpoint(checkpoint *cp, GrpPersonnages *groupe, int *reprendre_x, int *reprendre_y, float screenx) {
    if (cp->active) return 0;

    int marge = 20;
    int zone_x = cp->x - marge / 2;
    int zone_y = cp->y - marge / 2;
    int zone_largeur = cp->largeur + marge;
    int zone_hauteur = cp->hauteur + marge;

    for (int i = 0; i < groupe->nb_personnages; i++) {
        Personnage *p = &groupe->persos[i];

        int perso_x_monde = p->x + (int)screenx;
        int perso_y_monde = p->y;


        if (perso_x_monde + p->largeur > zone_x &&
            perso_x_monde < zone_x + zone_largeur &&
            perso_y_monde + p->hauteur > zone_y &&
            perso_y_monde < zone_y + zone_hauteur) {

            cp->active = 1;
            *reprendre_x = cp->x;
            *reprendre_y = cp->y;

            return 1;
            }
    }

    return 0;
}



void gerer_malus_clones(BonusPosition mon_bonus2[], GrpPersonnages *groupe, int screenx) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus2[b], groupe->persos[i].x, groupe->persos[i].y, groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                int n = (rand() % 4) + 1;
                if (n > groupe->nb_personnages) n = groupe->nb_personnages;
                for (int j = 0; j < n; j++) {
                    int idx = rand() % groupe->nb_personnages;
                    groupe->persos[idx] = groupe->persos[groupe->nb_personnages - 1];
                    groupe->nb_personnages--;
                }
                mon_bonus2[b].explosion_timer = 180;
                break;
            }
        }
    }

    for (int b = 0; b < NB_BONUS; b++) {
        if (mon_bonus2[b].explosion_timer > 0) mon_bonus2[b].explosion_timer--;
    }
}void gerer_bonus_clones(BonusPosition mon_bonus1[], GrpPersonnages *groupe, int screenx, int *timer_clones) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus1[b], groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                int clones = (rand() % 5) + 1;
                Personnage *original = &groupe->persos[i];
                for (int j = 0; j < clones && groupe->nb_personnages < MAX_PERSONNAGES; j++) {
                    int dx = 20 * j;
                    creation_personnage(&groupe->persos[groupe->nb_personnages++],
                                        original->x + dx, original->y, original->largeur, original->hauteur);
                }
                *timer_clones = 60;
                break; // un seul déclenchement par bonus
                                }
        }
    }
}





void gerer_bonus_saut(BonusPosition mon_bonus3[], GrpPersonnages *groupe, int screenx, int *dragon_acceleration_timer) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus3[b], groupe->persos[i].x, groupe->persos[i].y, groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    *dragon_acceleration_timer = 180;
                }

                break;
            }
        }
    }
}
void gerer_malus_vitesse(BonusPosition mon_bonus4[], GrpPersonnages *groupe, int screenx, int *dragon_malus_timer) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus4[b], groupe->persos[i].x, groupe->persos[i].y, groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    *dragon_malus_timer = 180;
                }

                break;
            }
        }
    }
}

void gerer_taille_petit(BonusPosition malust[], GrpPersonnages*groupe, int screenx, int *timer_malus_taille) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&malust[b], groupe->persos[i].x, groupe->persos[i].y, groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    groupe->persos[j].largeur /= 2;
                    groupe->persos[j].hauteur /= 2;
                }
                *timer_malus_taille = 300;
                break;
            }
        }
    }

    if (*timer_malus_taille > 0) {
        (*timer_malus_taille)--;
        if (*timer_malus_taille == 0) {
            for (int j = 0; j < groupe->nb_personnages; j++) {
                groupe->persos[j].largeur *= 2;
                groupe->persos[j].hauteur *= 2;
            }
        }
    }
}

void gerer_taille_grand (BonusPosition bonust[], GrpPersonnages *groupe, int screenx, int *timer_bonus_taille) {
    for (int b = 0; b < NB_BONUS; b++) {
        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&bonust[b], groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur, screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    groupe->persos[j].largeur *= 2;
                    groupe->persos[j].hauteur *= 2;
                }
                *timer_bonus_taille = 300;
                break;
                                }
        }
    }

    // Réduction après le délai
    if (*timer_bonus_taille > 0) {
        (*timer_bonus_taille)--;
        if (*timer_bonus_taille == 0) {
            for (int j = 0; j < groupe->nb_personnages; j++) {
                groupe->persos[j].largeur /= 2;
                groupe->persos[j].hauteur /= 2;
            }
        }
    }
}

/*
void gerer_malus_vitesse(BonusPosition mon_bonus4[], GrpPersonnages *groupe, int screenx, int *dragon_malus_timer) {
    for (int b = 0; b < NB_BONUS; b++) {
        if (!mon_bonus4[b].actif) continue;

        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus4[b],
                                groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur,
                                screenx)) {

                // Effet sur tous les personnages
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    groupe->persos[j].timer_vitesse = -180;  // valeur négative = malus ?
                }

                // Effet global sur scroll
                *dragon_malus_timer = 180;

                break;
                                }
        }
    }
}*/
void gerer_bonus_deplacement(BonusPosition deplacement[], GrpPersonnages *groupe, int screenx, int *timer_bonus_deplacement) {
    for (int b = 0; b < NB_BONUS; b++) {
        if (!deplacement[b].actif) continue;

        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&deplacement[b],
                                groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur,
                                screenx)) {

                *timer_bonus_deplacement = 500;  // effet global
                break;
                                }
        }
    }
}



void afficher_bonus_explosion(BonusPosition bonus, BITMAP *buffer, float screenx) {
    int largeur = bonus.largeur;
    int hauteur = bonus.hauteur;
    int x_affiche = bonus.x - (int)screenx;
    int y_affiche = bonus.y;

    if (bonus.explosion_timer > 0 && bonus.sprite_explosion[1]) {
        // Afficher l'explosion pendant 1 seconde (60 frames)
        stretch_sprite(buffer, bonus.sprite_explosion[1], x_affiche, y_affiche, largeur, hauteur);
    } else if (bonus.sprite) {
        // Affichage par défaut
        stretch_sprite(buffer, bonus.sprite, x_affiche, y_affiche, largeur, hauteur);
    }
}



void gerer_bonus_immunite_pic(BonusPosition mon_bonus5[], GrpPersonnages *groupe, int screenx) {
    for (int b = 0; b < NB_BONUS; b++) {
        if (!mon_bonus5[b].actif) continue;

        for (int i = 0; i < groupe->nb_personnages; i++) {
            if (collision_bonus(&mon_bonus5[b],
                                groupe->persos[i].x, groupe->persos[i].y,
                                groupe->persos[i].largeur, groupe->persos[i].hauteur,
                                screenx)) {
                for (int j = 0; j < groupe->nb_personnages; j++) {
                    groupe->persos[j].timer_pic = 300;
                }
                break;
                                }
        }
    }

    // Décrément timer
    for (int i = 0; i < groupe->nb_personnages; i++) {
        if (groupe->persos[i].timer_pic > 0)
            groupe->persos[i].timer_pic--;
    }
}


