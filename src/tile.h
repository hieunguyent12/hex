#pragma once
#include <vector>
#include "cube_coordinates.h"

class Tile
{
public:
  CubeCoords cubeCoords;
  bool isWall;
  bool isTarget;
  bool isPlayer;
  bool isPath;
  bool isRiver;
  bool reached;
  int cost;

  Tile(CubeCoords cc, bool isWall, bool isTarget, bool isPlayer, bool isPath, bool isRiver,
       bool reached,
       int cost);
  Tile(CubeCoords cc);
  Tile();

  bool operator==(const Tile &b) const;
  // get neighboring tiles adjacent to this tile
  std::vector<Tile *> getNeighbors();
};