#pragma once

// =========================================================
// ESPLAY32 - Shared simple types
// =========================================================

struct Rect {
  int x;
  int y;
  int w;
  int h;
};

inline bool pointInRect(int px, int py, const Rect& r) {
  return px >= r.x &&
         px <  r.x + r.w &&
         py >= r.y &&
         py <  r.y + r.h;
}