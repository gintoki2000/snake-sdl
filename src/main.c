#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h> //for bool type
#include <stdlib.h> //for rand()
#define BLOCK_SIZE          30
#define WINDOW_WIDTH        630
#define WINDOW_HEIGHT       480
#define DELAY_TICKS         800
#define MAX_LENGTH          100
#define GAME_BOARD_WIDTH    31
#define GAME_BOARD_HEIGHT   16

SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;
bool g_is_running = true;

const int DX[4] = {0, 0, -1, 1};
const int DY[4] = {-1, 1, 0, 0};

//const int x[2][3] = {{1, 3, 4}, {2, 5, 6}};

typedef enum {
  eUp,
  eDown,
  eLeft,
  eRight
} Direction;

struct _Snake
{
  int x[MAX_LENGTH];
  int y[MAX_LENGTH];
  int length;
  Direction direction;
} g_snake;

struct _Food
{
  int x;
  int y;
} g_food;

//init and quit sdl
int init();
void quit();
void loop();

void gameReset();
void gameTick();
void gamePaint();
void gameInput();


int main(int argc, char** argv)
{
  init ();
  loop ();
  quit ();
}

int init()
{
  if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
      printf("Could not init sdl: %s\n", SDL_GetError ());
      return -1;
    }
  g_window = SDL_CreateWindow ("snake-sdl", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN
            );
  if(!g_window)
    {
      printf("Could not create window: %s\n", SDL_GetError ());
      return -1;
    }
  g_renderer = SDL_CreateRenderer (g_window, -1, SDL_RENDERER_ACCELERATED);
  if(!g_renderer)
    {
      printf("Could not create renderer: %s\n", SDL_GetError ());
      return -1;
    }
  srand (SDL_GetTicks ());
  return 0;
}
void quit()
{
  SDL_DestroyRenderer (g_renderer);
  SDL_DestroyWindow (g_window);
  SDL_Quit ();
}

void loop()
{
  unsigned int start_ticks;
  unsigned int used_ticks;
  gameReset();
  while(g_is_running)
    {
      start_ticks = SDL_GetTicks ();
      gameInput ();
      gameTick ();
      SDL_SetRenderDrawColor (g_renderer, 255, 255, 255, 255);
      SDL_RenderClear (g_renderer);
      gamePaint ();
      SDL_RenderPresent (g_renderer);
      used_ticks = SDL_GetTicks () - start_ticks;
      //printf("used ticks = %d\n", used_ticks);
      if(used_ticks < DELAY_TICKS)
        {
          SDL_Delay (DELAY_TICKS - used_ticks);
        }
    }
}

//-----------------------------------------------//
void gameReset()
{
  g_snake.direction = eDown;
  g_snake.x[0] = 0; g_snake.y[0] = 2;
  g_snake.x[1] = 0; g_snake.y[1] = 1;
  g_snake.x[2] = 0; g_snake.y[2] = 0;
  g_snake.length = 3;

  g_food.x = rand () % GAME_BOARD_WIDTH;
  g_food.y = rand () % GAME_BOARD_HEIGHT;
}

void gameTick()
{
  //move snake
  for(int i = g_snake.length - 1; i > 0; --i)
    {
      g_snake.x[i] = g_snake.x[i - 1];
      g_snake.y[i] = g_snake.y[i - 1];
    }
  g_snake.x[0] += DX[g_snake.direction];
  g_snake.y[0] += DY[g_snake.direction];

  //check is snake out of game board
  if(g_snake.x[0] < 0
     || g_snake.y[0] < 0
     || g_snake.x[0] >= GAME_BOARD_WIDTH
     || g_snake.y[0] >= GAME_BOARD_HEIGHT)
    {
      gameReset ();
    }
  //check is snake collided with itself
  int head_x = g_snake.x[0];
  int head_y = g_snake.y[0];
  for(int i = 1; i < g_snake.length; ++i)
    {
      if(head_x == g_snake.x[i] && head_y == g_snake.y[i])
        {
          gameReset ();
        }
    }
  //check is snake collided with food
  if(head_x == g_food.x && head_y == g_food.y)
    {
      //add new dot
      int tail_dx = g_snake.x[g_snake.length - 1] - g_snake.x[g_snake.length - 2];
      int tail_dy = g_snake.y[g_snake.length - 1] - g_snake.y[g_snake.length - 2];

      g_snake.length++;
      g_snake.x[g_snake.length - 1] = g_snake.x[g_snake.length - 2] + tail_dx;
      g_snake.y[g_snake.length - 1] = g_snake.y[g_snake.length - 2] + tail_dy;

      //set new food position
      g_food.x = rand () % GAME_BOARD_WIDTH;
      g_food.y = rand () % GAME_BOARD_HEIGHT;
    }
}

void gamePaint()
{
  static SDL_Rect temp;

  //paint snake
  SDL_SetRenderDrawColor (g_renderer, 0, 0, 0, 0);
  for(int i =0; i < g_snake.length; ++i)
    {
      temp.x = g_snake.x[i] * BLOCK_SIZE;
      temp.y = g_snake.y[i] * BLOCK_SIZE;
      temp.w = BLOCK_SIZE;
      temp.h = BLOCK_SIZE;
      SDL_RenderFillRect (g_renderer, &temp);

    }
  //paint food
  SDL_SetRenderDrawColor (g_renderer, 200, 0, 0, 200);
  temp.x = g_food.x * BLOCK_SIZE;
  temp.y = g_food.y * BLOCK_SIZE;
  SDL_RenderFillRect (g_renderer, &temp);

}

void gameInput()
{
  /*const unsigned char* key_states =  SDL_GetKeyboardState (NULL);

  if(key_states[SDL_SCANCODE_W] && g_snake.direction != eDown)
    {
      g_snake.direction = eUp;
    }
  else if(key_states[SDL_SCANCODE_S] && g_snake.direction != eUp)
    {
      g_snake.direction = eDown;
    }
  else if(key_states[SDL_SCANCODE_A] && g_snake.direction != eRight)
    {
      g_snake.direction = eLeft;
    }
  else if(key_states[SDL_SCANCODE_D] && g_snake.direction != eLeft)
    {
      g_snake.direction = eRight;
    }
   */

  static SDL_Event event;
  while(SDL_PollEvent (&event))
    {
      switch (event.type)
        {
        case SDL_QUIT:
          g_is_running = false;
          break;
        case SDL_KEYDOWN:
          if(event.key.keysym.sym == SDLK_UP && g_snake.direction != eDown)
            {
              g_snake.direction = eUp;
            }
          else if(event.key.keysym.sym == SDLK_DOWN && g_snake.direction != eUp)
            {
              g_snake.direction = eDown;
            }
          else if(event.key.keysym.sym == SDLK_LEFT && g_snake.direction != eRight)
            {
              g_snake.direction = eLeft;
            }
          else if(event.key.keysym.sym == SDLK_RIGHT && g_snake.direction != eLeft)
            {
              g_snake.direction = eRight;
            }
          break;
        }
    }
}
