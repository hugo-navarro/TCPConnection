#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT_TO_CONNECT 8888
#define SERVER_ADDR "127.0.0.1"

int main() {
    SOCKET client_fd;
    struct sockaddr_in server_addr;
    char message[1024];

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    const char *close_connection = "CLOSE";
    
    wVersionRequested = MAKEWORD( 2, 2 );
    
    err = WSAStartup( wVersionRequested, &wsaData );
    // O inicio do programa é igual ao do servidor, com a diferença que o cliente nao precisa de um bind
    if ( err != 0 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        return -1;
    }

    // Create socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    // Prepare server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(PORT_TO_CONNECT);

    // Conecta ao servidor com o socket criado
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection failed.\n");
        closesocket(client_fd);
        WSACleanup();
        return 1;
    }

    printf("Connected to server.\n");

    // Teste de mensagem qualquer pelo usuario
    printf("Available commands: GET(id); LIST(type, value).\n");
    printf("GET message: GET;<id>\n");
    //type seria a coluna pela qual deseja se filtrar, value seria o valor a ser procurado na coluna
    printf("LIST message: LIST;<type>?;<value>?\n");
    printf("---<type>: id, titulo, interprete, idioma, tipodemusica, ano, NULL\n");
    printf("Close connection: CLOSE\n");
    while(1){
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);
        message[strlen(message) - 1] = '\0';
        //printf("Message: %s\n", message);
        send(client_fd, message, strlen(message), 0);
        char saved_message[1024];
        strcpy(saved_message, message);
        // Junto ao servidor, executamos uma rotina de callback para 
        memset(message, 0, sizeof(message));
        recv(client_fd, message, sizeof(message), 0);
        printf("Server response: %s\n", message);
        
        if (strcmp(saved_message, close_connection) == 0) {
            printf("Client requested to close the connection. Closing...\n");
            break;
        }
    }

    // Close socket
    closesocket(client_fd);

    WSACleanup();
    return 0;
}