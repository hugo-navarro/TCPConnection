#ifndef DATABASE_H
#define DATABASE_H

#include <stdbool.h>

typedef struct {
    int id;
    char titulo[100];
    char interprete[100];
    char idioma[20];
    char tipo_de_musica[100];  
    char refrao[200];
    char ano[3];
} Music;

int post_music(const char *titulo, const char *interprete, const char *idioma, const char *tipo_de_musica, const char *refrao, const char *ano);
Music *list_music(const char *paramType, const char *param);
Music *get_music(int index);
bool delete_music(int index);


#endif