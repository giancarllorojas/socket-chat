#include "mysocket.h"

#define SERVER_IP "192.168.1.117"
#define SERVER_PORT 2018

//4- Exibe lista de usu´arios ativos
void recieveUserList(TSocket sock){
    char userCount[20];
    int  intUserCount, i;

    // Recebe numero de usuarios
    ReadLine(sock, userCount, 20);
    intUserCount = atoi(userCount);
    
    printf("Total de usuários: %d\nLista de usuários:\n", intUserCount);
    for(i = 0; i < intUserCount; i++){
        char userString[20];
        ReadLine(sock, userString, 20);
        printf("%s", userString);
    }
}

void mainLoop(TSocket sock){
    char typedMessage[1];
    // 5- Aguarda comando do administrador:
    for(;;) {
        printf("Digite 1 para receber a lista de usuários e 0 para sair\n");
        scanf("%99[^\n]%*c", typedMessage);

        // 5.1) atualiza lista de usu´arios (volta ao passo 2)
        if(strncmp(typedMessage, "1", 1) == 0){
            // 3- Recebe lista de usu´arios ativos //tamanho maximo de 20
            recieveUserList(sock);
        }
        // 5.2) finaliza aplicac¸˜ao
        else if(strncmp(typedMessage, "0", 0) == 0){
            printf("Saindo\n");
            exit(0);
        }
    }
}

int main(int argc, char *argv[]) {
    TSocket sock;
    char initialMessage[2] = {"2\n"};

    // 1- Conecta-se com o servidor de usuarios: <IP do servidor> <porta 2018>
    sock = ConnectToServer(SERVER_IP, SERVER_PORT);

    // 2- Solicita lista de usu´arios ativos (cod 2)
    WriteN(sock, initialMessage, 2);

    mainLoop(sock);

    exit(0);
}