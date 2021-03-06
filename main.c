#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"
/* TODO: */
// Include any header files for title screen or exit
// screen images generated by nin10kit. Example for the provided garbage
// image:
// #include "images/garbage.h"
#include "images/title.h"
#include "images/player_sprite.h"
#include "images/player_sprite_left.h"
#include "images/player_sprite_right.h"
#include "images/enemy_sprite.h"
#include "images/win.h"
#include "images/lose.h"
#include "images/explosion.h"

#define IN_BOX(input_x, input_y, x, y, width, height, buffer) (input_x >= (x - buffer) && (input_x <= (x + width + buffer)) && (input_y >= (y - buffer)) && (input_y <= (y + height + buffer)))

/* TODO: */
// Add any additional states you need for your app. You are not requried to use
// these specific provided states.
enum gba_state
{
  START,
  PLAY,
  WIN,
  LOSE,
};

int main(void)
{
  /* TODO: */
  // Manipulate REG_DISPCNT here to set Mode 3. //
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial application state
  enum gba_state state = START;

  // Initialize stars
  struct star stars[NUM_STARS];

  // init player
  struct player player;

  struct player_projectile player_projectile[NUM_PROJECTILES];

  // init enemies
  struct enemy enemies[NUM_ENEMIES];

  char healthString[12];
  char scoreString[12];

  int losingAnim = -1;
  int winningAnim = -1;

  // init enemies
  for (int i = 0; i < NUM_ENEMIES; i++)
  {
    enemies[i].x = randint(0, WIDTH - (ENEMY_SPRITE_WIDTH));
    enemies[i].y = ENEMY_START_Y - (4 * i);
    enemies[i].health = ENEMY_HEALTH;
    enemies[i].speed = 1;
    enemies[i].died = -1;
  }

  // init player
  player.x = (WIDTH / 2) - (PLAYER_SPRITE_WIDTH / 2);
  player.y = (HEIGHT / 2) - (PLAYER_SPRITE_HEIGHT / 2);
  player.health = PLAYER_HEALTH;
  player.speed = 2;
  player.score = 0;
  player.player_state = DEFAULT;

  // init projectiles
  for (int i = 0; i < NUM_PROJECTILES; i++)
  {
    player_projectile[i].x = player.x;
    player_projectile[i].y = player.y;
    player_projectile[i].speed = 4;
    player_projectile[i].fired = 0;
    player_projectile[i].damage = PLAYER_DAMAGE;
  }

  // init start
  for (int i = 0; i < NUM_STARS; i++)
  {
    stars[i].x = randint(0, WIDTH);
    stars[i].y = randint(0, HEIGHT);
    stars[i].size = 2;
    stars[i].speed = randint(1, 3);
    if (randint(0, 2) == 0)
    {
      stars[i].color = WHITE;
    }
    else
    {
      stars[i].color = BLUE;
    }
  }

  while (1)
  {
    currentButtons = BUTTONS; // Load the current state of the buttons

    /* TODO: */
    // Manipulate the state machine below as needed //
    // NOTE: Call waitForVBlank() before you draw

    if (state == START || KEY_DOWN(BUTTON_SELECT, currentButtons))
    {
      // reset game
      losingAnim = -1;
      winningAnim = -1;

      // init enemies
      for (int i = 0; i < NUM_ENEMIES; i++)
      {
        enemies[i].x = randint(0, WIDTH - (ENEMY_SPRITE_WIDTH));
        enemies[i].y = ENEMY_START_Y - (4 * i);
        enemies[i].health = ENEMY_HEALTH;
        enemies[i].speed = 1;
        enemies[i].died = -1;
      }

      // init player
      player.x = (WIDTH / 2) - (PLAYER_SPRITE_WIDTH / 2);
      player.y = (HEIGHT / 2) - (PLAYER_SPRITE_HEIGHT / 2);
      player.health = PLAYER_HEALTH;
      player.speed = 2;
      player.score = 0;
      player.player_state = DEFAULT;

      // init projectiles
      for (int i = 0; i < NUM_PROJECTILES; i++)
      {
        player_projectile[i].x = player.x;
        player_projectile[i].y = player.y;
        player_projectile[i].speed = 4;
        player_projectile[i].fired = 0;
        player_projectile[i].damage = PLAYER_DAMAGE;
      }

      // init start
      for (int i = 0; i < NUM_STARS; i++)
      {
        stars[i].x = randint(0, WIDTH);
        stars[i].y = randint(0, HEIGHT);
        stars[i].size = 2;
        stars[i].speed = randint(1, 3);
        if (randint(0, 2) == 0)
        {
          stars[i].color = WHITE;
        }
        else
        {
          stars[i].color = BLUE;
        }
      }

      state = START;
    }

    switch (state)
    {
    case START:

      // Logic
      if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons))
      {
        // Switch state when player presses start
        state = PLAY;
      }

      // Draw
      waitForVBlank();
      drawFullScreenImageDMA(title);

      drawCenteredString(0, 0, WIDTH, HEIGHT + 60, "Press START", WHITE);

      // state = ?
      break;
    case PLAY:

      // Logic

      // check player state
      if (player.health <= 0)
      {
        losingAnim = LOSING_TIME;
        state = LOSE;
      }
      if (player.score >= SCORE_THRESHOLD)
      {
        winningAnim = WIN_TIME;
        state = WIN;
      }

      if (~KEY_DOWN(BUTTON_LEFT, currentButtons) && ~(KEY_DOWN(BUTTON_RIGHT, currentButtons)))
      {
        player.player_state = DEFAULT;
      }

      // update player movement x
      if (player.x >= 0 && player.x <= (WIDTH - PLAYER_SPRITE_WIDTH))
      {
        if (KEY_DOWN(BUTTON_LEFT, currentButtons))
        {
          player.x += -1 * player.speed;
          player.player_state = LEFT;
        }
        if (KEY_DOWN(BUTTON_RIGHT, currentButtons))
        {
          player.x += player.speed;
          player.player_state = RIGHT;
        }
      }
      else
      {
        if (player.x < 0)
        {
          player.x = 0;
        }
        if (player.x > (WIDTH - PLAYER_SPRITE_WIDTH))
        {
          player.x = (WIDTH - PLAYER_SPRITE_WIDTH);
        }
      }

      // update player movement y
      if (player.y >= 0 && player.y <= (HEIGHT - PLAYER_SPRITE_HEIGHT))
      {
        if (KEY_DOWN(BUTTON_UP, currentButtons))
        {
          player.y += -1 * player.speed;
        }
        if (KEY_DOWN(BUTTON_DOWN, currentButtons))
        {
          player.y += player.speed;
        }
      }
      else
      {
        if (player.y < 0)
        {
          player.y = 0;
        }
        if (player.y > (HEIGHT - PLAYER_SPRITE_HEIGHT))
        {
          player.y = (HEIGHT - PLAYER_SPRITE_HEIGHT);
        }
      }

      // Fire
      if (KEY_JUST_PRESSED(BUTTON_A, currentButtons, previousButtons))
      {
        for (int i = 0; i < NUM_PROJECTILES; i++)
        {
          if (player_projectile[i].fired == 0)
          {
            player_projectile[i].fired = 1;
            break;
          }
        }
      }

      // projectile logic
      for (int i = 0; i < NUM_PROJECTILES; i++)
      {
        if (player_projectile[i].fired == 1)
        {
          player_projectile[i].y += -1 * player_projectile[i].speed;
          if (player_projectile[i].y < 0)
          {
            player_projectile[i].fired = 0;
          }
        }
        else
        {
          player_projectile[i].x = player.x + (PLAYER_SPRITE_WIDTH / 2) - 1;
          player_projectile[i].y = player.y + (PLAYER_SPRITE_HEIGHT / 2);
        }
      }

      // enemy logic
      for (int i = 0; i < NUM_ENEMIES; i++)
      {
        if (enemies[i].died == -1)
        {
          enemies[i].y += enemies[i].speed;

          // collision with projectile
          for (int j = 0; j < NUM_PROJECTILES; j++)
          {
            if (player_projectile[j].fired == 1 && IN_BOX(player_projectile[j].x, player_projectile[j].y, enemies[i].x, enemies[i].y, ENEMY_SPRITE_WIDTH, ENEMY_SPRITE_HEIGHT, 0))
            {
              enemies[i].health -= PLAYER_DAMAGE;
              player_projectile[j].fired = 0;
              player.score++;
            }
          }

          // collision with player
          if (IN_BOX(enemies[i].x, enemies[i].y, player.x, player.y, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, 0))
          {
            enemies[i].health = 0;
            player.health -= ENEMY_DAMAGE;
            fillScreenDMA(RED);
          }

          // collision with border
          if (enemies[i].y > HEIGHT)
          {
            // die
            enemies[i].health = 0;
            player.health -= ENEMY_DAMAGE;
            fillScreenDMA(RED);
          }

          // death
          if (enemies[i].health <= 0)
          {
            enemies[i].died = ENEMY_DIE_TIME;
          }
        }
        else if (enemies[i].died >= 0)
        {
          enemies[i].died--;
          if (enemies[i].died == 0)
          {
            enemies[i].health = ENEMY_HEALTH;
            enemies[i].died = -1;
            enemies[i].x = randint(0, WIDTH);
            enemies[i].y = ENEMY_START_Y;
          }
        }
      }

      // Draw
      waitForVBlank();
      fillScreenDMA(BLACK);

      // draw the stars
      for (int i = 0; i < NUM_STARS; i++)
      {
        drawRectDMA(stars[i].y, stars[i].x, stars[i].size, stars[i].size, stars[i].color);
        stars[i].y += stars[i].speed;
        if (stars[i].y > HEIGHT)
        {
          stars[i].y = ENEMY_START_Y;
        }
      }

      // draw projectile
      for (int i = 0; i < NUM_PROJECTILES; i++)
      {
        if (player_projectile[i].fired == 1)
        {
          drawRectDMA(player_projectile[i].y, player_projectile[i].x, 2, 8, RED);
        }
      }

      // draw enemies
      for (int i = 0; i < NUM_ENEMIES; i++)
      {
        if (enemies[i].died == -1)
        {
          drawImageDMA(enemies[i].y, enemies[i].x, ENEMY_SPRITE_WIDTH, ENEMY_SPRITE_HEIGHT, enemy_sprite);
        }
        else if (enemies[i].died >= 0)
        {
          drawImageDMA(enemies[i].y, enemies[i].x, EXPLOSION_WIDTH, EXPLOSION_HEIGHT, explosion);
        }
      }

      // draw player
      switch (player.player_state)
      {
      case DEFAULT:
        drawImageDMA(player.y, player.x, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, player_sprite);
        break;
      case LEFT:
        drawImageDMA(player.y, player.x, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, player_sprite_left);
        break;
      case RIGHT:
        drawImageDMA(player.y, player.x, PLAYER_SPRITE_WIDTH, PLAYER_SPRITE_HEIGHT, player_sprite_right);
        break;
      }

      // draw UI
      snprintf(healthString, 32, "HEALTH: %i", player.health);
      drawString(HEIGHT - 11, 4, healthString, RED);
      drawString(HEIGHT - 12, 4, healthString, WHITE);

      snprintf(scoreString, 32, "SCORE: %02i", player.score);
      drawString(HEIGHT - 11, WIDTH - 58, scoreString, RED);
      drawString(HEIGHT - 12, WIDTH - 58, scoreString, WHITE);

      break;
    case WIN:
      waitForVBlank();
      if (winningAnim > 0)
      {
        drawRectDMA(0, 0, WIDTH, 40 * winningAnim, WHITE);
        winningAnim--;
      }
      else
      {
        drawFullScreenImageDMA(win);
      }
      break;
    case LOSE:
      waitForVBlank();
      if (losingAnim > 0)
      {
        drawRectDMA(0, 0, WIDTH, 40 * losingAnim, RED);
        losingAnim--;
      }
      else
      {
        drawFullScreenImageDMA(lose);
      }
      break;
    }

    previousButtons = currentButtons; // Store the current state of the buttons
  }

  UNUSED(previousButtons); // You can remove this once previousButtons is used

  return 0;
}