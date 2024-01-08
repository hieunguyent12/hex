#pragma once
#include <unordered_map>
#include "tile.h"

// Singleton Map class to store the map
class Map
{
private:
  static std::unordered_map<size_t, Tile> map;

public:
  Map() = delete; // disallow creating instances of this class because it is a singleton

  static void createMap(int top, int bottom, int left, int right);
  static std::unordered_map<size_t, Tile> &getMap(); // remove this later
  static Tile *getTile(size_t id);

  static size_t getTileId(Tile &tile);
  static size_t getTileId(CubeCoords &cc);
  static size_t getTileId(int q, int r);
};