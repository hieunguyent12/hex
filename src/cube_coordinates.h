#pragma once
#include <iostream>

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

  CubeCoords(int _q, int _r, int _s) : q(_q), r(_r), s(_s)
  {
    assert(q + r + s == 0);
  }

  CubeCoords operator+(const CubeCoords &other)
  {
    return CubeCoords(q + other.q, r + other.r);
  }
};