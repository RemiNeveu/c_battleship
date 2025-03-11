# Battleship Game
This project is an implementation of the classic Battleship game in C, featuring a client-server architecture. The server waits for two clients before launching their game in a separate process, then continues accepting new connections for the next games. The size of the grids, the number and the size of the ships can be modified.

## Table of Contents
- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Game Parameters Configuration](#game-parameters-configuration)
- [Building the Project](#building-the-project)
- [Installing the Project](#install-the-project)
- [Running the Game](#running-the-game)
- [Container](#container)


## Project Structure
The project is organized into two main directories: client and server.

- client/
    - makefile
    - src/
    - out/
    - build/
  
- server/
    - makefile
    - src/
    - out/
    - build/

- containers
    - battleship-server.dockerfile

## Prerequisites
- Unix OS (maybe, but only tested on Linux)
- C compiler (only tested on GCC, the makefile is set to use gcc)
- Make
- C Standard Library (only tested with glibc)

## Game Parameters Configuration
The game settings, such as the number of ships, grid size, and ship sizes, are defined in the param.h and param.c files on the server side.

- param.h:
    - NB_SHIP: Number of ships in the game (must be less than 128).
    - GRID_SIZE: Size of the game grid (must be less than 128).
    - SHIP_SIZE_ARRAY: Declared as an external array, its values are defined in param.c.

- param.c:
    - SHIP_SIZE_ARRAY[NB_SHIP]: The array describing the size of each ship.

## Building the Project
You have to build the server and the client separatly:

### Build the Client
```bash
cd client/
make build
```
This will compile the client source code and place the resulting binary in the client/build/ directory.

### Build the Server
```bash
cd server/
make build
```
This will compile the server source code and place the resulting binary in the server/build/ directory.

## Install the Project
You can also add the target executable in your /usr/local/bin/ folder, as for building, you have to do it for the client and the server separatly.

### Install the Client
```bash
cd client/
sudo make install
```
The current user will be able to run the battleship-client command

### Uninstall the Client
```bash
cd client/
sudo make install
```

### Install the Server
```bash
cd server/
sudo make install
```
The current user will be able to run the battleship-server command

### Uninstall the Server
```bash
cd server/
sudo make uninstall
```

## Running the Game
To run the game, follow these steps:

### 1. Start the Server:
- Run the server binary with port argument:

```bash
battleship-server <server_port>
```
- The server will start and listen for incoming client connections.

### 2. Start the Client:
- Run the client binary with the server's IP address and port number as arguments:
```bash
battleship-client <server_ip> <server_port>
```


You have to connect two clients to the server in order to start a game session. The server will listen for other connections while handling currently running games.

## Container
It's possible to run the server side in a container.  
-> tested with docker and podman

### Run the container from the hosted image
- On the default port.
    ```bash
    docker run --name battleship-server -p 50000:50000 ghcr.io/remineveu/c_battleship_server:latest
    ```
- On a custom port (exemple: 40000).
    ```bash
    docker run --name battleship-server -p 40000:40000 -e SERVER_PORT=40000 ghcr.io/remineveu/c_battleship_server:latest
    ```

### build and launch from the dockerfile
- Build the image.
    ```bash
    docker build -f battleship-server.dockerfile -t battleship-server-image .
    ```

- Run the container on the default port (50000).
    ```bash
    docker run --name battleship-server -p 50000:50000 battleship-server-image
    ```

- Run the container on a custom port (exemple: 40000).
    ```bash
    docker run --name battleship-server -p 40000:40000 -e SERVER_PORT=40000 battleship-server-image
    ```
