#pragma once

#include <LovyanGFX.hpp>
#include "../core/Game.h"
#include "../core/Types.h"

// =========================================================
// ESPLAY32 - Dynamic home screen
// =========================================================
//
// Dynamic tiles from registered games.
// Each known game can have its own visual identity.
// =========================================================

class HomeScreen {
public:
  void draw(lgfx::LGFX_Device& lcd, Game** games, int gameCount);
  int getTouchedGameIndex(int x, int y, int gameCount) const;

private:
  Rect getTileRect(int index, int gameCount) const;

  void drawGameTile(
    lgfx::LGFX_Device& lcd,
    const Rect& r,
    Game* game
  );

  void drawArcEnCielTile(
    lgfx::LGFX_Device& lcd,
    const Rect& r,
    Game* game
  );

  void drawMorpionTile(
    lgfx::LGFX_Device& lcd,
    const Rect& r,
    Game* game
  );

  void drawBombColorTile(
    lgfx::LGFX_Device& lcd,
    const Rect& r,
    Game* game
  );

  void drawDuelTargetTile(
    lgfx::LGFX_Device& lcd,
    const Rect& r,
    Game* game
  );

  void drawPuissance4Tile(
    lgfx::LGFX_Device& lcd,
    const Rect& r,
    Game* game
  );

  void drawCircuitPathTile(
    lgfx::LGFX_Device& lcd,
    const Rect& r,
    Game* game
  );

  void drawGenericTile(
    lgfx::LGFX_Device& lcd,
    const Rect& r,
    Game* game
  );

  void drawMiniX(
    lgfx::LGFX_Device& lcd,
    int cx,
    int cy,
    int size,
    uint16_t color
  );

  void drawMiniO(
    lgfx::LGFX_Device& lcd,
    int cx,
    int cy,
    int radius,
    uint16_t color
  );
};