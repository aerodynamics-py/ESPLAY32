#pragma once

#include <Arduino.h>
#include "../core/Game.h"
#include "../core/Types.h"

// =========================================================
// PUISSANCE 4 - Connect Four
// =========================================================
//
// Standard 7 columns x 6 rows.
// Players touch a column.
// Token falls to the lowest available row.
// Four aligned tokens wins.
// =========================================================

class Puissance4Game : public Game {
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
  static const int ROWS = 6;
  static const int COLS = 7;

  lgfx::LGFX_Device* _lcd = nullptr;
  SoundManager* _sound = nullptr;

  bool _wantsHome = false;

  int _board[ROWS][COLS];

  int _activePlayer = 1;
  int _winnerPlayer = 0;
  bool _drawGame = false;
  bool _gameOver = false;

  int _winningRow[4];
  int _winningCol[4];

  static const Rect BACK_BUTTON;
  static const Rect REPLAY_BUTTON;
  static const Rect HOME_BUTTON;

  static const int CELL_SIZE = 46;
  static const int CELL_GAP = 6;
  static const int GRID_W = COLS * CELL_SIZE + (COLS - 1) * CELL_GAP;
  static const int GRID_H = ROWS * CELL_SIZE + (ROWS - 1) * CELL_GAP;
  static const int GRID_X = (800 - GRID_W) / 2;
  static const int GRID_Y = 102;

  void resetGame();

  void drawScreen();
  void drawHeader();
  void drawBoard();
  void drawFooter();
  void drawGameOver();

  int getColumnAt(int x, int y) const;
  bool dropToken(int col, int& placedRow);

  bool checkWinnerFrom(int row, int col);
  int countDirection(int row, int col, int dRow, int dCol, int player) const;
  bool isBoardFull() const;

  bool isWinningCell(int row, int col) const;

  void drawToken(int row, int col, int player);
  void drawColumnHint(int col);
};