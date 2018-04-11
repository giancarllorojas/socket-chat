/*
* Peer Passive
*/
#include "mysocket.h"  
#include <pthread.h>

#define BUFSIZE 100
#define NTHREADS 100

/* Global Vars */
int tid = 0;
pthread_t threads[NTHREADS];
TSocket srvSock, cliSock;  
int i;

/* Structure of arguments to pass to client thread */
struct TArgs {
  TSocket cliSock;
};

/* Handle client request */
void * HandleChatWithActivePeer(void *args) {
  printf("Thread iniciada!\n");
  char typedMessage[BUFSIZE], recievedMessage[BUFSIZE];
  TSocket cliSock;
  fd_set setChat;
  int retChat;
  int n;

  printf("Extraindo sock!\n");
  cliSock = ((struct TArgs *) args) -> cliSock;
  free(args); 

  printf("Inicializando o select!\n");
  FD_ZERO(&setChat);
  FD_SET(STDIN_FILENO, &setChat);
  FD_SET(cliSock, &setChat);

  for(;;) {
    printf("Esperando escrever ou receber mensagem!\n");
    retChat = select (FD_SETSIZE, &setChat, NULL, NULL, NULL);
    printf("Saiu do select\n");

    if (retChat < 0) {
      WriteError("select() failed"); 
      break;
    }

    // a.1) aguardar mensagem do par e exibir mensagem
    if(FD_ISSET(cliSock, &setChat)){
      printf("Mensagem recebida do Par\n");
      if (ReadLine(cliSock, recievedMessage, BUFSIZE-1) < 0) 
      {
        ExitWithError("ReadLine() failed"); 
      } else {
        // Print the recieved message
        printf("Imprimindo mensagem\n");
        printf("Par: %s\n", recievedMessage);
      }
    }

    // a.2) esperar entrada do usuario local
    if (FD_ISSET(STDIN_FILENO, &setChat)) {
      printf("Recebeu do STDIN na thread\n");
      scanf("%99[^\n]%*c", typedMessage);
      // a.3) receber comando para finalizar a conversa/conexao
      if(strncmp(typedMessage, "FIM", 3) == 0){
        // b.1) envia mensagem com a tag "FIM" para o par
        WriteN(cliSock, "FIM", 3);
        close(cliSock);
        pthread_exit(NULL);
      }else{
        //a.2) enviar mensagem para o par
        n = strlen(typedMessage);
        typedMessage[n] = '\n';
        WriteN(cliSock, typedMessage, strlen(typedMessage));
      }
    }
  }

  close(cliSock);
  pthread_exit(NULL);
}

void ConnectWithActivePeer(){
  struct TArgs *args;
  if (tid == NTHREADS) { 
    WriteError("Reached max number of clients"); 
    exit(0);
  }

  printf("Criando nova conexao\n");
  cliSock = AcceptConnection(srvSock);
  printf("Conexao criada!\n");

  printf("Alocando memória para thread do client!\n");
  /* Create separate memory for client argument */
  if ((args = (struct TArgs *) malloc(sizeof(struct TArgs))) == NULL) { 
    WriteError("malloc() failed");
    exit(0);
  }
  args->cliSock = cliSock;

  printf("Criando thread(TID: %d) para chat com o client!\n", tid);
  /* Create a new thread to handle a chat with a new client */
  if (pthread_create(&threads[tid++], NULL, HandleChatWithActivePeer, (void *) args)) { 
    WriteError("pthread_create() failed"); 
    exit(0);
  }
}

/*
* Function to handle new connections and the quit command
*/
void MainLoop(){
  char strQuit[BUFSIZE];
  fd_set set;
  int retMainLoop;

  FD_ZERO(&set);
  FD_SET(STDIN_FILENO, &set);
  FD_SET(srvSock, &set);

  // Main loop
  for (;;) { 
    printf("Esperando acao no MainLoop\n");
    retMainLoop = select (FD_SETSIZE, &set, NULL, NULL, NULL);
    if (retMainLoop < 0) {
      WriteError("select() failed"); 
      break;
    }

    printf("Acao recebida\n");

    // 4.2) receber comando do usuario para finalizar a aplicac¸˜ao
    if (FD_ISSET(STDIN_FILENO, &set)) {
      printf("Recebeu do STDIN no MainLoop\n");
      scanf("%99[^\n]%*c", strQuit);
      if (strncmp(strQuit, "FIM", 3) == 0){
        // TODO: c.1) finaliza usu´ario no servidor de usuarios (cod 3):
        // c.2) finaliza aplicacao
        exit(0);
      };
    }

    // 4.1) aguardar conexao de "par ativo"
    if (FD_ISSET(srvSock, &set)) {
      ConnectWithActivePeer();
    }
  }

}

int main(int argc, char *argv[]) {
  // 1- Cria um no servidor: <maaquina local> <porta>
  if (argc == 1) { ExitWithError("Usage: server <local port>"); }
  srvSock = CreateServer(atoi(argv[1]));

  // TODO: 2- Conecta-se com o servidor de usuarios: <IP do servidor> <porta 2018>
  // TODO: 3- Registra seu usu´ario (cod 1): <1> <nome do usu´ario> <porta> <\n>
  // TODO: 3.1) recebe confirmacao da solicitac¸˜ao: (0, 1 ou 2)

  MainLoop();
  
  /* Wait for all threads to terminate */
  for(i=0; i<tid; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}