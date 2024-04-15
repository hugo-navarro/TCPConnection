#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <pthread.h>

#include "clientThread.h"

#define PORT 8888
#define ADM_PORT 8889
#define MAX_CLIENTS 9
#define TIMEOUT_SECONDS 30

int main() {
    SOCKET adm_server_descriptor, server_descriptor, adm_client_fd, client_fd;
    //Essa variavel de tipo sockaddr_in é um struct igual ao visto em aula, então so inicializei direto da biblioteca
    struct sockaddr_in adm_server_addr, server_addr, adm_client_addr, client_addr;
    pthread_t threads[MAX_CLIENTS];
    pthread_t adm_thread[MAX_CLIENTS];

    // referencia https://learn.microsoft.com/en-us/previous-versions/windows/embedded/ms911778(v=msdn.10)?redirectedfrom=MSDN
    WORD wVersionRequested;
    WSADATA wsaData;
    WORD wVersionRequestedadm;
    WSADATA wsaDataadm;
    int err;
    
    wVersionRequested = MAKEWORD( 2, 2 );
    
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        return -1;
    }

    // socket(AF_INET = IPv4, SOCK_STREAM = TCP, IPPROTO_TCP = TCP protocol)
    // referencia https://learn.microsoft.com/pt-br/windows/win32/api/winsock2/nf-winsock2-socket
    if ((adm_server_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    // Novamente, os valores usados são todos pré descritos na biblioteca então estou usando a estrutura da biblioteca para fazer todos os passos do servidor
    // A citacao abaixo é para explicar o que é o INADDR_ANY, que é um valor que pode ser usado para o campo s_addr caso o IP do servidor nao faca diferenca
    // If you don't know your IP address, or you know you only have one IP address on the machine, or you don't care which of the machine's IP addresses is used, you can simply set the s_addr field in your struct sockaddr_in to INADDR_ANY and it will fill in the IP address for you.
    // referencia https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming
    memset(&adm_server_addr, 0, sizeof(adm_server_addr));
    adm_server_addr.sin_family = AF_INET;
    adm_server_addr.sin_addr.s_addr = INADDR_ANY;
    adm_server_addr.sin_port = htons(ADM_PORT);

    // Bind socket com o IP e porta
    if (bind(adm_server_descriptor, (struct sockaddr *)&adm_server_addr, sizeof(adm_server_addr)) == SOCKET_ERROR) {
        printf("Binding failed\n");
        exit(EXIT_FAILURE);
    }

    // Listen precisa de um parametro de numero maximos de conexoes
    if (listen(adm_server_descriptor, 1) == SOCKET_ERROR) {
        printf("Listen failed\n");
        exit(EXIT_FAILURE);
    }

    wVersionRequestedadm = MAKEWORD( 2, 2 );

    err = WSAStartup( wVersionRequestedadm, &wsaDataadm );
    if ( err != 0 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        return -1;
    }

    if ((server_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    //Repetindo o processo para usuarios capazes somente de ler
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_descriptor, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Binding failed\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_descriptor, MAX_CLIENTS) == SOCKET_ERROR) {
        printf("Listen failed\n");
        exit(EXIT_FAILURE);
    }

    printf("ADM Server running on port %d\n", ADM_PORT);
    printf("Server running on port %d\n", PORT);

    while (1) {
        int max_descriptor;
        fd_set readfds;
        struct timeval timeout;
        int activity;

        FD_ZERO(&readfds);

        FD_SET(adm_server_descriptor, &readfds);
        FD_SET(server_descriptor, &readfds);

        max_descriptor = (adm_server_descriptor > server_descriptor) ? adm_server_descriptor : server_descriptor;

        timeout.tv_sec = TIMEOUT_SECONDS;
        timeout.tv_usec = 0;

        activity = select(max_descriptor + 1, &readfds, NULL, NULL, &timeout);

        //Olha para a porta ADM (criacao e delecao de musicas)
        if (FD_ISSET(adm_server_descriptor, &readfds)) {
            int adm_client_len = sizeof(adm_client_addr);
            if ((adm_client_fd = accept(adm_server_descriptor, (struct sockaddr *)&adm_client_addr, &adm_client_len)) == INVALID_SOCKET) {
                printf("Accept failed\n");
                continue;
            }

            pthread_create(&adm_thread[0], NULL, handle_adm_client, (void *)&adm_client_fd);

            printf("Client (IP: %s) connected to adm server!\n", inet_ntoa(adm_client_addr.sin_addr));
        }

        // Olha para a porta de leitura (busca de musicas)
        if (FD_ISSET(server_descriptor, &readfds)) {
            int client_len = sizeof(client_addr);
            if ((client_fd = accept(server_descriptor, (struct sockaddr *)&client_addr, &client_len)) == INVALID_SOCKET) {
                printf("Accept failed\n");
                continue;
            }

            pthread_create(&threads[MAX_CLIENTS], NULL, handle_client, (void *)&client_fd);

            printf("Client (IP: %s) connected to client server!\n", inet_ntoa(client_addr.sin_addr));
        }
    }

    // Wait for all threads to finish
    WaitForMultipleObjects(MAX_CLIENTS, threads, TRUE, INFINITE);

    WaitForMultipleObjects(1, adm_thread, TRUE, INFINITE);

    closesocket(adm_server_descriptor);
    closesocket(server_descriptor);
    WSACleanup();
    WSACleanup();

    return 0;
}