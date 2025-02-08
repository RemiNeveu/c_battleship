#include "gameFunc.h"

void sendSignal(int *sock, struct sockaddr_in *server, enum SocketSignal sig) {
  char buffer = sig;
  int errorCode = write(*sock, &buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server->sin_addr), ip, INET_ADDRSTRLEN);
    printf("write error on the socket(%s:%d): write=%d: %s\n", ip,
           ntohs(server->sin_port), errorCode, strerror(errno));
    exit(errno);
  }
}

void sendCoordinates(int *sock, struct sockaddr_in *server,
                     struct Coordinates *coord) {
  char buffer[2];
  buffer[0] = coord->x;
  buffer[1] = coord->y;
  int errorCode = write(*sock, buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server->sin_addr), ip, INET_ADDRSTRLEN);
    printf("write error on the socket(%s:%d): write=%d: %s\n", ip,
           ntohs(server->sin_port), errorCode, strerror(errno));
    exit(errno);
  }
}

void sendPlacement(int *sock, struct sockaddr_in *server,
                   struct Placement *placement) {
  char buffer[3];
  buffer[0] = placement->coordinates.x;
  buffer[1] = placement->coordinates.y;
  buffer[2] = placement->direction;
  int errorCode = write(*sock, buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server->sin_addr), ip, INET_ADDRSTRLEN);
    printf("write error on the socket(%s:%d): write=%d: %s\n", ip,
           ntohs(server->sin_port), errorCode, strerror(errno));
    exit(errno);
  }
}

enum SocketSignal receiveSignal(int *sock, struct sockaddr_in *server) {
  char buffer;
  int errorCode = read(*sock, &buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server->sin_addr), ip, INET_ADDRSTRLEN);
    printf("read error on the socket(%s:%d): read=%d: %s\n", ip,
           ntohs(server->sin_port), errorCode, strerror(errno));
    exit(errno);
  }
  return buffer;
}

struct Coordinates receiveCoordinates(int *sock, struct sockaddr_in *server) {
  char buffer[2];
  int errorCode = read(*sock, buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(server->sin_addr), ip, INET_ADDRSTRLEN);
    printf("read error on the socket(%s:%d): read=%d: %s\n", ip,
           ntohs(server->sin_port), errorCode, strerror(errno));
    exit(errno);
  }

  struct Coordinates coord;
  coord.x = (short)buffer[0];
  coord.y = (short)buffer[1];
  return coord;
}

struct Placement receivePlacement(int *sock, struct sockaddr_in *server) {
  char buffer[3];
  int errorCode = read(*sock, buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(server->sin_addr), ip, INET_ADDRSTRLEN);
    printf("read error on the socket(%s:%d): read=%d: %s\n", ip,
           ntohs(server->sin_port), errorCode, strerror(errno));
    exit(errno);
  }

  struct Placement placement;
  placement.coordinates.x = (short)buffer[0];
  placement.coordinates.y = (short)buffer[1];
  placement.direction = buffer[2];
  return placement;
}

struct GameParameters receiveParam(int *sock, struct sockaddr_in *server) {
  struct GameParameters param;

  // first TCP send : nbShip and gridSize
  char buffer[2];
  int errorCode = read(*sock, buffer, sizeof(buffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(server->sin_addr), ip, INET_ADDRSTRLEN);
    printf("read error on the socket(%s:%d): read=%d: %s\n", ip,
           ntohs(server->sin_port), errorCode, strerror(errno));
    exit(errno);
  }

  param.nbShip = (short)buffer[0];
  param.gridSize = (unsigned short)buffer[1];
  param.shipSizeArray =
      (unsigned short *)malloc(param.nbShip * sizeof(unsigned short));

  // second TCP send: shipSizeArray
  char bufferShipSize[param.nbShip];
  errorCode = read(*sock, bufferShipSize, sizeof(bufferShipSize));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(server->sin_addr), ip, INET_ADDRSTRLEN);
    printf("read error on the socket(%s:%d): read=%d: %s\n", ip,
           ntohs(server->sin_port), errorCode, strerror(errno));
    exit(errno);
  }

  for (unsigned short i = 0; i < param.nbShip; ++i) {
    param.shipSizeArray[i] = bufferShipSize[i];
  }

  return param;
}

void receiveShips(int *sock, struct sockaddr_in *server, unsigned short nbShip,
                  struct Ship shipArray[]) {
  char shipBuffer[nbShip * 5];
  int errorCode = read(*sock, shipBuffer, sizeof(shipBuffer));

  if (errorCode < 0) {
    char ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(server->sin_addr), ip, INET_ADDRSTRLEN);
    printf("read error on the socket(%s:%d): read=%d: %s\n", ip,
           ntohs(server->sin_port), errorCode, strerror(errno));
    exit(errno);
  }

  for (unsigned short i = 0; i < nbShip; ++i) {
    shipArray[i].id = (short)shipBuffer[(i * 5)];
    shipArray[i].size = (short)shipBuffer[(i * 5) + 1];
    shipArray[i].placement.direction = shipBuffer[(i * 5) + 2];
    shipArray[i].placement.coordinates.x = (short)shipBuffer[(i * 5) + 3];
    shipArray[i].placement.coordinates.y = (short)shipBuffer[(i * 5) + 4];
  }
}
