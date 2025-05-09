
#include <allegro.h>
#include "menu.h"
#include "stdio.h"
#include "scroll.h"
#include "boolean.h"



bool game_over = false;

void affichage_ecran_dacceuil(Joueur **j) {
    BITMAP *image = load_bitmap("badlandecran.bmp", NULL);
    if (image == NULL) {
        allegro_message("Erreur chargement badlandecran.bmp");
        exit(1);
    }

    blit(image, screen, 0, 0, 0, 0, image->w, image->h);

    while (!key[KEY_SPACE] && !key[KEY_ESC]) {
        poll_keyboard();
        rest(10);
    }

    destroy_bitmap(image);
    clear_to_color(screen, makecol(0, 0, 0));

    if (key[KEY_ESC]) {
        game_over = true;
    }
}void ecran_menu() {
    Joueur *j = NULL;
    BITMAP *image = load_bitmap("badlandmenu.bmp", NULL);
    if (image == NULL) {
        allegro_message("Erreur chargement badlandmenu.bmp");
        exit(1);
    }

    game_over = false;
    show_mouse(screen);

    while (!key[KEY_ESC] && !game_over) {
        j = NULL;
        blit(image, screen, 0, 0, 0, 0, image->w, image->h);
        poll_keyboard();
        if (key[KEY_ENTER] && j != NULL) {
            clear_keybuf();
            scrollingNiv1(j);
            show_mouse(screen);
            continue;
        }

        static int compteur_demo = 1;

        if (key[KEY_1]) {
            Joueur *demo = malloc(sizeof(Joueur));
            sprintf(demo->nom, "DEMO%d", compteur_demo++);
            demo->niveau = 1;
            demo->reprise_x = 75;
            demo->reprise_y = 300;

            clear_keybuf();
            scrollingNiv1(demo);
            free(demo);
            show_mouse(screen);
            continue;
        }

        if (key[KEY_B]) {
            affichage_ecran_dacceuil(&j);
            if (game_over) break;
            clear_keybuf();
            show_mouse(screen);
            continue;
        }

        if (mouse_b & 1) {
            // 🎮 Création d’un nouveau joueur
            if ((mouse_x >= 125) && (mouse_x <= 678) && (mouse_y >= 225) && (mouse_y <= 320)) {
                Joueur *temp = nouveau_joueur(screen);
                if (temp == NULL) {
                    clear_keybuf();
                    show_mouse(screen);
                    continue;
                }
                j = temp;
                scrollingNiv1(j);

                game_over = false;
                show_mouse(screen);
                clear_keybuf();
                continue;
            }
            if ((mouse_x >= 95) && (mouse_x <= 703) && (mouse_y >= 342) && (mouse_y <= 440)) {
                j = chargement_du_joueur(screen);
                if (j == NULL) {
                    clear_keybuf();
                    show_mouse(screen);
                    continue;
                }

                if (j->niveau == 1) {
                    scrollingNiv1(j);
                } else if (j->niveau == 2) {
                    scrollingNiv2(j);
                } else {
                    allegro_message("Niveau inconnu");
                }

                game_over = false;
                show_mouse(screen);
                clear_keybuf();
                continue;
            }
        }

        rest(10);
    }

    destroy_bitmap(image);
    clear_to_color(screen, makecol(0, 0, 0));
}





Joueur* chargement_du_joueur(BITMAP* screen) {
    FILE *pf = fopen("joueur.txt", "r");
    if (pf == NULL) {
        allegro_message("Erreur ouverture joueur.txt");
        return NULL;
    }

    char noms[100][30];
    int niveaux[100], xs[100], ys[100];
    int nb = 0;
    while (fscanf(pf, "%s %d %d %d", noms[nb], &niveaux[nb], &xs[nb], &ys[nb]) == 4 && nb < 100) {
        nb++;
    }
    fclose(pf);

    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!buffer) {
        allegro_message("Erreur buffer");
        return NULL;
    }

    BITMAP *fond = load_bitmap("fond.bmp", NULL);
    if (!fond) {
        allegro_message("Erreur chargement fond.bmp");
        destroy_bitmap(buffer);
        return NULL;
    }

    int ligne_hauteur = 50;
    int start_y = 150;
    int case_x = SCREEN_W - 100;
    int case_l = 30;
    int decalage_y = 20;

    while (1) {
        poll_keyboard();
        if (key[KEY_ESC]) break;
        if (key[KEY_SPACE]) {
            show_mouse(NULL);
            destroy_bitmap(buffer);
            destroy_bitmap(fond);
            return NULL;

        }

        draw_sprite(buffer, fond, 0, 0);
        textout_ex(buffer, font, "Clique sur une case pour sélectionner un joueur", 50, 50, makecol(255,255,255), -1);

        for (int i = 0; i < nb; i++) {
            int ligne_y = start_y + i * ligne_hauteur;
            int texte_y = ligne_y + decalage_y;
            int case_y = ligne_y + (ligne_hauteur - case_l) / 2;

            textout_ex(buffer, font, noms[i], 50, texte_y, makecol(255,255,255), -1);
            char niveau_str[20];
            sprintf(niveau_str, "Niv %d", niveaux[i]);
            textout_ex(buffer, font, niveau_str, 300, texte_y, makecol(255,255,255), -1);

            rect(buffer, case_x, case_y, case_x + case_l, case_y + case_l, makecol(255,255,255));
        }

        show_mouse(buffer);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        if (mouse_b & 1) {
            for (int i = 0; i < nb; i++) {
                int ligne_y = start_y + i * ligne_hauteur;
                int case_y = ligne_y + (ligne_hauteur - case_l) / 2;

                if (mouse_x >= case_x && mouse_x <= case_x + case_l &&
                    mouse_y >= case_y && mouse_y <= case_y + case_l) {

                    Joueur* j = malloc(sizeof(Joueur));
                    if (!j) {
                        destroy_bitmap(buffer);
                        destroy_bitmap(fond);
                        allegro_message("Erreur allocation mémoire joueur");
                        return NULL;
                    }

                    strcpy(j->nom, noms[i]);
                    j->niveau = niveaux[i];
                    j->reprise_x = xs[i];
                    j->reprise_y = ys[i];

                    destroy_bitmap(fond);
                    destroy_bitmap(buffer);
                    clear_keybuf();
                    return j;
                }
            }
        }

        rest(10);
    }

    destroy_bitmap(fond);
    destroy_bitmap(buffer);
    clear_keybuf();
    return NULL;
}

void ecran_defaite(Joueur *j) {
    BITMAP *image = load_bitmap("ecran_defaite.bmp", NULL);
    if (!image) {
        allegro_message("Erreur chargement ecran_defaite.bmp");
        exit(1);
    }
    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!buffer) {
        destroy_bitmap(image);
        allegro_message("Erreur création du double buffer");
        exit(1);
    }
    clear_keybuf();
    rest(200);

    while (1) {
        poll_keyboard();
        blit(image, buffer, 0, 0, 0, 0, image->w, image->h);

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        if (keypressed()) {
            int k = readkey();
            int touche = k >> 8;
            if (touche == KEY_V) {
                destroy_bitmap(buffer);
                destroy_bitmap(image);
                clear_keybuf();
                scrollingNiv1(j);
                return;
            }
            if (touche == KEY_ESC) {
                destroy_bitmap(buffer);
                destroy_bitmap(image);
                clear_keybuf();
                game_over = false;
                return;
            }
        }

        rest(10);
    }
}
int sauvegarder_joueur(Joueur *j) {
    if (strncmp(j->nom, "DEMO", 4) == 0) return 1;

    FILE *pf = fopen("joueur.txt", "r");
    char lignes[100][50];
    int niveaux[100], xs[100], ys[100];
    int nb_joueurs = 0, modifie = 0;

    if (pf != NULL) {
        while (fscanf(pf, "%s %d %d %d", lignes[nb_joueurs], &niveaux[nb_joueurs], &xs[nb_joueurs], &ys[nb_joueurs]) == 4) {
            if (strcmp(j->nom, lignes[nb_joueurs]) == 0) {
                niveaux[nb_joueurs] = j->niveau;
                xs[nb_joueurs] = j->reprise_x;
                ys[nb_joueurs] = j->reprise_y;
                modifie = 1;
            }
            nb_joueurs++;
        }
        fclose(pf);
    }

    if (!modifie) {
        if (nb_joueurs >= 8) {
            BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
            BITMAP *fond = load_bitmap("fond.bmp", NULL);
            if (!buffer || !fond) {
                if (buffer) destroy_bitmap(buffer);
                if (fond) destroy_bitmap(fond);
                return 0;
            }

            int ligne_hauteur = 50;
            int start_y = 150;
            int case_x = SCREEN_W - 100;
            int case_l = 30;
            int decalage_y = 20;

            while (1) {
                poll_keyboard();
                if (key[KEY_TAB]) {
                    show_mouse(NULL);
                    destroy_bitmap(buffer);
                    destroy_bitmap(fond);
                    clear_keybuf();
                    game_over = false;
                    return 0;
                }

                draw_sprite(buffer, fond, 0, 0);
                textout_ex(buffer, font, "Limite de 8 sauvegardes atteinte", 50, 50, makecol(255,0,0), -1);
                textout_ex(buffer, font, "Clique sur une case pour SUPPRIMER un joueur", 50, 80, makecol(255,255,255), -1);
                textout_ex(buffer, font, "Appuie sur TAB pour quitter", 50, 110, makecol(200,200,200), -1);

                for (int i = 0; i < nb_joueurs; i++) {
                    int ligne_y = start_y + i * ligne_hauteur;
                    int texte_y = ligne_y + decalage_y;
                    int case_y = ligne_y + (ligne_hauteur - case_l) / 2;

                    if (strlen(lignes[i]) > 0) {
                        textout_ex(buffer, font, lignes[i], 50, texte_y, makecol(255,255,255), -1);
                        char niveau_str[20];
                        sprintf(niveau_str, "Niv %d", niveaux[i]);
                        textout_ex(buffer, font, niveau_str, 300, texte_y, makecol(255,255,255), -1);
                        rect(buffer, case_x, case_y, case_x + case_l, case_y + case_l, makecol(255,255,255));
                    }
                }

                show_mouse(buffer);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

                if (mouse_b & 1) {
                    for (int i = 0; i < nb_joueurs; i++) {
                        int ligne_y = start_y + i * ligne_hauteur;
                        int case_y = ligne_y + (ligne_hauteur - case_l) / 2;

                        if (mouse_x >= case_x && mouse_x <= case_x + case_l &&
                            mouse_y >= case_y && mouse_y <= case_y + case_l &&
                            strlen(lignes[i]) > 0) {

                            lignes[i][0] = '\0';
                            niveaux[i] = -1;

                            FILE *pfw = fopen("joueur.txt", "w");
                            if (!pfw) {
                                destroy_bitmap(buffer);
                                destroy_bitmap(fond);
                                return 0;
                            }
                            for (int j = 0; j < nb_joueurs; j++) {
                                if (strlen(lignes[j]) > 0 && niveaux[j] != -1) {
                                    fprintf(pfw, "%s %d %d %d\n", lignes[j], niveaux[j], xs[j], ys[j]);
                                }
                            }
                            fclose(pfw);

                            destroy_bitmap(buffer);
                            destroy_bitmap(fond);
                            clear_keybuf();
                            game_over = false;
                            return 2;
                        }
                    }
                }

                rest(10);
            }
        }

        strcpy(lignes[nb_joueurs], j->nom);
        niveaux[nb_joueurs] = j->niveau;
        xs[nb_joueurs] = j->reprise_x;
        ys[nb_joueurs] = j->reprise_y;
        nb_joueurs++;
    }

    pf = fopen("joueur.txt", "w");
    if (!pf) return 0;

    for (int i = 0; i < nb_joueurs; i++) {
        fprintf(pf, "%s %d %d %d\n", lignes[i], niveaux[i], xs[i], ys[i]);
    }
    fclose(pf);
    return 1;
}







Joueur *saisir_joueur(BITMAP *screen) {
    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!buffer) {
        allegro_message("erreur");
        exit(1);
    }
    Joueur *j = malloc(sizeof(Joueur));
    if (j == NULL) {
        allegro_message("erreur");
        exit(1);
    }
    int i = 0;
    int touche = 1;
    j->niveau = 1;
    j->nom[0] = '\0';
    j->reprise_x = 75;
    j->reprise_y = 300;

    while (!key[KEY_ENTER]) {
        if(key[KEY_SPACE] ) {
            destroy_bitmap(buffer);
            free(j);
            return NULL;
        }
        if (keypressed() && touche) {
            int k = readkey();
            char c = k & 0xFF;
            k = k >> 8;
            if (k == KEY_BACKSPACE && i > 0) {
                i--;
                j->nom[i] = '\0';
            } else if (i < 49 && ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))) {
                j->nom[i++] = c;
                j->nom[i] = '\0';
            }
            touche = 0;
        }
        if (!keypressed()) touche = 1;
        rectfill(buffer, 0, 0, SCREEN_W, SCREEN_H, makecol(0, 0, 0));
        textout_centre_ex(buffer, font, "Entrez votre pseudo :", SCREEN_W / 2, 200, makecol(255, 255, 255), -1);
        rectfill(buffer, SCREEN_W / 2 - 100, 240, SCREEN_W / 2 + 100, 270, makecol(255, 255, 255));
        textout_centre_ex(buffer, font, j->nom, SCREEN_W / 2, 250, makecol(0, 0, 0), -1);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        rest(10);
    }
    destroy_bitmap(buffer);
    return j;
}
int recherche_dans_fichier(Joueur *j) {
    FILE *pf = fopen("joueur.txt", "r");
    if (pf == NULL) {
        allegro_message("Erreur ouverture joueur.txt");
        exit(1);
    }

    char nom_fichier[50];
    int niveau, x, y;

    while (fscanf(pf, "%s %d %d %d", nom_fichier, &niveau, &x, &y) == 4) {
        if (strcmp(j->nom, nom_fichier) == 0) {
            fclose(pf);
            return 1;
        }
    }

    fclose(pf);
    return 0;
}
Joueur* nouveau_joueur(BITMAP *screen) {

    FILE *pf = fopen("joueur.txt", "r");
    if (!pf) {
        allegro_message("Erreur ouverture joueur.txt");
        return NULL;
    }

    int count = 0;
    char ligne[100];
    while (fgets(ligne, sizeof(ligne), pf)) {
        if (ligne[0] != '\0' && ligne[0] != '\n') {
            count++;
        }
    }
    fclose(pf);

    if (count >= 8) {

        Joueur *temp = malloc(sizeof(Joueur));
        if (!temp) return NULL;
        strcpy(temp->nom, "DEMO");
        temp->niveau = 1;
        temp->reprise_x = 0;
        temp->reprise_y = 0;

        int res = sauvegarder_joueur(temp);
        free(temp);

        if (res == 0) return NULL;
    }
    while (1) {
        clear_keybuf();
        Joueur *j = saisir_joueur(screen);
        if (j == NULL) {
            return NULL;
        }

        if (recherche_dans_fichier(j)) {
            clear_to_color(screen, makecol(0, 0, 0));
            rectfill(screen, SCREEN_W / 2 - 120, 240, SCREEN_W / 2 + 120, 270, makecol(255, 255, 255));
            char msg[100];
            sprintf(msg, "Le pseudo %s existe déjà", j->nom);
            textout_centre_ex(screen, font, msg, SCREEN_W / 2, 250, makecol(0, 0, 0), -1);
            rest(1500);
            free(j);
            continue;
        }

        int statut = sauvegarder_joueur(j);
        if (statut == 0) {
            free(j);
            return NULL;
        }

        clear_to_color(screen, makecol(0, 0, 0));
        char message2[100];
        sprintf(message2, "Bienvenue %s !!", j->nom);
        rectfill(screen, SCREEN_W / 2 - 100, 240, SCREEN_W / 2 + 100, 270, makecol(255, 255, 255));
        textout_centre_ex(screen, font, message2, SCREEN_W / 2, 250, makecol(0, 0, 0), -1);
        rest(1500);

        return j;
    }
}



int ecran_victoireniv1() {
    BITMAP *image = load_bitmap("ecran_victoire.bmp", NULL);
    if (!image) {
        allegro_message("Erreur chargement ecran_victoire.bmp");
        exit(1);
    }

    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!buffer) {
        destroy_bitmap(image);
        allegro_message("Erreur création du double buffer");
        exit(1);
    }

    show_mouse(screen);

    while (!key[KEY_ESC]) {
        blit(image, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        if (mouse_b & 1) {
            if ((mouse_x >= 125) && (mouse_x <= 678) && (mouse_y >= 225) && (mouse_y <= 320)) {
                destroy_bitmap(image);
                destroy_bitmap(buffer);
                rest(300);
                return 1;
            } else if ((mouse_x >= 125) && (mouse_x <= 678) && (mouse_y >= 340) && (mouse_y <= 430)) {
                destroy_bitmap(image);
                destroy_bitmap(buffer);
                rest(300);
                return 2;
            }
        }
        rest(10);
    }

    destroy_bitmap(image);
    destroy_bitmap(buffer);
    return 2;
}
int ecran_defaiteniv1() {
    BITMAP *image = load_bitmap("ecran_defaite.bmp", NULL);
    if (!image) {
        allegro_message("Erreur chargement ecran_defaite.bmp");
        exit(1);
    }

    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!buffer) {
        destroy_bitmap(image);
        allegro_message("Erreur création du double buffer");
        exit(1);
    }

    show_mouse(screen);

    while (!key[KEY_ESC]) {
        blit(image, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        if (mouse_b & 1) {
            if ((mouse_x >= 150) && (mouse_x <= 650) && (mouse_y >= 270) && (mouse_y <= 340)) {
                destroy_bitmap(image);
                destroy_bitmap(buffer);
                rest(300);
                return 1;
            } else if ((mouse_x >= 150) && (mouse_x <= 650) && (mouse_y >= 370) && (mouse_y <= 440)) {
                destroy_bitmap(image);
                destroy_bitmap(buffer);
                rest(300);
                return 2;
            }
        }
        rest(10);
    }

    destroy_bitmap(image);
    destroy_bitmap(buffer);
    return 2;
}


