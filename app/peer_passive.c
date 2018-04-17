/*
* Peer Passive
*/
#include "mysocket.h"  
#include <pthread.h>

#define BUFSIZE 100
#define NTHREADS 100

#define SERVER_IP "192.168.1.117"
#define SERVER_PORT 2018

/* Global Vars */
int tid = 0;
pthread_t threads[NTHREADS];
TSocket mainSrvSock, srvSock, cliSock;  
int i;
char userName[20];

/* Structure of arguments to pass to client thread */
struct TArgs {
  TSocket cliSock;
};


void unregisterInServer(){
  char exitMessage[20];
  sprintf(exitMessage, "3 %s \n", userName);
  WriteN(mainSrvSock, exitMessage, strlen(exitMessage));
}

/* Handle client request */
void HandleChatWithActivePeer(TSocket cliSock) {
  printf("Iniciando conversa!\n");
  char typedMessage[BUFSIZE], recievedMessage[BUFSIZE];
  fd_set setChat;
  int retChat;
  int n;

  for(;;) {
    printf("Inicializando o select!\n");
    FD_ZERO(&setChat);
    FD_SET(STDIN_FILENO, &setChat);
    FD_SET(cliSock, &setChat);

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
        unregisterInServer();
        exit(0);
      }else{
        //a.2) enviar mensagem para o par
        printf("Escrevendo mensagem para o par: %s\n", typedMessage);
        n = strlen(typedMessage);
        typedMessage[n] = '\n';
        WriteN(cliSock, typedMessage, strlen(typedMessage));
      }
    }
  }

  close(cliSock);
}

void ConnectWithActivePeer(){
  printf("Criando nova conexao\n");
  cliSock = AcceptConnection(srvSock);
  printf("Conexao criada!\n");

  /* Handle chat with active peer */
  HandleChatWithActivePeer(cliSock);
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

    // 4.2) receber comando do usuario para finalizar a aplicacao
    if (FD_ISSET(STDIN_FILENO, &set)) {
      printf("Recebeu do STDIN no MainLoop\n");
      scanf("%99[^\n]%*c", strQuit);
      if (strncmp(strQuit, "FIM", 3) == 0){
        // TODO: c.1) finaliza usu´ario no servidor de usuarios (cod 3):
        // c.2) finaliza aplicacao
        unregisterInServer();
        exit(0);
      };
    }

    // 4.1) aguardar conexao de "par ativo"
    if (FD_ISSET(srvSock, &set)) {
      printf("Recebeu do srvSock no MainLoop\n");
      ConnectWithActivePeer();
    }
  }
}

void registerInServer(int localServerPort){
  char registerMessage[20];
  char registerResponse[10];
  int registerResponseCode;

  printf("Conectando ao servidor principal\n");
  // 2- Conecta-se com o servidor de usuarios: <IP do servidor> <porta 2018>
  mainSrvSock = ConnectToServer(SERVER_IP, SERVER_PORT);

  // 3- Registra seu usu´ario (cod 1): <1> <nome do usuario> <porta> <\n>
  printf("Inicializando par passivo.\n Digite seu nome de usuário para se registrar no servidor:\n");
  scanf("%99[^\n]%*c", userName);
  
  sprintf(registerMessage, "1 %s %d\n", userName, localServerPort);
  printf("Registrando par passivo no servidor: %s\n", registerMessage);

  WriteN(mainSrvSock, registerMessage, strlen(registerMessage) + 1);

  // 3.1) recebe confirmacao da solicitacao: (0, 1 ou 2)
  if (ReadLine(mainSrvSock, registerResponse, strlen(registerResponse)) < 0) 
  {
    ExitWithError("ReadLine() failed"); 
    exit(1);
  }else{
    printf("Recebida resposta do servidor principal: %s\n", registerResponse);
    registerResponseCode = atoi(registerResponse);
    if(registerResponseCode == 0){
      printf("Limite de usuários\n");
      exit(1);
    }else if(registerResponseCode == 1){
      printf("Login Existente\n");
      exit(1);
    }else if(registerResponseCode == 2){
      printf("Registrado com sucesso!!!\n");
    }
  }
}

int main(int argc, char *argv[]) {
  int localServerPort;
  
  // 1- Cria um no servidor: <maaquina local> <porta>
  if (argc == 1) { ExitWithError("Usage: server <local port>"); }

  localServerPort = atoi(argv[1]);
  srvSock = CreateServer(localServerPort);

  registerInServer(localServerPort);

  MainLoop();
  
  /* Wait for all threads to terminate */
  for(i=0; i<tid; i++) {
    pthread_join(threads[i], NULL);
  }

  return 0;
}