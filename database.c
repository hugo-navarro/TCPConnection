#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "database.h"
#define TXTFILEPATH "./pretty_print_musics.txt"
#define CSVFILEPATH "./musics.csv"
#define MAX_MUSICS 100
int next_movie_id = 0;

Music musics[MAX_MUSICS];
int num_musics = 0;

void load_from_csv() {
    FILE *file = fopen(CSVFILEPATH, "r");
    if (file == NULL) {
        FILE *file = fopen(CSVFILEPATH, "w+");
        fprintf(file, "ID;Titulo;Interprete;Idioma;Tipo de musica;Refrao;Ano\n");

        fclose(file);
        
        return;
    }

    int music_counter = 0;
    char str[128];
    char *sp;
    int result;
    while(fgets(str, 100, file )!= NULL)
    {
        sp = strtok(str, ";");
        if (sp[0] == 'I') {
            continue;
        }
        musics[music_counter].id = atof(sp);
 
        sp = strtok(NULL, ";");
        strcpy(musics[music_counter].titulo, sp);
 
        sp = strtok(NULL, ";");
        strcpy(musics[music_counter].interprete, sp);

        sp = strtok(NULL, ";");
        strcpy(musics[music_counter].idioma, sp);

        sp = strtok(NULL, ";");
        strcpy(musics[music_counter].tipo_de_musica, sp);

        sp = strtok(NULL, ";");
        strcpy(musics[music_counter].refrao, sp);

        sp = strtok(NULL, ";");
        strcpy(musics[music_counter].ano, sp);

        musics[music_counter].id = next_movie_id++;	
        music_counter++;
    }
    num_musics = music_counter;

    fclose(file);
}

int post_music(const char *titulo, const char *interprete, const char *idioma, const char *tipo_de_musica, const char *refrao, const char *ano) {
    if (num_musics > MAX_MUSICS) {
        return -1;
    }
    
    musics[num_musics].id = next_movie_id++;
    strcpy(musics[num_musics].titulo, titulo);
    strcpy(musics[num_musics].interprete, interprete);
    strcpy(musics[num_musics].idioma, idioma);
    strcpy(musics[num_musics].tipo_de_musica, tipo_de_musica);
    strcpy(musics[num_musics].refrao, refrao);
    strcpy(musics[num_musics].ano, ano);
    num_musics++;

    return musics[num_musics - 1].id;
}

Music *list_music(const char *paramType, const char *param) {
    Music *musics_list = malloc( MAX_MUSICS * sizeof(Music) );
    int num_musics_list = 0;
    //Based on paramType, select a single field to search for the parameter
    if (strcmp(paramType, "titulo") == 0) {
        for (int i = 0; i < num_musics; i++) {
            if (strstr(musics[i].titulo, param) != NULL) {
                musics_list[num_musics_list++] = musics[i];
            }
        }
    } else if (strcmp(paramType, "interprete") == 0) {
        for (int i = 0; i < num_musics; i++) {
            if (strstr(musics[i].interprete, param) != NULL) {
                musics_list[num_musics_list++] = musics[i];
            }
        }
    } else if (strcmp(paramType, "idioma") == 0) {
        for (int i = 0; i < num_musics; i++) {
            if (strstr(musics[i].idioma, param) != NULL) {
                musics_list[num_musics_list++] = musics[i];
            }
        }
    } else if (strcmp(paramType, "tipodemusica") == 0) {
        for (int i = 0; i < num_musics; i++) {
            if (strstr(musics[i].tipo_de_musica, param) != NULL) {
                musics_list[num_musics_list++] = musics[i];
            }
        }
    } else if (strcmp(paramType, "ano") == 0) {
        for (int i = 0; i < num_musics; i++) {
            if (strstr(musics[i].ano, param) != NULL){
                musics_list[num_musics_list++] = musics[i];
            }
        }
    } else if (strcmp(paramType, "refrao") == 0) {
        for (int i = 0; i < num_musics; i++) {
            if (strstr(musics[i].refrao, param) != NULL) {
                musics_list[num_musics_list++] = musics[i];
            }
        }
    }else if (strcmp(paramType, "NULL") == 0) {
        for (int i = 0; i < num_musics; i++) {
            musics_list[num_musics_list++] = musics[i];
        }
    } else {
        for (int i = 0; i < num_musics; i++) {
            musics_list[num_musics_list++] = musics[i];
        }
    }
    musics_list[num_musics_list].id = -1;

    return musics_list;
}

bool delete_music(int id) {
    if (id < 0 || num_musics == 0) {
        return false;
    }

    int index = -1;
    for (int i = 0; i < num_musics; i++) {
        if (musics[i].id == id) {
            index = i;
            musics[i].id = -1;
            break;
        }
    }

    if (index == -1) {
        return false;
    }

    for (int i = index; i < num_musics - 1; i++) {
        musics[i] = musics[i + 1];
    }

    num_musics--;
    return true;
}

Music *get_music(int id) {
    Music *musics_list = malloc(sizeof(Music) * 1);

    if (id < 0 || num_musics == 0) {
        Music default_music = { -1, "", "", "", "", "", ""};
        musics_list[0] = default_music;
        
        return musics_list;
    }
    
    int index = -1;
    for (int i = 0; i < num_musics; i++) {
        if (musics[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        Music default_music = { -1, "", "", "", "", "", ""};
        musics_list[0] = default_music;

        return musics_list;
    }

    musics_list[0] = musics[index];

    return musics_list;
}

void write_pretty_print_file() {
    FILE *file = fopen(TXTFILEPATH, "w+");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", TXTFILEPATH);
        return;
    }

    fprintf(file, "Musics available:\n\n");
    for (int i = 0; i < MAX_MUSICS; i++) {
        if (musics[i].id < 0 || musics[i].id > MAX_MUSICS) {
            break;
        }

        fprintf(file, "Musica ID %d\n", musics[i].id);
        fprintf(file, "Titulo: %s\n", musics[i].titulo);
        fprintf(file, "Interprete: %s\n", musics[i].interprete);
        fprintf(file, "Idioma: %s\n", musics[i].idioma);
        fprintf(file, "Tipo de musica: %s\n", musics[i].tipo_de_musica);
        fprintf(file, "Refrao: %s\n", musics[i].refrao);
        fprintf(file, "Ano: %s\n", musics[i].ano);
        fprintf(file, "\n\n");
    }

    fclose(file);
}

void write_csv_file() {
    FILE *file = fopen(CSVFILEPATH, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", CSVFILEPATH);
        return;
    }
    fprintf(file, "ID;Titulo;Interprete;Idioma;Tipo de musica;Refrao;Ano\n");
    for (int i = 0; i < num_musics; i++) {
        if (musics[i].id < 0 || musics[i].id > MAX_MUSICS) {
            break;
        }

        fprintf(file, "%d;%s;%s;%s;%s;%s;%s\n", musics[i].id, musics[i].titulo, musics[i].interprete, musics[i].idioma, musics[i].tipo_de_musica, musics[i].refrao, musics[i].ano);
    }

    fclose(file);
}