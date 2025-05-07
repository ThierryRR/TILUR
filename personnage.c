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
    int largeur = 60;
    int hauteur = 60;
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
                if (getr(couleur) == 0 && getg(couleur) == 0 && getb(couleur) == 0) {
                    return 1;
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


void deplacer_personnage(Personnage *p, BITMAP *fond, float screenx, int fin_scroll,int *timer_malus_deplacement,int *timer_bonus_deplacement) {
    int new_x = p->x;
    int new_y = p->y;

    int pied_x = (int)(p->x + screenx + p->largeur / 2);
    int pied_y = p->y + p->hauteur;

    int couleur_sol = getpixel(fond, pied_x, pied_y);

    // Si le pixel sous le perso est noir ET qu'on n'appuie pas sur espace
    if ((getr(couleur_sol) == 0 && getg(couleur_sol) == 0 && getb(couleur_sol) == 0) && !key[KEY_SPACE]) {
        p->vy = 0;
        p->x -= 1;
        return;
    }

    // Gestion du saut
    if (key[KEY_SPACE]) {
        if (saut_possible(p, fond, screenx)) {
            p->vy = -5;
            if (p->timer_vitesse > 0) {
                p->vy = -7;
            } else {
                p->vy = -4;
            }
        } else {
            p->vy = 2;
            p->x -= 2;
        }
    } else {
        p->vy += 1;
    }

    if (p->vy > 10) {
        p->vy = 10;
    }

    // Gestion du déplacement horizontal
    int vx = 0;
    if ((int)screenx >= fin_scroll) {
        vx = 2;
        if (key[KEY_SPACE] && !saut_possible(p, fond, screenx)) {
            p->vy += 3;
            vx -= 3;
        }
    }



    new_y += p->vy;
    new_x += vx;
    // Sauvegarder ancienne position
    int old_x = p->x;
    int old_y = p->y;

    p->x = new_x;
    p->y = new_y;

    if (collision_personnage(p, fond, screenx)) {
        p->x = old_x;
        if (key[KEY_SPACE] || p->vy < 0) {
            p->y = old_y;
            p->vy = 0;
        }
    }

    p->x = new_x;
    if (collision_personnage(p, fond, screenx)) {
        p->x = old_x;
    }

    // Bords haut et bas de l'écran
    if (p->y < 0) {
        p->y = 0;
        p->vy = 0;
    }
    if (p->y > SCREEN_H - p->hauteur) {
        p->y = SCREEN_H - p->hauteur;
        p->vy = 0;
    }

    // Bords droits (tant que le scroll n’est pas fini)
    if ((int)screenx < fin_scroll && p->x > SCREEN_W - p->largeur) {
        p->x = SCREEN_W - p->largeur;
    }

    // Timer de vitesse
    if (p->timer_vitesse > 0) {
        p->timer_vitesse--;
    }
    if (*timer_malus_deplacement > 0) {
        p->vy = 3;
(*timer_malus_deplacement)--;
        // On avance sans appliquer le saut ni gravité
        int new_y = p->y + p->vy;
        int old_y = p->y;
        p->y = new_y;

        if (collision_personnage(p, fond, screenx)) {
            p->y = old_y;
        }

        return;
    }
    if (*timer_bonus_deplacement > 0) {
(*timer_bonus_deplacement)--;
        p->vy += 1;
        if (p->vy > 6) p->vy = 6;
        int new_y = p->y + p->vy;
        int old_y = p->y;
        p->y = new_y;

        if (collision_personnage(p, fond, screenx)) {
            p->y = old_y;
            p->vy = 0;
        }

        return;
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
