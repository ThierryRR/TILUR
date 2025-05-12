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


void deplacer_personnage(Personnage *p, BITMAP *fond, float screenx, int fin_scroll) {
    int old_x = p->x;
    int old_y = p->y;

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
            p->x -= 1;
            p->y -= 1;
            if (!collision_personnage(p, fond, screenx)) {
                sorti_du_mur = 1;
                break;
            }

            p->y += 2;
            if (!collision_personnage(p, fond, screenx)) {
                sorti_du_mur = 1;
                break;
            }

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

    int count_sol = 0;
    while (collision_personnage(p, fond, screenx) && count_sol < 20) {
        p->y -= 1;
        count_sol++;
    }

    if (!key[KEY_SPACE]) {
        int count_plafond = 0;
        while (collision_personnage(p, fond, screenx) && count_plafond < 20) {
            p->y += 1;
            p->x -= 1;
            count_plafond++;
        }
    }
}














void dessiner_groupe(GrpPersonnages *g, BITMAP *buffer) {
    for (int i = 0; i < g->nb_personnages; i++) {
        dessiner_personnage(&(g->persos[i]), buffer);
    }
}

void deplacer_groupe(GrpPersonnages *g, BITMAP *fond, float screenx, int fin_scroll) {
    for (int i = 0; i < g->nb_personnages; i++) {
        deplacer_personnage(&(g->persos[i]), fond, screenx, fin_scroll);
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
bool collision_pic(Personnage* p, BITMAP* fond, float screenx) {
    float offset_x = screenx;
    if (p->timer_pic > 0) return false;
    if ((int)screenx >= fond->w - SCREEN_W) {
        offset_x = fond->w - SCREEN_W;
    }

    for (int dx = 0; dx < p->largeur; dx++) {
        for (int dy = 0; dy < p->hauteur; dy++) {  // toute la hauteur, pas -3
            int px = (int)(p->x + dx + offset_x);
            int py = p->y + dy;

            if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                int couleur = getpixel(fond, px, py);
                if (getr(couleur) == 104 && getg(couleur) == 0 && getb(couleur) == 0) {
                    return true;
                }
            }
        }
    }
    return false;
}
void gerer_collision_pics_groupe(GrpPersonnages *groupe, BITMAP *fond, float screenx) {
    for (int i = 0; i < groupe->nb_personnages; i++) {
        Personnage *p = &(groupe->persos[i]);

        float offset_x = screenx;
        if ((int)screenx >= fond->w - SCREEN_W) {
            offset_x = fond->w - SCREEN_W;
        }

        bool touche_pic = false;
        for (int dx = 0; dx < p->largeur; dx++) {
            for (int dy = 0; dy < p->hauteur; dy++) {
                int px = (int)(p->x + dx + offset_x);
                int py = p->y + dy;
                if (px >= 0 && px < fond->w && py >= 0 && py < fond->h) {
                    int couleur = getpixel(fond, px, py);
                    if (getr(couleur) == 104 && getg(couleur) == 0 && getb(couleur) == 0) {
                        touche_pic = true;
                        break;
                    }
                }
            }
            if (touche_pic) break;
        }

        if (touche_pic) {
            if (p->timer_pic > 0) {
                // Immunisé : bloque le perso comme un mur, mais ne le tue pas
                p->x -= 1;
                p->vy = 0;
            } else {
                // Pas immunisé : le perso est retiré du groupe (mort)
                for (int j = i; j < groupe->nb_personnages - 1; j++) {
                    groupe->persos[j] = groupe->persos[j + 1];
                }
                groupe->nb_personnages--;
                i--; // On ne saute pas le suivant
            }
        }
    }
}
