#ifndef GAMEFUNC_H
#define GAMEFUNC_H

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "param.h"

// add C++ like boolean support
#define true 1
#define false 0
#define bool char

// Signal used to communicate between server and client
enum SocketSignal {
  FALSE = 0,
  TRUE = 1,
  HIT = 2,
  MISS = 3,
  SUNK = 4,
  ALREADY_TARGETED = 5,
  OUT_OF_GRID = 6,
  VICTORY = 7,
  DEFEAT = 8,
  DRAW = 9,
  NOT_FINISHED = 10,
  FIRST = 11,
  SECOND = 12,
  PING = 13,
  PLACE_TAKEN = 14,
  WRONG_DIRECTION = 15
};

// Direction of a ship
enum Direction { HORIZONTAL = 0, VERTICAL = 1 };

// Coordinatess on a plane of positive integers
struct Coordinates {
  short x; // abscissa
  short y; // ordinate
};

// Placement for a ship with its cooordinates and its direction
struct Placement {
  struct Coordinates coordinates;
  enum Direction direction;
};

// Ship contain all the information about a ship
struct Ship {
  struct Placement placement;
  unsigned short id;
  unsigned short size;
};

// define one cell of the game grid
struct Cell {
  unsigned short boatId; // contain the id of the boat in the cell,if there is
                         // no boat: NB_SHIP
  bool targeted;         // 1 if targeted,else 0
};

// every informations about a player
struct Player {
  int sock;                               // conexion socket
  struct sockaddr_in client;              // information about client socket
  unsigned short score;                   // counter of ships sunk by the player
  struct Cell grid[GRID_SIZE][GRID_SIZE]; // player grid
};

/**
 * Sends a signal to the client. If there is an error during the function, it
 * stops the whole process, shows the error, and exits with the write error
 * code.
 *
 * @param sig The signal to send.
 * @param player The target player of the signal.
 */
void sendSignal(enum SocketSignal sig, struct Player *player);

/**
 * Sends coordinates to the client. If there is an error during the function, it
 * stops the whole process, shows the error, and exits with the write error
 * code.
 *
 * @param coord The coordinates to send.
 * @param player The target player of the signal.
 */
void sendCoordinates(struct Coordinates *coord, struct Player *player);

/**
 * Sends a placement to the client. If there is an error during the function, it
 * stops the whole process, shows the error, and exits with the write error
 * code.
 *
 * @param placement The placement to send.
 * @param player The target player of the signal.
 */
void sendPlacement(struct Placement *placement, struct Player *player);

/**
 * Sends game parameters to the client using two TCP exchanges. The first
 * exchange sends the number of ships and grid size, and the second exchange
 * sends a list of ship sizes. If there is an error during the function, it
 * stops the whole process, shows the error, and exits with the write error
 * code.
 *
 * @param nbShip The number of ships.
 * @param gridSize The size of the grid.
 * @param shipSizeArray An array containing the sizes of all available ships.
 * @param player The target player of the signal.
 */
void sendParam(unsigned short nbShip, unsigned short gridSize,
               const unsigned short shipSizeArray[], struct Player *player);

/**
 * Sends a list of Ship structures to the client in one TCP exchange. If there
 * is an error during the function, it stops the whole process, shows the error,
 * and exits with the write error code.
 *
 * @param nbShip The number of ships to send.
 * @param shipArray An array containing the ships to send.
 * @param player The target player of the signal.
 */
void sendShips(unsigned short nbShip, struct Ship shipArray[],
               struct Player *player);

/**
 * Receives a signal sent by the client. If there is an error during the
 * function, it stops the whole process, shows the error, and exits with the
 * read error code.
 *
 * @param player The player who sent the signal.
 * @return The signal received from the client.
 */
enum SocketSignal receiveSignal(struct Player *player);

/**
 * Receives coordinates sent by the client. If there is an error during the
 * function, it stops the whole process, shows the error, and exits with the
 * read error code.
 *
 * @param player The player who sent the coordinates.
 * @return The coordinates received from the client.
 */
struct Coordinates receiveCoordinates(struct Player *player);

/**
 * Receives a placement sent by the client. If there is an error during the
 * function, it stops the whole process, shows the error, and exits with the
 * read error code.
 *
 * @param player The player who sent the placement.
 * @return The placement received from the client.
 */
struct Placement receivePlacement(struct Player *player);

/**
 * Checks if a ship can be placed on the grid. Returns TRUE if the ship can be
 * placed, OUT_OF_GRID if the ship doesn't fit in the grid, or PLACE_TAKEN if
 * there is another ship at the specified location.
 *
 * @param size The size of the ship.
 * @param placement The placement of the ship.
 * @param grid The game grid.
 * @return A SocketSignal indicating the result of the placement check.
 */
enum SocketSignal checkShipPlacement(short size, struct Placement *placement,
                                     struct Cell grid[][GRID_SIZE]);

/**
 * Initializes the grid with default values.
 *
 * @param grid The game grid to initialize.
 */
void initGrid(struct Cell grid[][GRID_SIZE]);

/**
 * Randomly places ships on the grid and fills the shipArray with the
 * placements.
 *
 * @param grid The game grid.
 * @param shipArray The array to be filled with the random ship placements.
 */
void randomPlaceShips(struct Cell grid[][GRID_SIZE], struct Ship shipArray[]);

/**
 * Displays the grid, with ships represented by their sizes and hit cells marked
 * in red. (debug only function)
 *
 * @param grid The game grid to display.
 */
void displayGrid(struct Cell grid[][GRID_SIZE]);

/**
 * Displays two grids side by side, with ships represented by their sizes and
 * hit cells marked in red. (debug only function)
 *
 * @param player1 The first player's grid.
 * @param player2 The second player's grid.
 */
void displayGrids(struct Player *player1, struct Player *player2);

/**
 * Checks if the ship at the specified coordinates is sunk.
 *
 * @param grid The game grid.
 * @param coord The coordinates of a cell in the ship.
 * @return True if the ship is sunk, false otherwise.
 */
bool isSunk(struct Cell grid[][GRID_SIZE], struct Coordinates *coord);

/**
 * Fires at the specified coordinates on the grid and returns the result of the
 * shot.
 *
 * @param grid The game grid.
 * @param coord The coordinates to fire at.
 * @return The result of the shot.
 */
short fire(struct Cell grid[][GRID_SIZE], struct Coordinates *coord);

/*the functions in this section are just groupings of the low-level
  functions defined above in order to make the main.c more simple and modular*/

/**
 * Receives information about the player's ship placement. The player can choose
 * to randomize the placement.
 *
 * @param player The player whose ship placement is being received.
 */
void askPlayerToPlaceShips(struct Player *player);

/**
 * Initializes the game by choosing the order of play for each player, setting
 * the score to 0, and initializing the grid with ships.
 *
 * @param player1 A pointer to the first player.
 * @param player2 A pointer to the second player.
 */
void initGame(struct Player **player1, struct Player **player2);

/**
 * Executes a player's round by listening for coordinates, shooting at the
 * opponent's grid, increasing the score if a ship is sunk, and sending the
 * result of the shot to both players.
 *
 * @param player The player whose round it is.
 * @param opponent The opponent player.
 */
void playerRound(struct Player *player, struct Player *opponent);

/**
 * Checks if the game is finished and returns true if it is, false otherwise.
 * Sends the result to both players.
 *
 * @param player1 The first player.
 * @param player2 The second player.
 * @return True if the game is finished, false otherwise.
 */
bool endGameCheck(struct Player *player1, struct Player *player2);

#endif // GAMEFUNC_H
