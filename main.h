#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

// TODO: Create any necessary structs

/*
* For example, for a Snake game, one could be:
*
* struct snake {
*   int heading;
*   int length;
*   int row;
*   int col;
* };
*
* Example of a struct to hold state machine data:
*
* struct state {
*   int currentState;
*   int nextState;
* };
*
*/

const int NUM_PROJECTILES = 3;
const int NUM_ENEMIES = 5;
const int NUM_STARS = 30;

const int ENEMY_START_Y = -20;

const int ENEMY_DAMAGE = 5;
const int ENEMY_HEALTH = 50;
const int PLAYER_DAMAGE = 50;
const int PLAYER_HEALTH = 100;

const int ENEMY_DIE_TIME = 10;
const int LOSING_TIME = 4;
const int WIN_TIME = 4;

const int SCORE_THRESHOLD = 10;

struct star
{
    int x;
    int y;
    int size;
    int speed;
    u16 color;
};

struct player
{
    int x;
    int y;
    int health;
    int speed;
    int score;
    enum state
    {
        DEFAULT,
        LEFT,
        RIGHT,
    }player_state;
};

struct player_projectile
{
    int x;
    int y;
    int speed;
    int fired;
    int damage;
};

struct enemy
{
    int x;
    int y;
    int health;
    int speed;
    int active;
    int died;
};



#endif
