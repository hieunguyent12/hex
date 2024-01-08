#include <unordered_map>
#include "map.h"

std::unordered_map<size_t, Tile> Map::map;

void Map::createMap(int top, int bottom, int left, int right)
{
  for (int r = top; r <= bottom; r++)
  {                                // pointy top
    int r_offset = floor(r / 2.0); // or r>>1
    for (int q = left - r_offset; q <= right - r_offset; q++)
    {
      // does this make a copy of Tile or will it move the Tile to the map?
      Tile t(CubeCoords(q, r));
      map[getTileId(t)] = t;
    }
  }
}

std::unordered_map<size_t, Tile> &Map::getMap()
{
  return map;
}

Tile *Map::getTile(size_t id)
{
  return map.contains(id) ? &map[id] : nullptr;
}

size_t Map::getTileId(Tile &tile)
{
  std::hash<int> int_hash;
  size_t hq = int_hash(tile.cubeCoords.q);
  size_t hr = int_hash(tile.cubeCoords.r);
  return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
}

size_t Map::getTileId(int q, int r)
{
  std::hash<int> int_hash;
  size_t hq = int_hash(q);
  size_t hr = int_hash(r);
  return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
}

size_t Map::getTileId(CubeCoords &cc)
{
  std::hash<int> int_hash;
  size_t hq = int_hash(cc.q);
  size_t hr = int_hash(cc.r);
  return hq ^ (hr + 0x9e3779b9 + (hq << 6) + (hq >> 2));
}