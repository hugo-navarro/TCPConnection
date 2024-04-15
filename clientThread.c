#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>

#include "database.h"
#include "clientThread.h"

#define MAX_MUSICS 100
#define CSVFILEPATH "./musicas.csv"
#define TXTFILEPATH "./musicas.txt"

void *handle_client(void *arg) {
    printf("Handling client.\n");
    int client_fd = *((int *)arg);
    char buffer[1024];
    ssize_t bytes_received;
    char *token;
    const char delimiter[2] = ";";
    char *saveptr;
    const char *close_connection = "CLOSE";

    load_from_csv();
    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
        //buffer[bytes_received - 1] = '\0';
        printf("Received message from client: %s\n", buffer);

        if (strcmp(buffer, close_connection) == 0) {
            char response[1024];
            sprintf(response, "Conexao sendo encerrada!");   
            send(client_fd, response, strlen(response), 0);
            break;
        }
        // Tokenize the received message
        token = strtok_r(buffer, delimiter, &saveptr);

        // Vou checar se o primeiro token é um dos comandos que o servidor aceita
        if (strcmp(token, "LIST") == 0) {
            char *type = strtok_r(NULL, delimiter, &saveptr);
            char *value = strtok_r(NULL, delimiter, &saveptr);

            Music *musics = list_music(type, value);
            
            if (musics[0].id == -1) {
                send(client_fd, "Nenhuma musica encontrada com esse filtro!", strlen("Nenhuma musica encontrada com esse filtro!"), 0);
            }
            else{
                char response[1024];
                for (int i = 0; i < sizeof(MAX_MUSICS); i++){
                    if (musics[i].id == -1) {
                        break;
                    }
                    printf("%s", musics[i].titulo);
                    sprintf(response, "Musica: %s,\nAno: %s,\nInterprete: %s,\nIdioma: %s\n,Tipo de musica: %s\n,Refrao: %s\n\n ", &musics[i].titulo, &musics[i].ano, &musics[i].interprete, &musics[i].idioma, &musics[i].tipo_de_musica, &musics[i].refrao);   
                }
                send(client_fd, response, strlen(response), 0);
            }
        } else if (strcmp(token, "GET") == 0) {
            int id = atoi(strtok_r(NULL, delimiter, &saveptr));
            
            Music* music = get_music(id);

            if (music->id == -1) {
                send(client_fd, "Nenhuma musica encontrada!", strlen("Nenhuma musica encontrada!"), 0);
            }
            else{
                char response[1024];
                sprintf(response,  "Musica: %s,\nAno: %s,\nInterprete: %s,\nIdioma: %s,\nTipo de musica: %s,\nRefrao: %s\n\n ", music->titulo, music->ano, music->interprete, music->idioma, music->tipo_de_musica, music->refrao);   
                send(client_fd, response, strlen(response), 0);
            }
        } else {
            send(client_fd, "Operacao invalida!", strlen("Operacao invalida!"), 0);
        }
    }

    if (bytes_received == 0) {
        printf("Client disconnected.\n");
    }

    closesocket(client_fd);
    printf("Client socket closed.\n");
    
    _endthread();
}

void *handle_adm_client(void *arg) {
    printf("Handling adm client.\n");
    int client_fd = *((int *)arg);
    char buffer[1024];
    ssize_t bytes_received;
    char *token;
    const char delimiter[2] = ";";
    char *saveptr;
    const char *close_connection = "CLOSE";

    load_from_csv();
    // Loop para receber mensagens do cliente e criar tokens para cada string contida entre ;;
    while ((bytes_received = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {

        //buffer[bytes_received] = '\0';
        printf("Received message from client: %s\n", buffer);

        if (strcmp(buffer, close_connection) == 0) {
            char response[1024];
            sprintf(response, "Conexao sendo encerrada!");   
            send(client_fd, response, strlen(response), 0);
            break;
        }
        token = strtok_r(buffer, delimiter, &saveptr);

        // Vou checar se o primeiro token é um dos comandos que o servidor aceita
        if (strcmp(token, "POST") == 0) {
            char *title = strtok_r(NULL, delimiter, &saveptr);
            char *interprete = strtok_r(NULL, delimiter, &saveptr);
            char *idioma = strtok_r(NULL, delimiter, &saveptr);
            char *tipodemusica = strtok_r(NULL, delimiter, &saveptr);
            char *refrao = strtok_r(NULL, delimiter, &saveptr);
            char *ano = strtok_r(NULL, delimiter, &saveptr);

            int new_id = post_music(title, interprete, idioma, tipodemusica, refrao, ano);

            if (new_id == -1) {
                send(client_fd, "Falha ao adicionar musica!", strlen("Falha ao adicionar musica!"), 0);
            } else {
                char response[1024];
                sprintf(response, "Musica %s (Id: %d) adicionada.", title, new_id);   
                send(client_fd, response, strlen(response), 0);
            }

        } else if (strcmp(token, "DELETE") == 0) {
            int id = atoi(strtok_r(NULL, delimiter, &saveptr));

            bool deleted_music = delete_music(id);

            if (!deleted_music) {
                send(client_fd, "Nao e possivel deletar a musica!", strlen("Nao e possivel deletar a musica!"), 0);
            } else {
                char response[1024];
                sprintf(response, "Musica Id %d deletada.", id);   
                send(client_fd, response, strlen(response), 0);
            }
        } else {
            send(client_fd, "Operacao invalida!", strlen("Operacao invalida!"), 0);
        }
        write_csv_file();
    }
    write_pretty_print_file();

    if (bytes_received == 0) {
        printf("Client disconnected.\n");
    }

    // Close client socket
    closesocket(client_fd);
    printf("Client socket closed.\n");
    
    _endthread();
}