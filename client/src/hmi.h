#ifndef HMI_H
#define HMI_H

#include "gameFunc.h"

/**
 * Displays the opponent's grid above the player's grid.
 *
 * @param playerGrid The player's grid.
 * @param opponentGrid The opponent's grid.
 * @param param The parameters containing the size of the grids.
 */
void displayGrids(char *playerGrid, char *opponentGrid,
                  struct GameParameters param);

/**
 * Initializes the player's grid with all player's ships represented by their
 * size. Other cells of the grid are filled with empty char (~). The opponent's
 * grid is filled with empty cells.
 *
 * @param shipArray The list of ship positions.
 * @param playerGrid The grid of the player.
 * @param opponentGrid The grid of the opponent.
 * @param param The parameters containing the size of the grids.
 */
void initGrids(struct Ship shipArray[], char *playerGrid, char *opponentGrid,
               struct GameParameters param);

/**
 * Asks the player if they want to manually place their ships. If so, asks for
 * the placement of each ship.
 *
 * @param sock The socket to the server.
 * @param server The socket information.
 * @param param The parameters containing the number of ships and grid size.
 * @param shipArray An array which will contain all ships' placements.
 */
void placeShips(int *sock, struct sockaddr_in *server,
                struct GameParameters param, struct Ship shipArray[]);

/**
 * Asks the player to shoot on the opponent's grid and displays the result.
 *
 * @param sock The socket to the server.
 * @param server The socket information.
 * @param playerGrid The grid of the player.
 * @param opponentGrid The grid of the opponent.
 * @param param The parameters containing the size of the grids.
 */
void playerRound(int *sock, struct sockaddr_in *server, char *playerGrid,
                 char *opponentGrid, struct GameParameters param);

/**
 * Displays the result of the opponent's turn.
 *
 * @param sock The socket to the server.
 * @param server The socket information.
 * @param playerGrid The grid of the player.
 * @param opponentGrid The grid of the opponent.
 * @param param The parameters containing the size of the grids.
 */
void opponentRound(int *sock, struct sockaddr_in *server, char *playerGrid,
                   char *opponentGrid, struct GameParameters param);

/**
 * Returns true and displays the result of the game if it's over; otherwise,
 * returns false.
 *
 * @param sock The socket to the server.
 * @param server The socket information.
 * @return True if the game is over, false otherwise.
 */
bool endGame(int *sock, struct sockaddr_in *server);

#endif // HMI_H
