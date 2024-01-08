#include <array>
#include "tile.h"
#include "map.h"

const std::array<CubeCoords, 6> NEIGHBORS_DIRECTIONS{
    CubeCoords(0, -1, 1),
    CubeCoords(1, -1, 0),
    CubeCoords(1, 0, -1),
    CubeCoords(0, 1, -1),
    CubeCoords(-1, 1, 0),
    CubeCoords(-1, 0, 1),
};

// why do we have to define a default constructor?
Tile::Tile(){};

Tile::Tile(CubeCoords cc) : cubeCoords(cc),
                            isWall(false),
                            isTarget(false),
                            isPlayer(false),
                            isPath(false),
                            reached(false)
{
}

Tile::Tile(CubeCoords cc,
           bool _isWall,
           bool _isTarget,
           bool _isPlayer,
           bool _isPath,
           bool _reached) : cubeCoords(cc),
                            isWall(_isWall),
                            isTarget(_isTarget),
                            isPlayer(_isPlayer),
                            isPath(_isPath),
                            reached(_reached)
{
}

bool Tile::operator==(const Tile &b) const
{
  return (cubeCoords.q == b.cubeCoords.q &&
          cubeCoords.r == b.cubeCoords.r &&
          cubeCoords.s == b.cubeCoords.s);
}

std::vector<Tile *> Tile::getNeighbors()
{
  std::vector<Tile *> tiles;

  for (const auto &dir : NEIGHBORS_DIRECTIONS)
  {
    CubeCoords new_coords = cubeCoords + dir;
    auto id = Map::getTileId(new_coords.q, new_coords.r);
    auto tile = Map::getTile(id);

    // make sure the tile exist
    if (tile)
    {
      tiles.push_back(tile);
    }
  }

  return tiles;
}