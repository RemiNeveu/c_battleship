#ifndef PARAM_H
#define PARAM_H

/**
 * @file param.h
 * @brief Game settings definitions.
 */

/**
 * @brief The number of ships in the game.
 *
 * This value should be less than 128.
 */
#define NB_SHIP 5

/**
 * @brief The size of the game grid.
 *
 * This value should be less than 128.
 */
#define GRID_SIZE 10

/**
 * @brief An array containing the sizes of the ships.
 *
 * This array is defined in param.c and should be modified there.
 */
extern const unsigned short SHIP_SIZE_ARRAY[];

#endif // PARAM_H
