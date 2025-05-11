#include "personnage.h"
#include <stdio.h>
#include "boolean.h"
void creation_personnage(Personnage *p,int x,int y, int largeur, int hauteur) {
    p->x=x;
    p->y=y;
    p->vy=0;
    p->frame=0;
    p->largeur=largeur;
    p->hauteur=hauteur;
    p->timer_vitesse=0;
    char sprites[100];
    for(int i=0;i<5;i++) {
        sprintf(sprites, "dragon%d.bmp", i);
        p->sprites[i] = load_bitmap(sprites, NULL);
        if(p->sprites[i]==NULL) {
            allegro_message("erreurvvv");
            exit(1);
        }
    }//sdvrerever
}
void dessiner_personnage(Personnage *p, BITMAP* buffer) {
    static int compteur_anim = 0;
    static int espacetouche = 0;

    if (key[KEY_SPACE]) {
        espacetouche++;
    } else {
       espacetouche = 0;
    }
    if (!key[KEY_SPACE] && p->vy > 3) {
        stretch_sprite(buffer, p->sprites[3], p->x, p->y, p->largeur, p->hauteur);
        return;
    }

    if (espacetouche >= 5) {
        stretch_sprite(buffer, p->sprites[4], p->x, p->y, p->largeur, p->hauteur);
        return;
    }

    if (key[KEY_SPACE]) {
        compteur_anim++;
        if (compteur_anim >= 12) {
            p->frame = (p->frame + 1) % 3;
            compteur_anim = 0;
        }
    }

    stretch_sprite(buffer, p->sprites[p->frame], p->x, p->y, p->largeur, p->hauteur);
}



int collision_personnage(Personnage* p, BITMAP* fond, float screenx) {
    float offset_x = screenx;
    if ((int)screenx >= fond->w - SCREEN_W) {
        offset_x = fond->w - SCREEN_W;
    }

    for (int dx = 0; dx < p->largeur; dx++) {
        for (int dy = 1; dy < p->hauteur - 3; dy++) {
            int px = (int)(p->x + dx + offset_x);
            int py = p->y + dy;

            if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                int couleur = getpixel(fond, px, py);
                if (getr(couleur) == 0 && getg(couleur) == 0 && getb(couleur) == 0) {
                    return 1; // décor noir → collision
                }
            }
        }
    }
    return 0;
}

int collision_personnage2(Personnage* p, BITMAP* fond, float screenx) {
    int largeur = p->sprites[0]->w;
    int hauteur = p->sprites[0]->h;
    float offset_x = screenx;
    if ((int)screenx >= fond->w - SCREEN_W) {
        offset_x = fond->w - SCREEN_W;
    }
    for (int dx = 0; dx < largeur; dx++) {
        for (int dy = 1; dy < hauteur - 3; dy++) {
            int px = (int)(p->x + dx + offset_x);
            int py = p->y + dy;
            if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                int couleur = getpixel(fond, px, py);
                if (getr(couleur) == 255 && getg(couleur) == 255 && getb(couleur) == 255) {
                    return 1;
                }
            }
        }
    }
    return 0;
}


int saut_possible(Personnage* p, BITMAP* fond, float screenx) {
    int old_y = p->y;
    p->y = old_y - 5;  // on simule le saut
    int possible = !collision_personnage(p, fond, screenx);
    p->y = old_y;      // on revient à la position d'origine
    return possible;
}


void deplacer_personnage(Personnage *p, BITMAP *fond, float screenx, int fin_scroll,
                         int *timer_malus_deplacement, int *timer_bonus_deplacement) {
    int old_x = p->x;
    int old_y = p->y;


    if (*timer_malus_deplacement > 0) {
        (*timer_malus_deplacement)--;

        if (key[KEY_SPACE]) {
            if (p->vy > -3) p->vy -= 1;  // monte moins vite
        } else {
            if (p->vy < 6) p->vy += 1;   // chute normale
        }

        // Sécurité : limite max/min
        if (p->vy > 6) p->vy = 6;
        if (p->vy < -3) p->vy = -3;
    }

    else if (*timer_bonus_deplacement > 0) {
        (*timer_bonus_deplacement)--;

        if (key[KEY_SPACE]) {
            if (p->vy > -8) p->vy -= 2;  // il monte plus vite
        } else {
            if (p->vy < 6) p->vy += 1;   // chute normale
        }

        if (p->vy > 6) p->vy = 6;
        if (p->vy < -8) p->vy = -8;
    }
    else {
        if (key[KEY_SPACE]) {
            if (p->vy > -6) p->vy -= 1;
        } else {
            if (p->vy < 6) p->vy += 1;
        }

        if (p->timer_vitesse > 0) {
            p->vy *= 1.5;
            p->timer_vitesse--;
        } else if (p->timer_vitesse < 0) {
            p->vy *= 0.5;
            p->timer_vitesse++;
        }

        if (p->vy > 8) p->vy = 8;
        if (p->vy < -8) p->vy = -8;
    }

    // ----- POSITION PRÉVISIONNELLE -----
    int tentative_x = p->x;
    int tentative_y = p->y + p->vy;

    if ((int)screenx >= fin_scroll) {
        tentative_x += 2;
    }

    // ----- TEST AVANT MOUVEMENT -----
    p->x = tentative_x;
    p->y = tentative_y;

    if (collision_personnage(p, fond, screenx)) {
        p->x = old_x;
        p->y = old_y;
        p->vy = 0;

        int sorti_du_mur = 0;
        for (int recul = 0; recul < 10; recul++) {
            // Essai 1 : reculer + monter
            p->x -= 1;
            p->y -= 1;
            if (!collision_personnage(p, fond, screenx)) {
                sorti_du_mur = 1;
                break;
            }

            // Essai 2 : reculer + descendre
            p->y += 2;
            if (!collision_personnage(p, fond, screenx)) {
                sorti_du_mur = 1;
                break;
            }

            // Essai 3 : reculer seul
            p->y -= 1;
            if (!collision_personnage(p, fond, screenx)) {
                sorti_du_mur = 1;
                break;
            }
        }

        if (!sorti_du_mur) {
            p->x = old_x;
            p->y = old_y;
            p->vy = 0;
        }
    }

    // ----- LIMITES HAUT / BAS -----
    if (p->y < 0) {
        p->y = 0;
        p->vy = 0;
    }
    if (p->y + p->hauteur > SCREEN_H) {
        p->y = SCREEN_H - p->hauteur;
        p->vy = 0;
    }

    if ((int)screenx < fin_scroll && p->x > SCREEN_W - p->largeur) {
        p->x = SCREEN_W - p->largeur;
    }

    // ----- CORRECTION SOL (remonter jusqu’à sortir) -----
    int count_sol = 0;
    while (collision_personnage(p, fond, screenx) && count_sol < 20) {
        p->y -= 1;
        count_sol++;
    }

    // ----- CORRECTION PLAFOND (descendre s’il est collé) -----
    if (!key[KEY_SPACE]) {
        int count_plafond = 0;
        while (collision_personnage(p, fond, screenx) && count_plafond < 20) {
            p->y += 1;
            p->x -=1;
            count_plafond++;
        }
    }
}















void dessiner_groupe(GrpPersonnages *g, BITMAP *buffer) {
    for (int i = 0; i < g->nb_personnages; i++) {
        dessiner_personnage(&(g->persos[i]), buffer);
    }
}

void deplacer_groupe(GrpPersonnages *g, BITMAP *fond, float screenx, int fin_scroll,int *timer_malus_deplacement,int *timer_bonus_deplacement) {
    for (int i = 0; i < g->nb_personnages; i++) {
        deplacer_personnage(&(g->persos[i]), fond, screenx, fin_scroll,timer_malus_deplacement,timer_bonus_deplacement);
    }
}
bool groupe_est_mort(GrpPersonnages *groupe) {

    int morts = 0;
    for (int i = 0; i < groupe->nb_personnages; i++) {
        if (groupe->persos[i].x + groupe->persos[i].largeur <= 0) {
            morts++;
        }
    }
    return (morts == groupe->nb_personnages);
}
