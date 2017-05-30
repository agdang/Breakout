#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <ctime>
#include <cmath>
#include <stdlib.h>
#include <vector>

#define BLOCK_WIDTH 64
#define BLOCK_HEIGHT 16

struct block
{
  int x;
  int y;

  SDL_Color col;
  SDL_Rect rect;

  bool destroyed;

  block(int x, int y);
  ~block();

  void Show(SDL_Renderer* ren);
  void Destroy();
  void Reset();
};

block::block(int x, int y)
{
  this->x = x;
  this->y = y;

  col.r = rand() % 255 + 1;
  col.g = rand() % 255 + 1;
  col.b = rand() % 255 + 1;
  col.a = 255;

  destroyed = false;

  rect = { x * BLOCK_WIDTH, y * BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT };
}

block::~block() {}

void block::Show(SDL_Renderer* ren)
{
  SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, col.a);
  SDL_RenderFillRect(ren, &rect);
}

void block::Destroy() { destroyed = true; }
void block::Reset() { destroyed = false; }

int main(int argc, char* argv[])
{
  srand(time(NULL));

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* window = SDL_CreateWindow("Breakout", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

  bool running = true;

  int input[2];

  SDL_Rect player = { 320-32, 480-64, 80, 16 };
  SDL_Rect ball = { 320, 200, 8, 8 };

  int ballXV = 1;
  int ballYV = 3;

  std::vector<block*> blocks;

  for (int i = 0; i < 16; i++)
  {
    for (int j = 0; j < 8; j++)
      blocks.push_back(new block(i, j));
  }

  while(running)
  {
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
      switch(event.type)
      {
        case SDL_QUIT:
          running = false;
          break;
        case SDL_KEYDOWN:
          switch(event.key.keysym.sym)
          {
            case SDLK_LEFT:
              input[0] = 1;
              break;
            case SDLK_RIGHT:
              input[1] = 1;
              break;
          }
          break;
          case SDL_KEYUP:
            switch(event.key.keysym.sym)
            {
              case SDLK_LEFT:
                input[0] = 0;
                break;
              case SDLK_RIGHT:
                input[1] = 0;
                break;
            }
            break;
      }
    }

    if (input[0] == 1) player.x -= 10;
    if (input[1] == 1) player.x += 10;

    if (player.x < 0) player.x = 0;
    if (player.x + player.w > 640) player.x = 640 - player.w;

    ball.x += ballXV;
    ball.y += ballYV;

    if (ball.x < 0 || ball.x + ball.w > 640) ballXV = -ballXV;
    if (ball.y < 0 || ball.y + ball.h > 480) ballYV = -ballYV;

    if (ball.x >= player.x && ball.x + ball.w <= player.x + player.w
    && ball.y >= player.y && ball.y + ball.h <= player.y + player.h)
      ballYV = -ballYV;

    for (auto it : blocks)
    {
      if (ball.x >= it->rect.x && ball.x + ball.w <= it->rect.x + it->rect.w
      && ball.y >= it->rect.y && ball.y + ball.h <= it->rect.y + it->rect.h)
      {
        if (!it->destroyed)
        {
          ballYV = -ballYV;
          it->Destroy();
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (auto it : blocks)
    {
      if (!it->destroyed)
        it->Show(renderer);

    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &player);

    SDL_RenderFillRect(renderer, &ball);

    SDL_RenderPresent(renderer);

    SDL_Delay(10);
  }

  for (auto it : blocks)
    delete it;

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0;
}
