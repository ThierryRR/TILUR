#include <stdio.h>
#include <allegro.h>
#include "jeu.h"
#include "menu.h"
#include "personnage.h"
#include "boolean.h"
#include "scroll.h"
#include "bonus.h"

void jeu_niveau_1(BITMAP *fond_final, Joueur *j) {
    BITMAP *page = create_bitmap(SCREEN_W, SCREEN_H);
    BITMAP *sprite_bonus = load_bitmap("1.bmp", NULL);
    BITMAP *sprite_bonus3 = load_bitmap("2.bmp", NULL);
    BITMAP *bombe0 = load_bitmap("bombe0.bmp", NULL);
    BITMAP *bombe1 = load_bitmap("bombe1.bmp", NULL);
    BITMAP *malusvitesse = load_bitmap("4.bmp", NULL);
    BITMAP *malustaille = load_bitmap("malus.bmp", NULL);// <-- ajout ici
    BITMAP *bonuscomportement = load_bitmap("5.bmp", NULL);
BITMAP *bonuscomport = load_bitmap("6.bmp", NULL);
    if (!bonuscomport||!bonuscomportement||!fond_final || !page || !sprite_bonus || !sprite_bonus3 || !bombe0 || !bombe1 || !malusvitesse || !malustaille) {
        allegro_message("Erreur de chargement des ressources.");
        exit(1);
    }

    GrpPersonnages groupe;
    groupe.nb_personnages = 1;
    int reprise_x = j->reprise_x;
    int reprise_y = j->reprise_y;
    float screenx = reprise_x - SCREEN_W / 2;
    if (screenx < 0) screenx = 0;
    if (screenx > fond_final->w - SCREEN_W) screenx = fond_final->w - SCREEN_W;
    int perso_x = reprise_x - (int)screenx;

    creation_personnage(&groupe.persos[0], perso_x, reprise_y, 64, 64);

    checkpoint cp = creer_checkpoint(600, 430, "drapeau0.bmp", "drapeau1.bmp");
    cp.largeur = cp.sprite[0]->w / 12;
    cp.hauteur = cp.sprite[0]->h / 12;

    BonusPosition mon_bonus1[NB_BONUS] = {
        creer_bonus(200, 300, sprite_bonus, NULL),
        creer_bonus(300, 250, sprite_bonus, NULL),
        creer_bonus(1500, 280, sprite_bonus, NULL)
    };
    BonusPosition mon_bonus2[NB_BONUS] = {
        creer_bonus(1000, 300, bombe0, bombe1),
        creer_bonus(2300, 500, bombe0, bombe1),
        creer_bonus(2000, 280, bombe0, bombe1)
    };
    BonusPosition mon_bonus3[NB_BONUS] = {
        creer_bonus(1000, 300, sprite_bonus3, NULL),
        creer_bonus(4390, 200, sprite_bonus3, NULL),
        creer_bonus(2000, 280, sprite_bonus3, NULL)
    };
    BonusPosition mon_bonus4[NB_BONUS] = {
        creer_bonus(2400, 300, malusvitesse, NULL),
        creer_bonus(4350, 200, malusvitesse, NULL),
        creer_bonus(1000, 280, malusvitesse, NULL)
    };
    BonusPosition malust[NB_BONUS] = {
        creer_bonus(600, 300, malustaille, NULL),
        creer_bonus(3500, 250, malustaille, NULL),
        creer_bonus(4100, 280, malustaille, NULL)
    };
    BonusPosition mon_bonus5[NB_BONUS] = {
        creer_bonus(1200, 500, bonuscomportement, NULL),
        creer_bonus(3500, 250, bonuscomportement, NULL),
        creer_bonus(4100, 280, bonuscomportement, NULL)
    };
BonusPosition mon_bonus6 [NB_BONUS]={
creer_bonus(200, 300, bonuscomport, NULL),
creer_bonus(3520, 250, bonuscomport, NULL),
creer_bonus(4101, 280, bonuscomport, NULL)};
    for (int b = 0; b < NB_BONUS; b++) {
        mon_bonus2[b].largeur = bombe0->w / 12;
        mon_bonus2[b].hauteur = bombe0->h / 12;
        mon_bonus3[b].largeur = sprite_bonus3->w / 12;
        mon_bonus3[b].hauteur = sprite_bonus3->h / 12;

    }

    game_over = false;
    int fin_scroll = fond_final->w - SCREEN_W;
    float dragon_speed = 1.0;
    int dragon_acceleration_timer = 0;
    int dragon_malus_timer = 0;
    int jeu_lance = 0;
    int timer_clones = 0;
    int timer_malus_taille = 0;
    int timer_bonus_taille = 0;
int timer_malus_deplacement=0;
    int timer_bonus_deplacement=0;
    // Écran de démarrage
    clear_bitmap(page);
    blit(fond_final, page, (int)screenx, 0, 0, 0, SCREEN_W, SCREEN_H);
    dessiner_groupe(&groupe, page);
    textout_centre_ex(page, font, "Appuie sur SPACE pour commencer", SCREEN_W / 2, SCREEN_H - 30, makecol(255, 255, 255), -1);
    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    clear_keybuf();

    while (!jeu_lance) {
        poll_keyboard();
        if (key[KEY_ESC]) goto FIN_JEU;
        if (key[KEY_SPACE]) {
            while (key[KEY_SPACE]) poll_keyboard();
            jeu_lance = 1;
        }
        rest(10);
    }

    temps = 0;
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
    install_int_ex(temps_init, BPS_TO_TIMER(60));

    while (!game_over && !key[KEY_ESC]) {
        poll_keyboard();
        if (key[KEY_B]) {
            ecran_menu();
            goto FIN_JEU;
        }

        while (temps > 0) {
            gerer_acceleration(&dragon_speed, &dragon_acceleration_timer, &dragon_malus_timer);
            screenx += dragon_speed;
            if (screenx > fin_scroll) screenx = fin_scroll;

            deplacer_groupe(&groupe, fond_final, screenx, fin_scroll,&timer_malus_deplacement,&timer_bonus_deplacement);
            gerer_bonus_clones(mon_bonus1, &groupe, screenx, &timer_clones);
            gerer_malus_clones(mon_bonus2, &groupe, screenx);
            gerer_bonus_saut(mon_bonus3, &groupe, screenx, &dragon_acceleration_timer);
            gerer_malus_vitesse(mon_bonus4, &groupe, screenx, &dragon_malus_timer);
            gerer_malus_taille(malust, &groupe, screenx, &timer_malus_taille);
gerer_malus_deplacement(mon_bonus5, &groupe, screenx, &timer_malus_deplacement);
            gerer_bonus_deplacement(mon_bonus6,&groupe,screenx,&timer_bonus_taille);
            // Collision avec le checkpoint
            if (collision_checkpoint(&cp, &groupe, &reprise_x, &reprise_y, screenx)) {
                j->reprise_x = cp.x;
                j->reprise_y = cp.y;
                sauvegarder_joueur(j);
            }

            if (groupe_est_mort(&groupe)) game_over = true;

            // Affichage
            int int_screenx = (int)screenx;
            int part1 = fond_final->w - int_screenx;
            int part2 = SCREEN_W - part1;
            clear_bitmap(page);
            if (part1 >= SCREEN_W) {
                blit(fond_final, page, int_screenx, 0, 0, 0, SCREEN_W, SCREEN_H);
            } else {
                blit(fond_final, page, int_screenx, 0, 0, 0, part1, SCREEN_H);
                blit(fond_final, page, 0, 0, part1, 0, part2, SCREEN_H);
            }

            for (int b = 0; b < NB_BONUS; b++) {
                afficher_bonus(mon_bonus1[b], page, screenx);
                afficher_bonus_explosion(mon_bonus2[b], page, screenx);
                afficher_bonus(mon_bonus3[b], page, screenx);
                afficher_bonus(mon_bonus4[b], page, screenx);
                afficher_bonus((mon_bonus5[b]), page, screenx);
                afficher_bonus(malust[b], page, screenx);
                afficher_bonus(mon_bonus6[b], page, screenx);
            }

            afficher_checkpoint(page, cp, int_screenx);
            dessiner_groupe(&groupe, page);
            temps--;
        }

        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }

FIN_NIVEAU:
    remove_int(temps_init);
    rest(200);
    if (game_over) {
        int choix = ecran_defaiteniv1();
        if (choix == 1) jeu_niveau_1(copier_bitmap(fond_final), j);
        else if (choix == 2) ecran_menu();
    } else if (!key[KEY_ESC]) {
        if (j->niveau < 2) {
            j->niveau = 2;
            sauvegarder_joueur(j);
        }
    }

FIN_JEU:
    destroy_bitmap(page);
    destroy_bitmap(sprite_bonus);
    destroy_bitmap(sprite_bonus3);
    destroy_bitmap(bombe0);
    destroy_bitmap(bombe1);
    destroy_bitmap(malusvitesse);
    destroy_bitmap(malustaille);
    destroy_bitmap(fond_final);
}

 // Fin de jeu_niveau_1

void jeu_niveau_2(BITMAP *fond_final, Joueur *j) {
    if (!fond_final) {
        allegro_message("Erreur : fond final non charg\xE9.");
        exit(1);
    }

    BITMAP *page = create_bitmap(SCREEN_W, SCREEN_H);
    if (!page) {
        allegro_message("Erreur ");
        destroy_bitmap(fond_final);
        exit(1);
    }

    BITMAP *sprite_bonus = load_bitmap("1.bmp", NULL);
    BITMAP *sprite_bonus3 = load_bitmap("2.bmp", NULL);
    BITMAP *bombe0 = load_bitmap("bombe0.bmp", NULL);
    BITMAP *bombe1 = load_bitmap("bombe1.bmp", NULL);

    if (!sprite_bonus || !sprite_bonus3 || !bombe0 || !bombe1) {
        allegro_message("Erreur : chargement des images bonus.");
        destroy_bitmap(page);
        destroy_bitmap(fond_final);
        exit(1);
    }

    GrpPersonnages groupe;
    groupe.nb_personnages = 1;
    creation_personnage(&groupe.persos[0], 75, 300, 64, 64);

    BonusPosition mon_bonus1[NB_BONUS] = {
        creer_bonus(800, 300, sprite_bonus, NULL),
        creer_bonus(1300, 250, sprite_bonus, NULL),
        creer_bonus(1700, 280, sprite_bonus, NULL)
    };

    BonusPosition mon_bonus2[NB_BONUS] = {
        creer_bonus(1000, 300, bombe0, bombe1),
        creer_bonus(1300, 500, bombe0, bombe1),
        creer_bonus(2000, 280, bombe0, bombe1)
    };

    BonusPosition mon_bonus3[NB_BONUS] = {
        creer_bonus(2300, 300, sprite_bonus3, NULL),
        creer_bonus(1390, 200, sprite_bonus3, NULL),
        creer_bonus(1000, 280, sprite_bonus3, NULL)
    };

    for (int b = 0; b < NB_BONUS; b++) {
        mon_bonus3[b].largeur = sprite_bonus3->w / 12;
        mon_bonus3[b].hauteur = sprite_bonus3->h / 12;

        mon_bonus2[b].largeur = bombe0->w / 12;
        mon_bonus2[b].hauteur = bombe0->h / 12;
    }


    game_over = false;
    float screenx = 0;
    int screeny = 0;
    int fin_scroll = fond_final->w - SCREEN_W;
    float dragon_speed = 1.0;
    int dragon_acceleration_timer = 0;
    int jeu_lance = 0;
    int timer_clones = 0;
    int dragon_malus_timer = 0;

    clear_bitmap(page);
    blit(fond_final, page, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    dessiner_groupe(&groupe, page);
    textout_centre_ex(page, font, "Appuie sur SPACE pour commencer", SCREEN_W / 2, SCREEN_H - 30, makecol(255, 255, 255), -1);
    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    clear_keybuf();

    while (!jeu_lance) {
        poll_keyboard();
        if (key[KEY_ESC]) {
            destroy_bitmap(page);
            destroy_bitmap(fond_final);
            destroy_bitmap(sprite_bonus);
            destroy_bitmap(sprite_bonus3);
            destroy_bitmap(bombe0);
            destroy_bitmap(bombe1);
            return;
        }
        if (key[KEY_SPACE]) {
            while (key[KEY_SPACE]) poll_keyboard();
            jeu_lance = 1;
        }
        rest(10);
    }

    clear_keybuf();
    temps = 0;
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
    install_int_ex(temps_init, BPS_TO_TIMER(60));

    while (!game_over && !key[KEY_ESC]) {
        poll_keyboard();
        if (key[KEY_SPACE]) dragon_acceleration_timer = 30;

        while (temps > 0) {
            gerer_acceleration(&dragon_speed, &dragon_acceleration_timer,&dragon_malus_timer);
            screenx += dragon_speed;
            if (screenx > fin_scroll) screenx = fin_scroll;

            for (int i = 0; i < groupe.nb_personnages; i++) {
                int pos_abs = (int)screenx + groupe.persos[i].x;
                if (pos_abs >= fond_final->w) {
                    int choix = ecran_victoireniv1();
                    temps = 0;
                    if (choix == 1) {
                        j->niveau = 2;
                        sauvegarder_joueur(j);
                        scrollingNiv2(j);
                    } else {
                        ecran_menu();
                    }
                    goto FIN_NIVEAU;
                }
            }

           // deplacer_groupe(&groupe, fond_final, screenx, fin_scroll,);

            for (int b = 0; b < NB_BONUS; b++) {
                if (collision_bonus(&mon_bonus1[b], groupe.persos[0].x, groupe.persos[0].y,
                                    groupe.persos[0].largeur, groupe.persos[0].hauteur, screenx)) {
                    int clones = (rand() % 5) + 1;
                    for (int i = 0; i < clones && groupe.nb_personnages < MAX_PERSONNAGES; i++) {
                        int dx = 30 * (i + 1), dy = 40 * (i + 1), tentative = 0;
                        while (tentative++ < 10) {
                            int x_try = groupe.persos[0].x - dx;
                            int y_try = groupe.persos[0].y - dy;
                            int px = (int)(x_try + screenx);
                            int py = y_try;
                            int couleur = getpixel(fond_final, px, py);
                            if (!(getr(couleur) == 0 && getg(couleur) == 0 && getb(couleur) == 0)) {creation_personnage(&groupe.persos[groupe.nb_personnages++], x_try, y_try, 64, 64);

                                break;
                            }
                            dy += 10;
                        }
                    }
                    timer_clones = 60;
                }
            }

            for (int b = 0; b < NB_BONUS; b++) {
                for (int i = 0; i < groupe.nb_personnages; i++) {
                    if (collision_bonus(&mon_bonus2[b], groupe.persos[i].x, groupe.persos[i].y,
                                        groupe.persos[i].largeur, groupe.persos[i].hauteur, screenx)) {
                        int n = (rand() % 2) + 1;

                        if (n > groupe.nb_personnages) n = groupe.nb_personnages;
                        for (int j = 0; j < n; j++) {
                            int idx = rand() % groupe.nb_personnages;
                            groupe.persos[idx] = groupe.persos[groupe.nb_personnages - 1];
                            groupe.nb_personnages--;
                        }
                        mon_bonus2[b].explosion_timer = 180;
                        break;
                                        }
                }
            }
            for (int b = 0; b < NB_BONUS; b++) {
                if (mon_bonus2[b].explosion_timer > 0) {
                    mon_bonus2[b].explosion_timer--;
                }
            }


            for (int b = 0; b < NB_BONUS; b++) {
                for (int i = 0; i < groupe.nb_personnages; i++) {
                    if (collision_bonus(&mon_bonus3[b], groupe.persos[i].x, groupe.persos[i].y,
                                        groupe.persos[i].largeur, groupe.persos[i].hauteur, screenx)) {
                        for (int j = 0; j < groupe.nb_personnages; j++) {
                            groupe.persos[j].vy = -7;
                            groupe.persos[j].timer_vitesse = 180;
                        }
                        dragon_acceleration_timer = 180;
                        break;
                    }
                }
            }

            for (int i = 0; i < groupe.nb_personnages; i++) {
                if (collision_personnage(&groupe.persos[i], fond_final, screenx) ||
                    ((int)screenx != fin_scroll && groupe.persos[i].y >= 480)) {
                    groupe.persos[i].x -= 1;
                    groupe.persos[i].vy = 0;
                }
            }

            if (timer_clones > 0) timer_clones--;
            else if (groupe_est_mort(&groupe)) game_over = false;

            int int_screenx = (int)screenx;
            int part1 = fond_final->w - int_screenx;
            int part2 = SCREEN_W - part1;
            clear_bitmap(page);
            if (part1 >= SCREEN_W) {
                blit(fond_final, page, int_screenx, screeny, 0, 0, SCREEN_W, SCREEN_H);
            } else {
                blit(fond_final, page, int_screenx, screeny, 0, 0, part1, SCREEN_H);
                blit(fond_final, page, 0, screeny, part1, 0, part2, SCREEN_H);
            }

            for (int b = 0; b < NB_BONUS; b++) {
                afficher_bonus(mon_bonus1[b], page, screenx);
                afficher_bonus_explosion(mon_bonus2[b], page, screenx);
                afficher_bonus(mon_bonus3[b], page, screenx);

            }


            dessiner_groupe(&groupe, page);
            temps--;
        }

        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }

FIN_NIVEAU:
    remove_int(temps_init);
    rest(200);
    if (game_over) {
        int choix = ecran_defaiteniv1();
        if (choix == 1) scrollingNiv1(j);
        else if (choix == 2) ecran_menu();
        game_over = false;
    } else if (!key[KEY_ESC]) {
        if (j->niveau < 2) {
            j->niveau = 2;
            sauvegarder_joueur(j);
        }
    }

    destroy_bitmap(page);
    destroy_bitmap(fond_final);
    destroy_bitmap(sprite_bonus);
    destroy_bitmap(sprite_bonus3);
    destroy_bitmap(bombe0);
    destroy_bitmap(bombe1);
}
/*
void jeu_niveau_1(BITMAP *fond_final, Joueur *j) {
    if (!fond_final) {
        allegro_message("Erreur : fond final non chargé.");
        exit(1);
    }

    BITMAP *page = create_bitmap(SCREEN_W, SCREEN_H);
    if (!page) {
        allegro_message("Erreur : création du double buffer.");
        destroy_bitmap(fond_final);
        exit(1);
    }

    BITMAP *sprite_bonus = load_bitmap("1.bmp", NULL);
    if (!sprite_bonus) {
        allegro_message("Erreur : chargement bonus");
        destroy_bitmap(page);
        destroy_bitmap(fond_final);
        exit(1);
    }
    BITMAP *sprite_bonus2 = load_bitmap("bombe.bmp", NULL);
    if (!sprite_bonus2) {
        allegro_message("Erreur : chargement bonus");
        destroy_bitmap(page);
        destroy_bitmap(fond_final);
        exit(1);
    }

    BITMAP *sprite_bonus3 = load_bitmap("3.bmp", NULL);
    if (!sprite_bonus3) {
        allegro_message("Erreur : chargement bonus");
        destroy_bitmap(page);
        destroy_bitmap(fond_final);
        exit(1);
    }
    GrpPersonnages groupe;
    groupe.nb_personnages = 1;
    creation_personnage(&groupe.persos[0], 75, 300);
    BonusPosition mon_bonus1[NB_BONUS];
    BonusPosition mon_bonus2[NB_BONUS];
    BonusPosition mon_bonus3[NB_BONUS];

    mon_bonus1[0] = creer_bonus(800, 300, sprite_bonus);
    mon_bonus1[1] = creer_bonus(1300, 250, sprite_bonus);
    mon_bonus1[2] = creer_bonus(1700, 280, sprite_bonus);
    mon_bonus2[0] = creer_bonus(1000, 300, sprite_bonus2);
    mon_bonus2[1] = creer_bonus(1300, 500, sprite_bonus2);
    mon_bonus2[2] = creer_bonus(2000, 280, sprite_bonus2);
    mon_bonus3[0] = creer_bonus(2300, 300, sprite_bonus3);
    mon_bonus3[1] = creer_bonus(1390, 200, sprite_bonus3);
    mon_bonus3[2] = creer_bonus(1000, 280, sprite_bonus3);

    game_over = false;


    float screenx = 0;
    int screeny = 0;
    int fin_scroll = fond_final->w - SCREEN_W;
    float dragon_speed = 1.0;
    int dragon_acceleration_timer = 0;
    int jeu_lance = 0;
    int timer_clones = 0;

    clear_bitmap(page);
    blit(fond_final, page, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    dessiner_groupe(&groupe, page);
    textout_centre_ex(page, font, "Appuie sur SPACE pour commencer", SCREEN_W / 2, SCREEN_H - 30, makecol(255, 255, 255), -1);
    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    clear_keybuf();

    while (!jeu_lance) {
        poll_keyboard();

        if (key[KEY_ESC]) {
            destroy_bitmap(page);
            destroy_bitmap(fond_final);
            destroy_bitmap(sprite_bonus);
            return;
        }

        if (key[KEY_SPACE]) {
            while (key[KEY_SPACE]) {
                poll_keyboard();
                rest(10);
            }
            jeu_lance = 1;
        }

        rest(10);
    }

    clear_keybuf();
    temps = 0;
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
    install_int_ex(temps_init, BPS_TO_TIMER(60));

    while (!game_over && !key[KEY_ESC]) {
        poll_keyboard();

        if (key[KEY_SPACE]) {
            dragon_acceleration_timer = 30;
        }

        while (temps > 0) {
            gerer_acceleration(&dragon_speed, &dragon_acceleration_timer);
            screenx += dragon_speed;
            if (screenx > fin_scroll) screenx = fin_scroll;

            for (int i = 0; i < groupe.nb_personnages; i++) {
                int position_absolue = (int)screenx + groupe.persos[i].x;
                if (position_absolue >= fond_final->w) {
                    int choix = ecran_victoireniv1();
                    temps = 0;
                    if (choix == 1) {
                        j->niveau = 2;
                        sauvegarder_joueur(j);
                        scrollingNiv2(j);
                    } else if (choix == 2) {
                        ecran_menu();
                    }
                    goto FIN_NIVEAU;
                }
            }


            deplacer_groupe(&groupe, fond_final, screenx, fin_scroll);

            for (int b = 0; b < NB_BONUS; b++) {
                if (collision_bonus(&mon_bonus1[b], groupe.persos[0].x, groupe.persos[0].y,
                                    groupe.persos[0].largeur, groupe.persos[0].hauteur, screenx)) {

                    int clones_a_ajouter = (rand() % 5) + 1;
                    for (int i = 0; i < clones_a_ajouter; i++) {
                        if (groupe.nb_personnages < MAX) {
                            int tentative = 0;
                            while (tentative < 10) {
                                int dx = 30 * (i + 1);
                                int dy = 40 * (i + 1);
                                int x_try = groupe.persos[0].x - dx;
                                int y_try = groupe.persos[0].y - dy;

                                int px = (int)(x_try + screenx);
                                int py = y_try;
                                int couleur = getpixel(fond_final, px, py);

                                if (!(getr(couleur) == 0 && getg(couleur) == 0 && getb(couleur) == 0)) {
                                    Personnage *nouveau = &groupe.persos[groupe.nb_personnages];
                                    creation_personnage(nouveau, x_try, y_try);
                                    groupe.nb_personnages++;
                                    break;
                                }
                                tentative++;
                                dy += 10;
                            }

                        }
                    }


                    mon_bonus1[b].x = -1000;
                    mon_bonus1[b].y = -1000;

                    timer_clones = 60;
                                    }
            }

            for (int b = 0; b < NB_BONUS; b++) {
                for (int i = 0; i < groupe.nb_personnages; i++) {
                    if (collision_bonus(&mon_bonus2[b], groupe.persos[i].x, groupe.persos[i].y,
                                        groupe.persos[i].largeur, groupe.persos[i].hauteur, screenx)) {
                        int nb_a_supprimer = (rand() % 2) + 1;
                        if (nb_a_supprimer > groupe.nb_personnages)
                            nb_a_supprimer = groupe.nb_personnages;
                        for (int j = 0; j < nb_a_supprimer; j++) {
                            int index = rand() % groupe.nb_personnages;
                            groupe.persos[index] = groupe.persos[groupe.nb_personnages - 1];
                            groupe.nb_personnages--;
                        }

                        // Supprimer le bonus
                        mon_bonus2[b].x = -1000;
                        mon_bonus2[b].y = -1000;

                        break;
                                        }
                }
            }
            for (int b = 0; b < NB_BONUS; b++) {
                for (int i = 0; i < groupe.nb_personnages; i++) {
                    if (collision_bonus(&mon_bonus3[b], groupe.persos[i].x, groupe.persos[i].y,
                                        groupe.persos[i].largeur, groupe.persos[i].hauteur, screenx)) {

                        // Appliquer le bonus à tous les clones
                        for (int j = 0; j < groupe.nb_personnages; j++) {
                            groupe.persos[j].vy = -7;
                            groupe.persos[j].timer_vitesse = 180;
                        }
                        dragon_acceleration_timer = 180;
                        mon_bonus3[b].x = -1000;
                        mon_bonus3[b].y = -1000;
                        break;
                                        }
                }
            }

            // COLLISIONS
            for (int i = 0; i < groupe.nb_personnages; i++) {
                if (collision_personnage(&groupe.persos[i], fond_final, screenx)) {
                    groupe.persos[i].x -= 1;
                    groupe.persos[i].vy = 0;
                }
                if ((int)screenx != fin_scroll && groupe.persos[i].y >= 480) {
                    groupe.persos[i].x -= 1;
                    groupe.persos[i].vy = 0;
                }
            }

            if (timer_clones > 0) {
                timer_clones--;
            } else {
                if (groupe_est_mort(&groupe)) {
                    game_over = true;
                }
            }

            int int_screenx = (int)screenx;
            int part1 = fond_final->w - int_screenx;
            int part2 = SCREEN_W - part1;

            clear_bitmap(page);

            if (part1 >= SCREEN_W) {
                blit(fond_final, page, int_screenx, screeny, 0, 0, SCREEN_W, SCREEN_H);
            } else {
                blit(fond_final, page, int_screenx, screeny, 0, 0, part1, SCREEN_H);
                blit(fond_final, page, 0, screeny, part1, 0, part2, SCREEN_H);
            }
            for (int b = 0; b < NB_BONUS; b++) {
              afficher_bonus(mon_bonus1[b],page,screenx);
                afficher_bonus(mon_bonus2[b], page, screenx);
                afficher_bonus(mon_bonus3[b], page, screenx);
            }

            dessiner_groupe(&groupe, page);

            temps--;
        }

        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }

FIN_NIVEAU:
    remove_int(temps_init);
    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    rest(200);

    if (game_over) {
        int choix = ecran_defaiteniv1();
        if (choix == 1) {
            scrollingNiv1(j);
        } else if (choix == 2) {
            ecran_menu();
        }
        game_over = false;
    } else if (!key[KEY_ESC]) {
        if (j->niveau < 2) {
            j->niveau = 2;
            sauvegarder_joueur(j);
        }
    }

    destroy_bitmap(page);
    destroy_bitmap(fond_final);
    destroy_bitmap(sprite_bonus);
}*/


void gerer_acceleration(float *dragon_speed, int *dragon_acceleration_timer, int *dragon_malus_timer) {
    if (*dragon_malus_timer > 0) {
        *dragon_speed = 0.75;  // vitesse réduite par malus
        (*dragon_malus_timer)--;
    } else if (*dragon_acceleration_timer > 0) {
        *dragon_speed = 2;  // vitesse boostée
        (*dragon_acceleration_timer)--;
    } else if (key[KEY_SPACE]) {
        *dragon_speed = 1.5;  // vitesse accélérée à la main
    } else {
        *dragon_speed = 1;  // vitesse normale
    }
}
///eedededed

/*////edeeefef
void jeu_niveau_1(BITMAP *fond_final, Joueur *j) {
    if (!fond_final) {
        allegro_message("Erreur : fond final non chargé.");
        exit(1);
    }

    BITMAP *page = create_bitmap(SCREEN_W, SCREEN_H);
    if (!page) {
        allegro_message("Erreur : création du double buffer.");
        destroy_bitmap(fond_final);
        exit(1);
    }

    BITMAP *sprite_bonus = load_bitmap("1.bmp", NULL);
    if (!sprite_bonus) {
        allegro_message("Erreur chargement bonus position.bmp");
        exit(1);
    }

    Personnage perso;
    creation_personnage(&perso, 75  , 300);
    BonusPosition mon_bonus = creer_bonus(800, 300, sprite_bonus);
    game_over = false;

    float screenx = 0;
    int screeny = 0;
    int fin_scroll = fond_final->w - SCREEN_W;
    float dragon_speed = 1.0;
    int dragon_acceleration_timer = 0;

    int jeu_lance = 0;
    clear_bitmap(page);
    blit(fond_final, page, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    dessiner_personnage(&perso, page);
    afficher_bonus(mon_bonus, page, screenx);
    textout_centre_ex(page, font, "Appuie sur SPACE pour commencer", SCREEN_W / 2, SCREEN_H - 30, makecol(255, 255, 255), -1);
    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    clear_keybuf();

    while (1) {
        poll_keyboard();

        if (key[KEY_ESC]) {
            destroy_bitmap(page);
            destroy_bitmap(fond_final);
            destroy_bitmap(sprite_bonus);
            return;
        }

        if (key[KEY_SPACE]) {
            while (key[KEY_SPACE]) {
                poll_keyboard();
                rest(10);
            }
            jeu_lance = 1;
            break;
        }

        rest(10);
    }

    clear_keybuf();
    temps = 0;
    LOCK_VARIABLE(temps);
    LOCK_FUNCTION(temps_init);
    install_int_ex(temps_init, BPS_TO_TIMER(60));

    while (!game_over) {
        poll_keyboard();
        if (key[KEY_SPACE]) {
            dragon_acceleration_timer = 30; // 0.5s à 60 FPS
        }
        if (key[KEY_ESC]) break;

        while (temps > 0) {

            gerer_acceleration(&dragon_speed, &dragon_acceleration_timer);
            screenx += dragon_speed;
            if (screenx > fin_scroll) screenx = fin_scroll;

            int position_absolue = (int)screenx + perso.x;
            if (position_absolue >= fond_final->w) {
                int choix = ecran_victoireniv1();
                temps = 0;
                if (choix == 1) {
                    j->niveau = 2;
                    sauvegarder_joueur(j);
                    scrollingNiv2(j);
                } else if (choix == 2) {
                    ecran_menu();
                }
                goto FIN_NIVEAU;
            }
            deplacer_personnage(&perso, fond_final, screenx, fin_scroll);
            if (jeu_lance &&collision_bonus(&mon_bonus, perso.x, perso.y, perso.largeur, perso.hauteur, screenx)) {
          perso.x += 300;
            }
            if (collision_personnage(&perso, fond_final, screenx)) {
                perso.x -= 1;
                perso.vy = 0;
            }
            if ((int)screenx != fin_scroll && perso.y >= 480) {
                perso.x -= 1;
                perso.vy = 0;
            }

            int int_screenx = (int)screenx;
            int part1 = fond_final->w - int_screenx;
            int part2 = SCREEN_W - part1;

            clear_bitmap(page);
            if (part1 >= SCREEN_W) {
                blit(fond_final, page, int_screenx, screeny, 0, 0, SCREEN_W, SCREEN_H);
            } else {
                blit(fond_final, page, int_screenx, screeny, 0, 0, part1, SCREEN_H);
                blit(fond_final, page, 0, screeny, part1, 0, part2, SCREEN_H);
            }


            afficher_bonus(mon_bonus,page,screenx);
            dessiner_personnage(&perso, page);


            temps--;
        }

        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }

FIN_NIVEAU:
    remove_int(temps_init);


    if (game_over) {
        int choix = ecran_defaiteniv1();
        if (choix == 1) {
            scrollingNiv1(j);
        } else if (choix == 2) {
            ecran_menu();
        }
        game_over = false;
    } else if (!key[KEY_ESC]) {
        if (j->niveau < 2) {
            j->niveau = 2;
            sauvegarder_joueur(j);
        }
    }

    destroy_bitmap(page);
    destroy_bitmap(fond_final);
    destroy_bitmap(sprite_bonus);
}*/