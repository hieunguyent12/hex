#include "raylib.h"
#include "math.h"
#include <iostream>
#include <array>

template <typename T, std::size_t Row, std::size_t Col>
using Array2D = std::array<std::array<T, Col>, Row>;

struct CubeCoords
{
  int q;
  int r;
  int s;

  CubeCoords(int _q, int _r) : q(_q), r(_r), s(-_q - _r)
  {
    assert(q + r + s == 0);
  }
};

struct Tile
{
  Vector2 coords;
  bool isWall;
  bool isTarget;
  CubeCoords cubeCoords;
};

void drawHex(const float x, const float y)
{
  DrawPolyLines((Vector2){x, y}, 6, 25, 30, BLUE);
}

template <std::size_t Row, std::size_t Col>
Array2D<Tile, Row, Col> createGrid()
{
  Array2D<Tile, Row, Col> g{};

  for (auto i = 0; i < Row; i++)
  {
    for (auto j = 0; j < Col; j++)
    {
      g[i][j] = {(Vector2){0, 0},
                 false,
                 false};
    }
  }

  return g;
}

int main(void)
{
  const int screenWidth = 800;
  const int screenHeight = 450;

  const int width = 8;
  const int height = 8;
  const int radius = 25;
  const int offset_x = 25;
  const int offset_y = 25;
  // https://www.redblobgames.com/grids/hexagons/
  const float horiz = sqrtf(3) * radius;
  const float vert = (3.0 / 2.0) * radius;
  auto grid = createGrid<width, height>();
  float prevX = 0;
  float prevY = 0;

  InitWindow(screenWidth, screenHeight, "algorithm visualizer");

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
      Vector2 mousePos = GetMousePosition();
      float a_q = (sqrt(3) / 3 * (mousePos.x - offset_x) - (1.0 / 3.0 * (mousePos.y - offset_y))) / radius;
      float a_r = (2.0 / 3.0) * (mousePos.y - offset_y) / radius;

      int c_q = int(round(a_q));
      int c_r = int(round(a_r));
      int s = int(round(-a_q - a_r));
      double q_diff = abs(c_q - a_q);
      double r_diff = abs(c_r - a_r);
      double s_diff = abs(s - (-a_q - a_r));
      if ((q_diff > r_diff) && (q_diff > s_diff))
      {
        c_q = -c_r - s;
      }
      else if (r_diff > s_diff)
      {
        c_r = -c_q - s;
      }
      else
      {
        s = -c_q - c_r;
      }

      int col = c_q + int((c_r - (c_r & 1)) / 2);
      int row = c_r;

      prevX = col;
      prevY = row;

      if (row < height && col < width)
      {
        grid[row][col].isWall = true;
      }
    }
    for (int row = 0; row < height; row++)
    {
      for (int col = 0; col < width; col++)
      {
        Tile tile = grid[row][col];
        float hex_x = row % 2 != 0 ? (col + 0.5) * horiz + offset_x : col * horiz + offset_x;
        float hex_y = row * vert + offset_y;
        tile.coords = {hex_x, hex_y};

        if (tile.isWall)
        {
          DrawPoly((Vector2){hex_x, hex_y}, 6, 25, 30, RED);
        }
        else
        {
          DrawPolyLines((Vector2){hex_x, hex_y}, 6, 25, 30, BLUE);
        }
      }
    }
    DrawText(TextFormat("y: %02.02f", prevY), 200, 250, 20, BLACK);
    DrawText(TextFormat("x: %f", prevX), 200, 200, 20, BLACK);
    // DrawText(TextFormat("Mouse Position: %d %d", GetMouseX(), GetMouseY()), 200, 300, 20, BLACK);
    EndDrawing();
  }

  //----------------------------------------------------------------------------------
  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}