#include "HomeScreen.h"
#include "UIHelpers.h"
#include "../config/AppConfig.h"

#include <string.h>

using namespace ESPLAY;

// =========================================================
// ESPLAY32 - Dynamic home screen
// =========================================================

void HomeScreen::draw(lgfx::LGFX_Device& lcd, Game** games, int gameCount) {
  lcd.fillScreen(COL_BLACK);

  UI::drawHeader(
    lcd,
    "ESPLAY32",
    "Mini plateforme de jeux tactiles"
  );

  for (int i = 0; i < gameCount; i++) {
    Rect r = getTileRect(i, gameCount);
    drawGameTile(lcd, r, games[i]);
  }

  lcd.setTextDatum(TL_DATUM);
  lcd.setTextColor(COL_LIGHTGREY, COL_BLACK);
  lcd.setTextSize(1);
  lcd.drawString("Plateforme multi-jeux", 32, 450);
}

int HomeScreen::getTouchedGameIndex(int x, int y, int gameCount) const {
  for (int i = 0; i < gameCount; i++) {
    Rect r = getTileRect(i, gameCount);

    if (pointInRect(x, y, r)) {
      return i;
    }
  }

  return -1;
}

Rect HomeScreen::getTileRect(int index, int gameCount) const {
  (void)gameCount;

  const int cols = 3;
  const int tileW = 220;
  const int tileH = 145;
  const int gapX = 30;
  const int gapY = 25;
  const int startX = 40;
  const int startY = 115;

  int row = index / cols;
  int col = index % cols;

  Rect r;
  r.x = startX + col * (tileW + gapX);
  r.y = startY + row * (tileH + gapY);
  r.w = tileW;
  r.h = tileH;

  return r;
}

// =========================================================
// Tile router
// =========================================================

void HomeScreen::drawGameTile(
  lgfx::LGFX_Device& lcd,
  const Rect& r,
  Game* game
) {
  const char* name = game->getName();

  if (strcmp(name, "ARC-EN-CIEL") == 0) {
    drawArcEnCielTile(lcd, r, game);
    return;
  }

  if (strcmp(name, "MORPION") == 0) {
    drawMorpionTile(lcd, r, game);
    return;
  }

  if (strcmp(name, "BOMB COLOR") == 0) {
    drawBombColorTile(lcd, r, game);
    return;
  }

  if (strcmp(name, "DUEL TARGET") == 0) {
    drawDuelTargetTile(lcd, r, game);
    return;
  }

  if (strcmp(name, "PUISSANCE 4") == 0) {
    drawPuissance4Tile(lcd, r, game);
    return;
  }

  if (strcmp(name, "CIRCUIT PATH") == 0) {
    drawCircuitPathTile(lcd, r, game);
    return;
  }

  drawGenericTile(lcd, r, game);
}

// =========================================================
// ARC-EN-CIEL tile
// =========================================================

void HomeScreen::drawArcEnCielTile(
  lgfx::LGFX_Device& lcd,
  const Rect& r,
  Game* game
) {
  uint16_t bg = COL_NAVY;
  uint16_t inner = rgb565(10, 10, 45);

  lcd.fillRoundRect(r.x, r.y, r.w, r.h, 18, bg);
  lcd.drawRoundRect(r.x, r.y, r.w, r.h, 18, COL_WHITE);

  lcd.fillRoundRect(r.x + 12, r.y + 12, r.w - 24, r.h - 24, 14, inner);

  const int stripX = r.x + 24;
  const int stripY = r.y + 22;
  const int stripW = r.w - 48;
  const int stripH = 8;

  lcd.fillRoundRect(stripX, stripY,      stripW, stripH, 4, COL_RED);
  lcd.fillRoundRect(stripX, stripY + 11, stripW, stripH, 4, COL_ORANGE);
  lcd.fillRoundRect(stripX, stripY + 22, stripW, stripH, 4, COL_YELLOW);
  lcd.fillRoundRect(stripX, stripY + 33, stripW, stripH, 4, COL_GREEN);
  lcd.fillRoundRect(stripX, stripY + 44, stripW, stripH, 4, COL_CYAN);
  lcd.fillRoundRect(stripX, stripY + 55, stripW, stripH, 4, COL_PURPLE);

  lcd.fillRect(r.x + 30,  r.y + 94, 4, 4, COL_WHITE);
  lcd.fillRect(r.x + 184, r.y + 24, 4, 4, COL_WHITE);
  lcd.fillRect(r.x + 172, r.y + 92, 4, 4, COL_YELLOW);
  lcd.fillRect(r.x + 46,  r.y + 112, 4, 4, COL_CYAN);

  lcd.setTextDatum(MC_DATUM);

  lcd.setTextColor(COL_WHITE, inner);
  lcd.setTextSize(2);
  lcd.drawString(game->getName(), r.x + r.w / 2, r.y + 92);

  lcd.setTextColor(COL_LIGHTGREY, inner);
  lcd.setTextSize(1);
  lcd.drawString(game->getSubtitle(), r.x + r.w / 2, r.y + 122);
}

// =========================================================
// MORPION tile
// =========================================================

void HomeScreen::drawMorpionTile(
  lgfx::LGFX_Device& lcd,
  const Rect& r,
  Game* game
) {
  uint16_t bg = game->getAccentColor();

  lcd.fillRoundRect(r.x, r.y, r.w, r.h, 18, bg);
  lcd.drawRoundRect(r.x, r.y, r.w, r.h, 18, COL_WHITE);

  const int gx = r.x + 24;
  const int gy = r.y + 22;
  const int cell = 26;
  const int grid = cell * 3;

  lcd.drawRoundRect(gx - 6, gy - 6, grid + 12, grid + 12, 10, COL_LIGHTGREY);

  lcd.drawLine(gx + cell,     gy, gx + cell,     gy + grid, COL_LIGHTGREY);
  lcd.drawLine(gx + cell * 2, gy, gx + cell * 2, gy + grid, COL_LIGHTGREY);

  lcd.drawLine(gx, gy + cell,     gx + grid, gy + cell,     COL_LIGHTGREY);
  lcd.drawLine(gx, gy + cell * 2, gx + grid, gy + cell * 2, COL_LIGHTGREY);

  drawMiniX(lcd, gx + cell / 2,              gy + cell / 2,              16, COL_CYAN);
  drawMiniO(lcd, gx + cell + cell / 2,       gy + cell / 2,              8,  COL_ORANGE);
  drawMiniX(lcd, gx + cell * 2 + cell / 2,   gy + cell + cell / 2,       16, COL_CYAN);
  drawMiniO(lcd, gx + cell / 2,              gy + cell * 2 + cell / 2,   8,  COL_ORANGE);

  lcd.setTextDatum(MC_DATUM);

  lcd.setTextColor(COL_WHITE, bg);
  lcd.setTextSize(2);
  lcd.drawString(game->getName(), r.x + r.w / 2 + 36, r.y + 52);

  lcd.setTextColor(COL_LIGHTGREY, bg);
  lcd.setTextSize(1);
  lcd.drawString(game->getSubtitle(), r.x + r.w / 2 + 36, r.y + 86);

  lcd.drawRoundRect(r.x + 16, r.y + 112, r.w - 32, 18, 8, COL_LIGHTGREY);
}

// =========================================================
// BOMB COLOR tile
// =========================================================

void HomeScreen::drawBombColorTile(
  lgfx::LGFX_Device& lcd,
  const Rect& r,
  Game* game
) {
  uint16_t bg = rgb565(90, 18, 24);

  lcd.fillRoundRect(r.x, r.y, r.w, r.h, 18, bg);
  lcd.drawRoundRect(r.x, r.y, r.w, r.h, 18, COL_WHITE);

  const int startX = r.x + 24;
  const int startY = r.y + 22;
  const int size = 26;
  const int gap = 8;

  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 4; col++) {
      int x = startX + col * (size + gap);
      int y = startY + row * (size + gap);

      uint16_t c = COL_DARKGREY;

      if (row == 1 && col == 2) {
        c = COL_RED;
      }

      lcd.fillRoundRect(x, y, size, size, 6, c);
      lcd.drawRoundRect(x, y, size, size, 6, COL_LIGHTGREY);

      lcd.setTextDatum(MC_DATUM);
      lcd.setTextSize(1);

      if (row == 1 && col == 2) {
        lcd.setTextColor(COL_BLACK, c);
        lcd.drawString("B", x + size / 2, y + size / 2);
      } else {
        lcd.setTextColor(COL_LIGHTGREY, c);
        lcd.drawString("?", x + size / 2, y + size / 2);
      }
    }
  }

  lcd.drawLine(r.x + 146, r.y + 40, r.x + 168, r.y + 24, COL_LIGHTGREY);
  lcd.fillCircle(r.x + 172, r.y + 22, 4, COL_YELLOW);

  lcd.setTextDatum(MC_DATUM);

  lcd.setTextColor(COL_WHITE, bg);
  lcd.setTextSize(2);
  lcd.drawString(game->getName(), r.x + r.w / 2 + 34, r.y + 92);

  lcd.setTextColor(COL_LIGHTGREY, bg);
  lcd.setTextSize(1);
  lcd.drawString(game->getSubtitle(), r.x + r.w / 2 + 34, r.y + 120);
}

// =========================================================
// DUEL TARGET tile
// =========================================================

void HomeScreen::drawDuelTargetTile(
  lgfx::LGFX_Device& lcd,
  const Rect& r,
  Game* game
) {
  uint16_t bg = rgb565(18, 52, 86);

  lcd.fillRoundRect(r.x, r.y, r.w, r.h, 18, bg);
  lcd.drawRoundRect(r.x, r.y, r.w, r.h, 18, COL_WHITE);

  lcd.drawLine(r.x + r.w / 2, r.y + 18, r.x + r.w / 2, r.y + 96, COL_LIGHTGREY);

  int cy = r.y + 52;
  int leftCx = r.x + 58;
  int rightCx = r.x + r.w - 58;

  lcd.fillCircle(leftCx, cy, 28, rgb565(20, 20, 20));
  lcd.drawCircle(leftCx, cy, 28, COL_CYAN);
  lcd.drawCircle(leftCx, cy, 18, COL_CYAN);
  lcd.fillCircle(leftCx, cy, 8, COL_CYAN);

  lcd.fillCircle(rightCx, cy, 28, rgb565(20, 20, 20));
  lcd.drawCircle(rightCx, cy, 28, COL_ORANGE);
  lcd.drawCircle(rightCx, cy, 18, COL_ORANGE);
  lcd.fillCircle(rightCx, cy, 8, COL_ORANGE);

  lcd.setTextDatum(MC_DATUM);

  lcd.setTextColor(COL_WHITE, bg);
  lcd.setTextSize(2);
  lcd.drawString(game->getName(), r.x + r.w / 2, r.y + 104);

  lcd.setTextColor(COL_LIGHTGREY, bg);
  lcd.setTextSize(1);
  lcd.drawString(game->getSubtitle(), r.x + r.w / 2, r.y + 127);
}

// =========================================================
// PUISSANCE 4 tile
// =========================================================

void HomeScreen::drawPuissance4Tile(
  lgfx::LGFX_Device& lcd,
  const Rect& r,
  Game* game
) {
  uint16_t bg = rgb565(20, 50, 140);
  uint16_t boardBlue = rgb565(18, 86, 190);

  lcd.fillRoundRect(r.x, r.y, r.w, r.h, 18, bg);
  lcd.drawRoundRect(r.x, r.y, r.w, r.h, 18, COL_WHITE);

  const int cols = 7;
  const int rows = 4;
  const int cell = 17;
  const int gap = 5;

  int boardW = cols * cell + (cols - 1) * gap;
  int boardH = rows * cell + (rows - 1) * gap;

  int bx = r.x + (r.w - boardW) / 2;
  int by = r.y + 18;

  lcd.fillRoundRect(bx - 8, by - 8, boardW + 16, boardH + 16, 10, boardBlue);

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      int cx = bx + col * (cell + gap) + cell / 2;
      int cy = by + row * (cell + gap) + cell / 2;

      uint16_t token = COL_BLACK;

      if ((row == 3 && col == 1) || (row == 2 && col == 2) || (row == 3 && col == 3)) {
        token = COL_RED;
      }

      if ((row == 3 && col == 2) || (row == 2 && col == 3) || (row == 3 && col == 4)) {
        token = COL_YELLOW;
      }

      lcd.fillCircle(cx, cy, cell / 2 - 1, token);
      lcd.drawCircle(cx, cy, cell / 2 - 1, COL_LIGHTGREY);
    }
  }

  lcd.setTextDatum(MC_DATUM);

  lcd.setTextColor(COL_WHITE, bg);
  lcd.setTextSize(2);
  lcd.drawString(game->getName(), r.x + r.w / 2, r.y + 106);

  lcd.setTextColor(COL_LIGHTGREY, bg);
  lcd.setTextSize(1);
  lcd.drawString(game->getSubtitle(), r.x + r.w / 2, r.y + 128);
}

// =========================================================
// CIRCUIT PATH tile
// =========================================================

void HomeScreen::drawCircuitPathTile(
  lgfx::LGFX_Device& lcd,
  const Rect& r,
  Game* game
) {
  uint16_t bg = rgb565(8, 55, 38);
  uint16_t board = rgb565(12, 82, 55);
  uint16_t wire = rgb565(90, 255, 170);

  lcd.fillRoundRect(r.x, r.y, r.w, r.h, 18, bg);
  lcd.drawRoundRect(r.x, r.y, r.w, r.h, 18, COL_WHITE);

  lcd.fillRoundRect(r.x + 18, r.y + 18, r.w - 36, 74, 12, board);

  // PCB traces
  int cy = r.y + 55;

  lcd.fillRect(r.x + 34, cy - 3, 56, 6, wire);
  lcd.fillRect(r.x + 88, cy - 3, 6, 32, wire);
  lcd.fillRect(r.x + 88, cy + 26, 48, 6, wire);
  lcd.fillRect(r.x + 132, cy - 24, 6, 56, wire);
  lcd.fillRect(r.x + 132, cy - 24, 46, 6, wire);

  lcd.fillCircle(r.x + 34, cy, 8, COL_GOLD);
  lcd.fillCircle(r.x + 178, cy - 24, 10, COL_YELLOW);
  lcd.drawCircle(r.x + 178, cy - 24, 14, COL_YELLOW);

  // Small electronic pads
  lcd.fillCircle(r.x + 88, cy + 29, 5, COL_LIGHTGREY);
  lcd.fillCircle(r.x + 135, cy + 29, 5, COL_LIGHTGREY);
  lcd.fillCircle(r.x + 135, cy - 24, 5, COL_LIGHTGREY);

  lcd.setTextDatum(MC_DATUM);

  lcd.setTextColor(COL_WHITE, bg);
  lcd.setTextSize(2);
  lcd.drawString(game->getName(), r.x + r.w / 2, r.y + 106);

  lcd.setTextColor(COL_LIGHTGREY, bg);
  lcd.setTextSize(1);
  lcd.drawString(game->getSubtitle(), r.x + r.w / 2, r.y + 128);
}

// =========================================================
// Generic fallback tile
// =========================================================

void HomeScreen::drawGenericTile(
  lgfx::LGFX_Device& lcd,
  const Rect& r,
  Game* game
) {
  uint16_t accent = game->getAccentColor();

  lcd.fillRoundRect(r.x, r.y, r.w, r.h, 18, accent);
  lcd.drawRoundRect(r.x, r.y, r.w, r.h, 18, COL_WHITE);

  lcd.setTextDatum(MC_DATUM);

  lcd.setTextColor(COL_WHITE, accent);
  lcd.setTextSize(2);
  lcd.drawString(game->getName(), r.x + r.w / 2, r.y + 48);

  lcd.setTextColor(COL_LIGHTGREY, accent);
  lcd.setTextSize(1);
  lcd.drawString(game->getSubtitle(), r.x + r.w / 2, r.y + 86);

  lcd.drawRoundRect(r.x + 16, r.y + 112, r.w - 32, 18, 8, COL_LIGHTGREY);
}

// =========================================================
// Mini drawing helpers
// =========================================================

void HomeScreen::drawMiniX(
  lgfx::LGFX_Device& lcd,
  int cx,
  int cy,
  int size,
  uint16_t color
) {
  int h = size / 2;

  for (int i = -1; i <= 1; i++) {
    lcd.drawLine(cx - h, cy - h + i, cx + h, cy + h + i, color);
    lcd.drawLine(cx + h, cy - h + i, cx - h, cy + h + i, color);
  }
}

void HomeScreen::drawMiniO(
  lgfx::LGFX_Device& lcd,
  int cx,
  int cy,
  int radius,
  uint16_t color
) {
  for (int i = 0; i < 3; i++) {
    lcd.drawCircle(cx, cy, radius + i, color);
  }
}