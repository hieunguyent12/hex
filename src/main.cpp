// https://www.redblobgames.com/grids/hexagons/implementation.html#cpp
#include "raylib.h"
#include "math.h"
#include <iostream>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "map.h"

struct Orientation
{
  const double f0, f1, f2, f3;
  const double b0, b1, b2, b3;
  const double start_angle; // in multiples of 60°
  Orientation(double f0_, double f1_, double f2_, double f3_,
              double b0_, double b1_, double b2_, double b3_,
              double start_angle_)
      : f0(f0_), f1(f1_), f2(f2_), f3(f3_),
        b0(b0_), b1(b1_), b2(b2_), b3(b3_),
        start_angle(start_angle_) {}
};

struct Layout
{
  Orientation orientation;
  Vector2 size;
  Vector2 origin;
  Layout(Orientation orientation_, Vector2 size_, Vector2 origin_)
      : orientation(orientation_), size(size_), origin(origin_) {}
};

// namespace std
// {
//   template <>
//   struct hash<Tile>
//   {
//     size_t operator()(const Tile &h) const
//     {
//       hash<int> int_hash;
//       size_t hq = int_hash(h.cubeCoords.q);
//       size_t hr = int_hash(h.cubeCoords.r);
//       return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
//     }
//   };
// }

CubeCoords offsetToCube(int row, int col)
{
  int q = col - int((row - (row & 1)) / 2);
  int r = row;

  return CubeCoords(q, r);
}

CubeCoords round_fractional_hex(double q, double r)
{
  int c_q = int(round(q));
  int c_r = int(round(r));
  int s = int(round(-q - r));
  double q_diff = abs(c_q - q);
  double r_diff = abs(c_r - r);
  double s_diff = abs(s - (-q - r));
  if ((q_diff > r_diff) && (q_diff > s_diff))
  {
    c_q = -c_r - s;
  }
  else if (r_diff > s_diff)
  {
    c_r = -c_q - s;
  }

  return CubeCoords(c_q, c_r);
}

int main(void)
{
  const int screenWidth = 800;
  const int screenHeight = 450;

  const int width = 8;
  const int height = 8;
  const int radius = 25;
  const int offset_x = 200;
  const int offset_y = 100;
  bool searching = false;
  bool foundTarget = false;
  Map::createMap(0, 4, 0, 6);

  CubeCoords playerCoords(0, 0);
  auto player = Map::getTile(Map::getTileId(playerCoords));
  player->isPlayer = true;

  CubeCoords targetCoords(4, 4);
  auto target = Map::getTile(Map::getTileId(targetCoords));
  target->isTarget = true;

  const Orientation pointy_orientation = Orientation(sqrt(3.0), sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0,
                                                     sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0,
                                                     0.5);
  const Layout pointy_layout = Layout(pointy_orientation, {radius, radius}, {offset_x, offset_y});
  const Orientation &M = pointy_layout.orientation;

  std::vector<Tile *> queue{};
  std::unordered_set<Tile *> reached{};
  std::unordered_map<size_t, Tile *> came_from{};

  queue.push_back(player);
  reached.emplace(player);
  came_from[Map::getTileId(playerCoords)] = nullptr;

  InitWindow(screenWidth, screenHeight, "algorithm visualizer");
  double tick_time = 0.05;
  double time = GetTime();
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (IsKeyPressed(KEY_ENTER))
    {
      if (!searching)
      {
        searching = true;
      }
      else
      {
        searching = false;
      }
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
      Vector2 mousePos = GetMousePosition();
      double q = (sqrt(3) / 3 * (mousePos.x - offset_x) - (1.0 / 3.0 * (mousePos.y - offset_y))) / radius;
      double r = (2.0 / 3.0) * (mousePos.y - offset_y) / radius;

      auto cubeCoords = round_fractional_hex(q, r);
      auto tile = Map::getTile(Map::getTileId(cubeCoords));

      if (tile)
      {
        tile->isWall = true;
      }
    }

    for (const auto &[_, tile] : Map::getMap())
    {
      float hex_x = (M.f0 * tile.cubeCoords.q + M.f1 * tile.cubeCoords.r) * pointy_layout.size.x;
      float hex_y = (M.f2 * tile.cubeCoords.q + M.f3 * tile.cubeCoords.r) * pointy_layout.size.y;

      if (tile.isWall)
      {
        DrawPoly((Vector2){hex_x + pointy_layout.origin.x, hex_y + pointy_layout.origin.y}, 6, 25, 30, RED);
      }
      else if (tile.isPlayer)
      {
        DrawPoly((Vector2){hex_x + pointy_layout.origin.x, hex_y + pointy_layout.origin.y}, 6, 25, 30, GREEN);
      }
      else if (tile.isTarget)
      {
        DrawPoly((Vector2){hex_x + pointy_layout.origin.x, hex_y + pointy_layout.origin.y}, 6, 25, 30, PURPLE);
      }
      else if (tile.isPath)
      {
        DrawPoly((Vector2){hex_x + pointy_layout.origin.x, hex_y + pointy_layout.origin.y}, 6, 25, 30, LIME);
      }
      else if (tile.reached)
      {
        DrawPoly((Vector2){hex_x + pointy_layout.origin.x, hex_y + pointy_layout.origin.y}, 6, 25, 30, ORANGE);
      }
      else
      {
        DrawPolyLines((Vector2){hex_x + pointy_layout.origin.x, hex_y + pointy_layout.origin.y}, 6, 25, 30, BLACK);
      }
    }

    if (searching)
    {
      // only update the queue for new nodes to search only at certain interval so it doesn't go too fast.
      if (GetTime() >= time + tick_time)
      {
        time = GetTime();

        if (!queue.empty())
        {
          auto current = queue.front();
          for (const auto tile : current->getNeighbors())
          {
            auto id = Map::getTileId(tile->cubeCoords);
            if (came_from.count(id) <= 0 && !tile->isWall)
            {
              queue.push_back(tile);
              reached.insert(tile);
              tile->reached = true;
              came_from[id] = current;
            }
          }
          queue.erase(queue.begin());
        }
        else
        {
          if (!foundTarget)
          {
            std::vector<Tile *> path{};

            size_t current = Map::getTileId(targetCoords); // start from the target tile and go backwards from there
            while (current != Map::getTileId(playerCoords))
            {
              path.push_back(came_from[current]);
              current = Map::getTileId(came_from[current]->cubeCoords);
            }
            foundTarget = true;

            if (foundTarget)
            {
              for (const auto t : path)
              {
                t->isPath = true;
              }
            }
          }
        }
      }
      for (const auto tile : queue)
      {
        float hex_x = (M.f0 * tile->cubeCoords.q + M.f1 * tile->cubeCoords.r) * pointy_layout.size.x;
        float hex_y = (M.f2 * tile->cubeCoords.q + M.f3 * tile->cubeCoords.r) * pointy_layout.size.y;
        DrawPoly((Vector2){hex_x + pointy_layout.origin.x, hex_y + pointy_layout.origin.y}, 6, 25, 30, BLUE);
      }
    }
    EndDrawing();
  }

  //----------------------------------------------------------------------------------
  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}