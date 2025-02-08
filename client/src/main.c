#include "hmi.h"

int main(int argc, char **argv) {
  /************ INITIALIZATION OF THE CONNECTION ************/

  // check parameters
  if (argc != 3) {
    fprintf(stderr, "usage: %s server port\n", argv[0]);
    exit(1);
  }

  int sock = 0;              /* communication socket */
  struct sockaddr_in server; /* information about server socket */
  bool isFirstPlayer;

  // initialization of the connexion
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    fprintf(stderr, "%s: socket %s\n", argv[0], strerror(errno));
    exit(1);
  }
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(argv[2]));
  inet_aton(argv[1], &server.sin_addr);

  if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
    fprintf(stderr, "%s: connect %s\n", argv[0], strerror(errno));
    perror("bind");
    exit(1);
  }

  /************ INITIALIZATION OF THE CLIENT ************/

  // Receive the parameters of the game
  struct GameParameters param = receiveParam(&sock, &server);

  // set of functions used to respect the order of play
  void (*round[2][2])(int *, struct sockaddr_in *, char *, char *,
                      struct GameParameters) = {{opponentRound, playerRound},
                                                {playerRound, opponentRound}};

  // Receive the play order
  isFirstPlayer = (receiveSignal(&sock, &server) == FIRST);

  struct Ship shipArray[param.nbShip];
  char playerGrid[param.gridSize][param.gridSize];
  char opponentGrid[param.gridSize][param.gridSize];

  // ships placement
  placeShips(&sock, &server, param, shipArray);

  // fill both oponenent and player's grid
  initGrids(shipArray, (char *)playerGrid, (char *)opponentGrid, param);

  /********************* GAME LOOP *********************/
  while (true) {

    // progress of a complete game round with the player's and opponent's round
    (*(round[(unsigned short)isFirstPlayer][0]))(
        &sock, &server, (char *)playerGrid, (char *)opponentGrid, param);
    (*(round[(unsigned short)isFirstPlayer][1]))(
        &sock, &server, (char *)playerGrid, (char *)opponentGrid, param);

    // end of game check
    if (endGame(&sock, &server)) {
      break;
    }
  }

  // free the pointer inside param
  free(param.shipSizeArray);

  close(sock);
  return 0;
}
