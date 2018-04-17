/*
* Peer Active
*/
#include "mysocket.h"
#define BUFSIZE 100

TSocket initiateChat(char *servIP, unsigned short servPort){
  TSocket sock;
  char initialMessage[100];
  int n;

  // 1- Conecta-se com um "par passivo": <IP do par passivo> <porta do par passivo>
  sock = ConnectToServer(servIP, servPort);
  
  printf("Envie sua mensagem de saudação\n");

  // 2- Recebe mensagem de saudacao do usuario (teclado) e a envia
  scanf("%99[^\n]%*c",initialMessage);
  n = strlen(initialMessage);
  initialMessage[n] = '\n';

  if (WriteN(sock, initialMessage, ++n) <= 0)
  { 
    ExitWithError("WriteN() failed"); 
  }

  return sock;
}

void handleChat(TSocket sock){
  char typedMessage[BUFSIZE], recievedMessage[BUFSIZE];
  fd_set setChat;
  int retChat;
  int n;
  
  for(;;) {
    FD_ZERO(&setChat);
    FD_SET(STDIN_FILENO, &setChat);
    FD_SET(sock, &setChat);
    // 3- Usar SELECT para:
    retChat = select (FD_SETSIZE, &setChat, NULL, NULL, NULL);

    if (retChat < 0) {
      WriteError("select() failed"); 
      break;
    }

    // 3.2) aguardar mensagens do par
    if(FD_ISSET(sock, &setChat)){
      if (ReadLine(sock, recievedMessage, BUFSIZE-1) < 0) 
      { 
        ExitWithError("ReadLine() failed"); 
      } else {
        // Print the recieved message
        printf("Par: %s\n", recievedMessage);
      }
    }

    // 3.1) continuar escrevendo
    if (FD_ISSET(STDIN_FILENO, &setChat)) {
      scanf("%99[^\n]%*c", typedMessage);

      // 3.3) finalizar
      if(strncmp(typedMessage, "FIM", 3) == 0){
        // c.1) envia mensagem com a tag "FIM"
        WriteN(sock, "FIM", 3);

        //c.2) finaliza aplicac¸˜ao
        close(sock);
        exit(0);
      }else{
        n = strlen(typedMessage);
        typedMessage[n] = '\n';
        // a.1) esperar entrada do usuario local, enviar mensagem para o par
        WriteN(sock, typedMessage, strlen(typedMessage));
      }
    }
  }
}

int main(int argc, char *argv[]) {
  TSocket sock;

  if (argc != 3) {
    ExitWithError("Usage: client <peer passive IP> <peer passive Port>");    
  }

  sock = initiateChat(argv[1], atoi(argv[2]));

  handleChat(sock);

  close(sock);
  return 0;
}
