#ifndef GAMEFUNC_H
#define GAMEFUNC_H

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

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

struct Ship {
  struct Placement placement;
  unsigned short id;
  unsigned short size;
};

struct GameParameters {
  unsigned short nbShip;
  unsigned short gridSize;
  unsigned short *shipSizeArray;
};

/**
 * Sends a signal to the server. If there is an error during the function, it
 * stops the whole process, shows the error, and exits with the write error
 * code.
 *
 * @param sock The socket to the server.
 * @param server The socket information.
 * @param sig The signal to send.
 */
void sendSignal(int *sock, struct sockaddr_in *server, enum SocketSignal sig);

/**
 * Send Coordinates to the server,If there is an error during the function it
 * stops the whole process,show the error,and exit the write error code.
 *
 * @param sock: the socket to the server
 * @param server: socket info
 * @param coord: Coordinate which will be send
 */
void sendCoordinates(int *sock, struct sockaddr_in *server,
                     struct Coordinates *coord);

/**
 * Send Placement to the server,If there is an error during the function it
 * stops the whole process,show the error,and exit the write error code.
 *
 * @param sock: the socket to the server
 * @param server: socket info
 * @param placement: the Placement which will be send
 */
void sendPlacement(int *sock, struct sockaddr_in *server,
                   struct Placement *placement);

/**
 * Receives a signal sent by the server. If there is an error during the
 * function, it stops the whole process, shows the error, and exits with the
 * read error code.
 *
 * @param sock The socket to the server.
 * @param server The socket information.
 * @return The signal received from the server.
 */
enum SocketSignal receiveSignal(int *sock, struct sockaddr_in *server);

/**
 * Receives coordinates sent by the server. If there is an error during the
 * function, it stops the whole process, shows the error, and exits with the
 * read error code.
 *
 * @param sock The socket to the server.
 * @param server The socket information.
 * @return The coordinates received from the server.
 */
struct Coordinates receiveCoordinates(int *sock, struct sockaddr_in *server);

/**
 * Receives a placement sent by the server. If there is an error during the
 * function, it stops the whole process, shows the error, and exits with the
 * read error code.
 *
 * @param sock The socket to the server.
 * @param server The socket information.
 * @return The placement received from the server.
 */
struct Placement receivePlacement(int *sock, struct sockaddr_in *server);

/**
 * Receives game parameters sent by the server. Two TCP requests are needed:
 * the first one sends the number of ships and grid size, and the second one
 * sends a list of ship sizes. If there is an error during the function, it
 * stops the whole process, shows the error, and exits with the read error code.
 *
 * @param sock The socket to the server.
 * @param server The socket information.
 * @return The game parameters received from the server.
 */
struct GameParameters receiveParam(int *sock, struct sockaddr_in *server);

/**
 * Receives a list of ships into the shipArray parameter. The whole process uses
 * one TCP exchange. If there is an error during the function, it stops the
 * whole process, shows the error, and exits with the read error code.
 *
 * @param sock The socket to the server.
 * @param server The socket information.
 * @param nbShip The expected number of ships.
 * @param shipArray An array in which the received ships will be placed. Its
 * size must be the same as nbShip.
 */
void receiveShips(int *sock, struct sockaddr_in *server, unsigned short nbShip,
                  struct Ship shipArray[]);

#endif // GAMEFUNC_H
