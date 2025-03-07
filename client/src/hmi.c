#include "hmi.h"

void displayGrid(char *grid, struct GameParameters param) {

  // the abscissa indicator
  printf("    \033[1m");
  for (short x = 1; x <= param.gridSize; ++x) {
    // for the 1 digit number
    if (x < 10) {
      printf("%hu ", x);
    }
    // for the two digit numbers
    else if (x % 2 == 0 && x < 100) {
      printf("%hu  ", x);
    } else if (x % 2 == 0) {
      printf("%hu ", x);
    }
  }

  // the grid
  printf("\033[0m\n");
  for (short y = 1; y <= param.gridSize; ++y) {

    // ordinate indicator
    if (y < 10) {
      printf("\033[1m%hu\033[0m   ", y);
    } else if (y < 100) {
      printf("\033[1m%hu\033[0m  ", y);
    } else {
      printf("\033[1m%hu\033[0m ", y);
    }

    for (short x = 0; x < param.gridSize; ++x) {
      printf("%c ", *(grid + ((y - 1) * param.gridSize) + x));
    }
    printf("\n");
  }
}

void displayGrids(char *playerGrid, char *opponentGrid,
                  struct GameParameters param) {

  printf("\n");
  printf("opponent's grid:\n");
  displayGrid(opponentGrid, param);

  printf("\n");
  printf("your grid:\n");
  displayGrid(playerGrid, param);
}

void initGrids(struct Ship shipArray[], char *playerGrid, char *opponentGrid,
               struct GameParameters param) {
  // fill both grid with empty cells: ~
  for (short y = 0; y < param.gridSize; ++y) {
    for (short x = 0; x < param.gridSize; ++x) {
      *(playerGrid + (y * param.gridSize) + x) = '~';
      *(opponentGrid + (y * param.gridSize) + x) = '~';
    }
  }

  // place ships for the player's grid
  for (short id = 0; id < param.nbShip; ++id) {

    switch (shipArray[id].placement.direction) {
    case HORIZONTAL:
      for (short i = 0; i < param.shipSizeArray[id]; ++i) {
        *(playerGrid +
          (shipArray[id].placement.coordinates.y * param.gridSize) +
          shipArray[id].placement.coordinates.x + i) =
            (char)(param.shipSizeArray[id] + 48);
      }
      break;

    case VERTICAL:
      for (short i = 0; i < param.shipSizeArray[id]; ++i) {
        *(playerGrid +
          ((shipArray[id].placement.coordinates.y + i) * param.gridSize) +
          shipArray[id].placement.coordinates.x) =
            (char)(param.shipSizeArray[id] + 48);
      }
      break;
    }
  }
}

void placeShips(int *sock, struct sockaddr_in *server,
                struct GameParameters param, struct Ship shipArray[]) {

  printf("\n\nGAME INITIALIZATION\nwaiting for the opponent ...\n");
  // wait for the server's ping
  receiveSignal(sock, server);

  char answer[1 << 8];
  short scanfResult = 0;
  // the player choose between a random and a manual placement
  do {
    printf("manual ship placement?(y/n)\n");
    scanfResult = scanf("%s", answer);
  } while (answer[0] != 'y' && answer[0] != 'n' && scanfResult != 1);

  if (answer[0] == 'y') {
    sendSignal(sock, server, TRUE);
    struct Placement placement;
    char abscissa[1 << 8], ordinate[1 << 8], direction[1 << 8];
    short scanfResult = 0;
    enum SocketSignal isPlaced;
    for (short id = 0; id < param.nbShip; ++id) {
      isPlaced = TRUE;
      do {
        switch (isPlaced) {
        case OUT_OF_GRID:
          printf("The ship is out of the grid!\n");
          break;
        case PLACE_TAKEN:
          printf("there is at least one other ship in this position.\n");
          break;
        case WRONG_DIRECTION:
          printf("Orientation must be 0 or 1.\n");
          break;

        default:
          break;
        }
        do {
          printf(
              "\nChoose the coordinates of the ship size %d\nformat : "
              "ordinate abscissa orientation (1 = vertical 0 = horizontal): ",
              param.shipSizeArray[id]);
          scanfResult = scanf("%s%s%s", abscissa, ordinate, direction);
        } while (scanfResult != 3);
        placement.coordinates.x = atoi(abscissa) - 1;
        placement.coordinates.y = atoi(ordinate) - 1;
        placement.direction = atoi(direction);
        sendPlacement(sock, server, &placement);
        isPlaced = receiveSignal(sock, server);
      } while (isPlaced != TRUE);
    }
  } else {
    sendSignal(sock, server, FALSE);
  }

  receiveShips(sock, server, param.nbShip, shipArray);

  printf("waiting for the opponent...\n");
  // wait for the server's ping
  receiveSignal(sock, server);
}

void playerRound(int *sock, struct sockaddr_in *server, char *playerGrid,
                 char *opponentGrid, struct GameParameters param) {
  enum SocketSignal signal;
  struct Coordinates coord;
  char firstNumber[1 << 8], secondNumber[1 << 8];
  short scanfResult = 0;

  printf("\n\nYOUR TURN ...\n");

  // print grids
  displayGrids(playerGrid, opponentGrid, param);

  // ask the player for coordinate until they are correct
  do {
    // ask the coordinate to the player
    do {
      printf("choose coordinates (example: 10 9):");
      scanfResult = scanf("%s%s", firstNumber, secondNumber);
    } while (scanfResult != 2);

    // convert numbers to  integers and send them to the server
    coord.x = atoi(firstNumber) - 1;
    coord.y = atoi(secondNumber) - 1;

    sendCoordinates(sock, server, &coord);

    // the result is received in char (1 byte)
    signal = receiveSignal(sock, server);
    // result of the shot
    switch (signal) {
    case ALREADY_TARGETED:
      printf("cell already targeted!\n");
      break;
    case OUT_OF_GRID:
      printf("bad coordinates! Out of the grid!\n");
      break;
    default:
      break;
    }
  } while (signal == ALREADY_TARGETED || signal == OUT_OF_GRID);

  // result of the shot
  switch (signal) {
  case HIT:
    *(opponentGrid + (coord.y * param.gridSize) + coord.x) = 'X';
    printf("HIT!\n");
    break;
  case MISS:
    *(opponentGrid + (coord.y * param.gridSize) + coord.x) = 'O';
    printf("MISS!\n");
    break;
  case SUNK:
    *(opponentGrid + (coord.y * param.gridSize) + coord.x) = 'X';
    printf("SINK!\n");
    break;
  default:
    break;
  }
}

void opponentRound(int *sock, struct sockaddr_in *server, char *playerGrid,
                   char *opponentGrid, struct GameParameters param) {
  enum SocketSignal signal;
  struct Coordinates coord;

  // wait for the opponent play
  printf("\n\nOPPONENT'S TURN ...\n");
  // the result is received in char (1 byte)
  coord = receiveCoordinates(sock, server);
  signal = receiveSignal(sock, server);

  // TO FIX: the following line is used to not raise the unused-parameter error
  // on the opponent grid param
  displayGrids(playerGrid, opponentGrid, param);

  printf("opponent's last play : x:%d,y:%d -> ", coord.x + 1, coord.y + 1);
  // result of the shot
  switch (signal) {
  case HIT:
    *(playerGrid + (coord.y * param.gridSize) + coord.x) = 'X';
    printf("HIT!\n");
    break;
  case MISS:
    *(playerGrid + (coord.y * param.gridSize) + coord.x) = 'O';
    printf("MISS!\n");
    break;
  case SUNK:
    *(playerGrid + (coord.y * param.gridSize) + coord.x) = 'X';
    printf("SINK!\n");
    break;

  default:
    break;
  }
}

bool endGame(int *sock, struct sockaddr_in *server) {
  enum SocketSignal signal = receiveSignal(sock, server);
  switch (signal) {
  case NOT_FINISHED:
    return false;
  case VICTORY:
    printf("VICTORY!\n");
    return true;
  case DEFEAT:
    printf("DEFEAT!\n");
    return true;
  case DRAW:
    printf("DRAW!!\n");
    return true;

  default:
    return false;
  }
}
