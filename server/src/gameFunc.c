#include "gameFunc.h"

void sendSignal(enum SocketSignal sig, struct Player *player) {
  char buffer = sig;
  int errorCode = write(player->sock, &buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(player->client.sin_addr), ip, INET_ADDRSTRLEN);
    printf("write error on the socket(%s:%d): write=%d: %s\n", ip,
           ntohs(player->client.sin_port), errorCode, strerror(errno));
    exit(errno);
  }
}

void sendCoordinates(struct Coordinates *coord, struct Player *player) {
  char buffer[2];
  buffer[0] = coord->x;
  buffer[1] = coord->y;
  int errorCode = write(player->sock, buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(player->client.sin_addr), ip, INET_ADDRSTRLEN);
    printf("write error on the socket(%s:%d): write=%d: %s\n", ip,
           ntohs(player->client.sin_port), errorCode, strerror(errno));
    exit(errno);
  }
}

void sendPlacement(struct Placement *placement, struct Player *player) {
  char buffer[3];
  buffer[0] = placement->coordinates.x;
  buffer[1] = placement->coordinates.y;
  buffer[2] = placement->direction;
  int errorCode = write(player->sock, buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(player->client.sin_addr), ip, INET_ADDRSTRLEN);
    printf("write error on the socket(%s:%d): write=%d: %s\n", ip,
           ntohs(player->client.sin_port), errorCode, strerror(errno));
    exit(errno);
  }
}

void sendParam(unsigned short nbShip, unsigned short gridSize,
               const unsigned short shipSizeArray[], struct Player *player) {
  // first TCP send : nbShip and gridSize
  char buffer[2];
  buffer[0] = nbShip;
  buffer[1] = gridSize;

  int errorCode = write(player->sock, buffer, sizeof(buffer));
  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(player->client.sin_addr), ip, INET_ADDRSTRLEN);
    printf("write error on the socket(%s:%d): write=%d: %s\n", ip,
           ntohs(player->client.sin_port), errorCode, strerror(errno));
    exit(errno);
  }

  // second TCP send: shipSizeArray
  char bufferShipSize[nbShip];
  for (unsigned short i = 0; i < nbShip; ++i) {
    bufferShipSize[i] = shipSizeArray[i];
  }

  errorCode = write(player->sock, bufferShipSize, sizeof(bufferShipSize));
  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(player->client.sin_addr), ip, INET_ADDRSTRLEN);
    printf("write error on the socket(%s:%d): write=%d: %s\n", ip,
           ntohs(player->client.sin_port), errorCode, strerror(errno));
    exit(errno);
  }
}

void sendShips(unsigned short nbShip, struct Ship shipArray[],
               struct Player *player) {
  char shipBuffer[(nbShip * 5)];

  for (unsigned short i = 0; i < nbShip; ++i) {
    shipBuffer[(i * 5)] = shipArray[i].id;
    shipBuffer[(i * 5) + 1] = shipArray[i].size;
    shipBuffer[(i * 5) + 2] = shipArray[i].placement.direction;
    shipBuffer[(i * 5) + 3] = shipArray[i].placement.coordinates.x;
    shipBuffer[(i * 5) + 4] = shipArray[i].placement.coordinates.y;
  }

  int errorCode = write(player->sock, shipBuffer, sizeof(shipBuffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(player->client.sin_addr), ip, INET_ADDRSTRLEN);
    printf("write error on the socket(%s:%d): write=%d: %s\n", ip,
           ntohs(player->client.sin_port), errorCode, strerror(errno));
    exit(errno);
  }
}

enum SocketSignal receiveSignal(struct Player *player) {
  char buffer;
  int errorCode = read(player->sock, &buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(player->client.sin_addr), ip, INET_ADDRSTRLEN);
    printf("read error on the socket(%s:%d): read=%d: %s\n", ip,
           ntohs(player->client.sin_port), errorCode, strerror(errno));
    exit(errno);
  }

  return buffer;
}

struct Coordinates receiveCoordinates(struct Player *player) {
  char buffer[2];
  int errorCode = read(player->sock, buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(player->client.sin_addr), ip, INET_ADDRSTRLEN);
    printf("read error on the socket(%s:%d): read=%d: %s\n", ip,
           ntohs(player->client.sin_port), errorCode, strerror(errno));
    exit(errno);
  }

  struct Coordinates coord;
  coord.x = (short)buffer[0];
  coord.y = (short)buffer[1];
  return coord;
}

struct Placement receivePlacement(struct Player *player) {
  char buffer[3];
  int errorCode = read(player->sock, buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(player->client.sin_addr), ip, INET_ADDRSTRLEN);
    printf("read error on the socket(%s:%d): read=%d: %s\n", ip,
           ntohs(player->client.sin_port), errorCode, strerror(errno));
    exit(errno);
  }

  struct Placement placement;
  placement.coordinates.x = (short)buffer[0];
  placement.coordinates.y = (short)buffer[1];
  placement.direction = buffer[2];
  return placement;
}

enum SocketSignal checkShipPlacement(short size, struct Placement *placement,
                                     struct Cell grid[][GRID_SIZE]) {
  // check if the direction is 0 or 1
  if (placement->direction != HORIZONTAL && placement->direction != VERTICAL) {
    return WRONG_DIRECTION;
  }

  // check is the ship is inside the grid
  if (placement->coordinates.x < 0 || placement->coordinates.y < 0 ||
      (placement->direction == VERTICAL &&
       (placement->coordinates.x >= GRID_SIZE ||
        placement->coordinates.y + size - 1 >= GRID_SIZE)) ||
      (placement->direction == HORIZONTAL &&
       (placement->coordinates.x + size - 1 >= GRID_SIZE ||
        placement->coordinates.y >= GRID_SIZE))) {
    return OUT_OF_GRID;
  }

  // check if there is no ships at this place
  for (short i = 0; i < size; ++i) {
    if (grid[placement->coordinates.y +
             ((placement->direction == VERTICAL) * i)]
            [placement->coordinates.x +
             ((placement->direction == HORIZONTAL) * i)]
                .boatId != NB_SHIP) {
      return PLACE_TAKEN;
    }
  }
  // everything's good,the ship can be placed here
  return TRUE;
}

void initGrid(struct Cell grid[][GRID_SIZE]) {
  // fill the grid with empty cells
  for (unsigned short y = 0; y < GRID_SIZE; ++y) {
    for (unsigned short x = 0; x < GRID_SIZE; x++) {
      grid[y][x].boatId = (unsigned short)NB_SHIP;
      grid[y][x].targeted = false;
    }
  }
}

void randomPlaceShips(struct Cell grid[][GRID_SIZE], struct Ship shipArray[]) {
  // random placement of 5 ships
  struct Placement placement;
  for (unsigned short id = 0; id < NB_SHIP; ++id) {
    // roll random value until target cells are empty
    do {
      placement.direction = rand() % 2;
      if (placement.direction == VERTICAL) {
        placement.coordinates.x = rand() % GRID_SIZE;
        placement.coordinates.y =
            rand() % (GRID_SIZE + 1 - (SHIP_SIZE_ARRAY[id]));
      } else {
        placement.coordinates.x =
            rand() % (GRID_SIZE + 1 - (SHIP_SIZE_ARRAY[id]));
        placement.coordinates.y = rand() % GRID_SIZE;
      }
    } while (checkShipPlacement((SHIP_SIZE_ARRAY[id]), &placement, grid) !=
             TRUE);

    // add the ship to the list
    shipArray[id].placement = placement;
    shipArray[id].size = SHIP_SIZE_ARRAY[id];
    shipArray[id].id = id;

    // place the ship
    for (unsigned short j = 0; j < (SHIP_SIZE_ARRAY[id]); ++j) {
      grid[placement.coordinates.y + ((placement.direction == VERTICAL) * j)]
          [placement.coordinates.x + ((placement.direction == HORIZONTAL) * j)]
              .boatId = id;
    }
  }
}

void displayGrid(struct Cell grid[][GRID_SIZE]) {
  for (unsigned short y = 0; y < GRID_SIZE; y++) {
    for (unsigned short x = 0; x < GRID_SIZE; x++) {
      // change the color if needed
      if (grid[y][x].targeted) {
        printf("\033[31m"); // red
      } else if (grid[y][x].boatId != NB_SHIP) {
        printf("\033[33m"); // orange
      }
      // display the id of the ship, E(mpty) else
      if (grid[y][x].boatId == NB_SHIP) {
        printf("E");
      } else {
        printf("%hu", grid[y][x].boatId);
      }
      // reset the color of the promt
      printf("\033[m ");
    }
    printf("\n");
  }
  printf("\n");
}

void displayGrids(struct Player *player1, struct Player *player2) {
  // NAMES
  printf("      Player1       	      Player2\n");
  // GRIDS
  for (unsigned short y = 0; y < GRID_SIZE; y++) {
    // line of the first grid
    for (unsigned short x = 0; x < GRID_SIZE; x++) {
      // change the color if needed
      if (player1->grid[y][x].targeted) {
        printf("\033[31m"); // red
      } else if (player1->grid[y][x].boatId != NB_SHIP) {
        printf("\033[33m"); // orange
      }
      // display the id of the ship, E(mpty) else
      if (player1->grid[y][x].boatId == NB_SHIP) {
        printf("E");
      } else {
        printf("%hu", player1->grid[y][x].boatId);
      }
      // reset the color of the promt
      printf("\033[m ");
    }
    printf("\t");
    // line of the second grid
    for (unsigned short x = 0; x < GRID_SIZE; x++) {
      // change the color if needed
      if (player2->grid[y][x].targeted) {
        printf("\033[31m"); // red
      } else if (player2->grid[y][x].boatId != NB_SHIP) {
        printf("\033[33m"); // orange
      }
      // display the id of the ship, E(mpty) else
      if (player2->grid[y][x].boatId == NB_SHIP) {
        printf("E");
      } else {
        printf("%hu", player2->grid[y][x].boatId);
      }
      // reset the color of the promt
      printf("\033[m ");
    }
    printf("\n");
  }
  printf("\n");
}

bool isSunk(struct Cell grid[][GRID_SIZE], struct Coordinates *coord) {
  short count = 1;
  short shipSize = SHIP_SIZE_ARRAY[grid[coord->y][coord->x].boatId];
  // loop according to ship size and number of healthy cells remaining
  for (short i = 1; i < shipSize && count < shipSize; ++i) {
    // bot cells
    count +=
        (coord->y + i < GRID_SIZE // not out of the grid
         && grid[coord->y + i][coord->x].boatId ==
                grid[coord->y][coord->x].boatId          // is the same boat
         && grid[coord->y + i][coord->x].targeted == 1); // is already targeted
    // top cells
    count +=
        (coord->y - 1 >= 0 // not out of the grid
         && grid[coord->y - i][coord->x].boatId ==
                grid[coord->y][coord->x].boatId          // is the same boat
         && grid[coord->y - i][coord->x].targeted == 1); // is already targeted
    // right cells
    count +=
        (coord->x + i < GRID_SIZE // not out of the grid
         && grid[coord->y][coord->x + i].boatId ==
                grid[coord->y][coord->x].boatId          // is the same boat
         && grid[coord->y][coord->x + i].targeted == 1); // is already targeted
    // left cells
    count +=
        (coord->x - i >= 0 // not out of the grid
         && grid[coord->y][coord->x - i].boatId ==
                grid[coord->y][coord->x].boatId          // is the same boat
         && grid[coord->y][coord->x - i].targeted == 1); // is already targeted
  }
  return count == shipSize;
}

short fire(struct Cell grid[][GRID_SIZE], struct Coordinates *coord) {
  if (coord->y < 0 || coord->y > GRID_SIZE - 1 || coord->x < 0 ||
      coord->x > GRID_SIZE - 1) {
    return OUT_OF_GRID;
  }
  if (grid[coord->y][coord->x].targeted) {
    return ALREADY_TARGETED;
  } else if (grid[coord->y][coord->x].boatId != NB_SHIP) {
    grid[coord->y][coord->x].targeted = true;
    if (isSunk(grid, coord)) {
      return SUNK;
    } else {
      return HIT;
    }
  } else {
    grid[coord->y][coord->x].targeted = true;
    return MISS;
  }
}

void askPlayerToPlaceShips(struct Player *player) {
  // the list of ship placement that the function will send to the client
  struct Ship shipArray[5];

  // pings the player to tell him to start choosing
  sendSignal(PING, player);

  if (receiveSignal(player) ==
      TRUE) { // the player want to choose the place of ships
    struct Placement placement;
    enum SocketSignal isPlaced;
    for (short id = 0; id < NB_SHIP; ++id) {
      // receive player placement
      do {
        placement = receivePlacement(player);
        isPlaced =
            checkShipPlacement(SHIP_SIZE_ARRAY[id], &placement, player->grid);
        sendSignal(isPlaced, player);
      } while (isPlaced != TRUE);

      // add the ship to the array
      shipArray[id].placement = placement;
      shipArray[id].id = id;
      shipArray[id].size = SHIP_SIZE_ARRAY[id];

      // place the ship
      for (short j = 0; j < SHIP_SIZE_ARRAY[id]; ++j) {
        player
            ->grid[placement.coordinates.y + (placement.direction * j)]
                  [placement.coordinates.x + (!placement.direction * j)]
            .boatId = id;
      }
      // displayGrid(player->grid);
    }
  } else { // the player don't want to choose the place of ships
    randomPlaceShips(player->grid, shipArray);
  }

  // send the ship list to the player
  sendShips(NB_SHIP, shipArray, player);
}

void initGame(struct Player **player1, struct Player **player2) {
  // send the parameters to both player1 and player2
  sendParam(NB_SHIP, GRID_SIZE, SHIP_SIZE_ARRAY, *player1);
  sendParam(NB_SHIP, GRID_SIZE, SHIP_SIZE_ARRAY, *player2);

  // swap players randomly
  if (rand() % 2) {
    struct Player *tmpPtr = *player1;
    *player1 = *player2;
    *player2 = tmpPtr;
  }

  // assigns the order of play
  sendSignal(FIRST, *player1);
  sendSignal(SECOND, *player2);

  // init game grid of both players
  initGrid((*player1)->grid);
  initGrid((*player2)->grid);

  // place ships
  // for the first player
  askPlayerToPlaceShips(*player1);
  // for the second player
  askPlayerToPlaceShips(*player2);

  // init scores of both players
  (*player1)->score = 0;
  (*player2)->score = 0;

  // sends a ping to each player to tell them that the init step is over
  sendSignal(PING, *player1);
  sendSignal(PING, *player2);
}

void playerRound(struct Player *player, struct Player *opponent) {
  enum SocketSignal signal;
  struct Coordinates fireCoordinatess;

  // receive the shot coordinate until they are correct
  do {
    // receive the Coordinatess from the player
    fireCoordinatess = receiveCoordinates(player);
    // shoot at the Coordinatess targeted by the client1
    signal = fire(opponent->grid, &fireCoordinatess);
    // update the score if a boat is sunk
    player->score += (signal == SUNK);
    // send the result to the player
    sendSignal(signal, player);
  } while (signal == ALREADY_TARGETED || signal == OUT_OF_GRID);

  // send the result to the opponent
  sendCoordinates(&fireCoordinatess, opponent);
  sendSignal(signal, opponent);
}

bool endGameCheck(struct Player *player1, struct Player *player2) {
  // DRAW
  if (player1->score == NB_SHIP && player2->score == NB_SHIP) {
    // send the result of the game for both clients
    sendSignal(DRAW, player1);
    sendSignal(DRAW, player2);
    return true;
  }
  // WIN for player1
  else if (player1->score == NB_SHIP) {
    // send the result of the game for both clients
    sendSignal(VICTORY, player1);
    sendSignal(DEFEAT, player2);
    return true;
  }
  // WIN for player2
  else if (player2->score == NB_SHIP) {
    // send the result of the game for both clients
    sendSignal(DEFEAT, player1);
    sendSignal(VICTORY, player2);
    return true;
  }
  // nobody wins yet,the game is still running
  else {
    // send the result of the game for both clients
    sendSignal(NOT_FINISHED, player1);
    sendSignal(NOT_FINISHED, player2);
    return false;
  }
}
