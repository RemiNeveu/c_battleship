/**
 * @file param.c
 * @brief Implementation of game settings definitions.
 */

#include "param.h"

/**
 * @brief An array containing the sizes of the ships.
 *
 * This array defines the sizes of the ships in the game. The values can be
 * modified to change the game's ship configurations. The size of a ship should
 * be less than 128.
 */
const unsigned short SHIP_SIZE_ARRAY[NB_SHIP] = {5, 4, 3, 3, 2};
