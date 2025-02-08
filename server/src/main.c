#include "gameFunc.h"

int main(int argc, char **argv) {
  /************ INITIALISATION OF THE SERVER ************/
  int servSock = 0;          // server socket
  struct sockaddr_in server; // conexion info of servSock

  // check parameters
  if (argc != 2) {
    fprintf(stderr, "usage: %s port\n", argv[0]);
    exit(1);
  }

  // initialisation of the server socket
  if ((servSock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "%s: socket %s\n", argv[0], strerror(errno));
    exit(1);
  }

  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(argv[1]));
  server.sin_addr.s_addr = INADDR_ANY;
  if (bind(servSock, (struct sockaddr *)&server, sizeof(server)) < 0) {
    fprintf(stderr, "%s: bind %s\n", argv[0], strerror(errno));
    exit(1);
  }

  if (listen(servSock, 5) != 0) {
    fprintf(stderr, "%s: listen %s\n", argv[0], strerror(errno));
    exit(1);
  }

  /************ WAITING FOR CLIENTS ************/
  while (1) {
    // connexion of the first client
    struct Player *player1 = malloc(sizeof(struct Player)); // First player
    player1->sock = accept(servSock, (struct sockaddr *)&(player1->client),
                           &(socklen_t){sizeof(player1->client)});

    // connexion of the second client
    struct Player *player2 = malloc(sizeof(struct Player)); // second player
    player2->sock = accept(servSock, (struct sockaddr *)&(player2->client),
                           &(socklen_t){sizeof(player2->client)});

    // launch the game in a child process
    pid_t numPID = fork();

    if (numPID == 0) {   // child process -> takes care of current clients
      close(servSock);   // this process must not listen to new connexion
      srand(time(NULL)); // mandatory for random generation

      // initialisation of the game
      initGame(&player1, &player2);

      // displayGrids(player1,player2);

      /************ GAME LOOP ************/
      while (1) {
        /*** player1 is playing ***/
        playerRound(player1, player2);
        // displayGrids(player1,player2);

        /*** player2 is playing ***/
        playerRound(player2, player1);
        // only for debug
        // displayGrids(player1,player2);

        /*** End of game check ***/
        if (endGameCheck(player1, player2)) {
          break;
        }
      }

      // if we are out of the game loop,then this process must close all his
      // sockets and shutdown
      close(player1->sock);
      close(player2->sock);
      free(player1);
      free(player2);
      return 0;
    }

    else { // parent process -> keeps waiting for new connections
      close(player1->sock);
      close(player2->sock);
      free(player1);
      free(player2);
      player1 = NULL;
      player2 = NULL;
    }
  }
  close(servSock);
  return 0;
}
