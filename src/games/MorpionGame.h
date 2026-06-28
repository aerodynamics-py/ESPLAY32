#pragma once

#include <Arduino.h>
#include "../core/Game.h"
#include "../core/Types.h"

// =========================================================
// MORPION - Tic-tac-toe game
// =========================================================

class MorpionGame : public Game {
public:
  const char* getName() const override;
  const char* getSubtitle() const override;
  uint16_t getAccentColor() const override;

  void begin(lgfx::LGFX_Device& lcd, SoundManager& sound) override;
  void update() override;
  void handleTouch(int x, int y) override;

  bool wantsHome() const override;
  void clearHomeRequest() override;

private:
  lgfx::LGFX_Device* _lcd = nullptr;
  SoundManager* _sound = nullptr;

  bool _wantsHome = false;

  int _board[9];
  int _activePlayer = 1;
  int _winnerPlayer = 0;
  bool _drawGame = false;
  bool _gameOver = false;

  static const Rect BACK_BUTTON;
  static const Rect REPLAY_BUTTON;
  static const Rect HOME_BUTTON;

  static const int GRID_X = 250;
  static const int GRID_Y = 110;
  static const int CELL_SIZE = 100;
  static const int GRID_SIZE = 300;

  void resetGame();
  void drawScreen();
  void drawHeader();
  void drawGrid();
  void drawMarks();
  void drawFooter();
  void drawGameOver();

  Rect getCellRect(int index) const;
  int getCellAt(int x, int y) const;

  void handleCellTouch(int cell);
  int checkWinner() const;
  bool isBoardFull() const;

  void drawX(const Rect& r, uint16_t color);
  void drawO(const Rect& r, uint16_t color);
  void drawThickLine(int x1, int y1, int x2, int y2, uint16_t color, int thickness);
};