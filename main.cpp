// https://www.redblobgames.com/grids/hexagons/implementation.html#cpp
#include "raylib.h"
#include "math.h"
#include <iostream>
#include <array>
#include <unordered_set>
#include <unordered_map>

template <typename T, std::size_t Row, std::size_t Col>
using Array2D = std::array<std::array<T, Col>, Row>;

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

struct CubeCoords
{
  int q;
  int r;
  int s;

  // default constructor
  CubeCoords()
  {
    q = 0;
    r = 0;
    s = 0;
  }

  CubeCoords(int _q, int _r) : q(_q), r(_r), s(-_q - _r)
  {
    assert(q + r + s == 0);
  }
};

struct Tile
{
  CubeCoords cubeCoords;
  bool isWall;
  bool isTarget;

  bool operator==(const Tile &b) const
  {
    return (cubeCoords.q == b.cubeCoords.q &&
            cubeCoords.r == b.cubeCoords.r &&
            cubeCoords.s == b.cubeCoords.s);
  }
};

namespace std
{
  template <>
  struct hash<Tile>
  {
    size_t operator()(const Tile &h) const
    {
      hash<int> int_hash;
      size_t hq = int_hash(h.cubeCoords.q);
      size_t hr = int_hash(h.cubeCoords.r);
      return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
    }
  };
}

size_t myhash(const Tile &t)
{
  std::hash<int> int_hash;
  size_t hq = int_hash(t.cubeCoords.q);
  size_t hr = int_hash(t.cubeCoords.r);
  return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
}

size_t myhash(int q, int r)
{
  std::hash<int> int_hash;
  size_t hq = int_hash(q);
  size_t hr = int_hash(r);
  return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
}

void cubeToOffset()
{
}

CubeCoords offsetToCube(int row, int col)
{
  int q = col - int((row - (row & 1)) / 2);
  int r = row;

  return CubeCoords(q, r);
}

// The article says that the top, bottom, left, and right arguments are "offset coordinates"
// but im unsure of what that means
std::unordered_map<size_t, Tile> createMap(int top, int bottom, int left, int right)
{
  std::unordered_map<size_t, Tile> map;
  for (int r = top; r <= bottom; r++)
  {                                // pointy top
    int r_offset = floor(r / 2.0); // or r>>1
    for (int q = left - r_offset; q <= right - r_offset; q++)
    {
      Tile t;
      t.isTarget = false;
      t.isWall = false;
      t.cubeCoords = CubeCoords(q, r);
      map[myhash(t)] = t;
    }
  }

  return map;
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
  auto map = createMap(-2, 1, -3, 3);

  const Orientation pointy_orientation = Orientation(sqrt(3.0), sqrt(3.0) / 2.0, 0.0, 3.0 / 2.0,
                                                     sqrt(3.0) / 3.0, -1.0 / 3.0, 0.0, 2.0 / 3.0,
                                                     0.5);
  const Layout pointy_layout = Layout(pointy_orientation, {radius, radius}, {offset_x, offset_y});
  const Orientation &M = pointy_layout.orientation;

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

      auto key = myhash(c_q, c_r);

      if (map.count(key) > 0)
      {
        map[key].isWall = true;
      }
    }

    for (const auto &[_, tile] : map)
    {
      float hex_x = (M.f0 * tile.cubeCoords.q + M.f1 * tile.cubeCoords.r) * pointy_layout.size.x;
      float hex_y = (M.f2 * tile.cubeCoords.q + M.f3 * tile.cubeCoords.r) * pointy_layout.size.y;

      if (tile.isWall)
      {

        DrawPoly((Vector2){hex_x + pointy_layout.origin.x, hex_y + pointy_layout.origin.y}, 6, 25, 30, RED);
      }
      else
      {
        DrawPolyLines((Vector2){hex_x + pointy_layout.origin.x, hex_y + pointy_layout.origin.y}, 6, 25, 30, BLUE);
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