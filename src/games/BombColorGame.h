#pragma once

#include <Arduino.h>
#include "../core/Game.h"
#include "../core/Types.h"

// =========================================================
// BOMB COLOR - Duel minefield game
// =========================================================

class BombColorGame : public Game {
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
  enum Phase {
    PHASE_DIFFICULTY,
    PHASE_PLAYING,
    PHASE_GAME_OVER
  };

  lgfx::LGFX_Device* _lcd = nullptr;
  SoundManager* _sound = nullptr;

  bool _wantsHome = false;

  Phase _phase = PHASE_DIFFICULTY;

  static const int MAX_CELLS = 25;

  int _gridSize = 4;
  int _cellCount = 16;
  int _bombCount = 1;
  int _safeRevealed = 0;

  bool _bombs[MAX_CELLS];
  bool _revealed[MAX_CELLS];
  uint16_t _cellColors[MAX_CELLS];

  int _activePlayer = 1;
  int _winnerPlayer = 0;
  int _loserPlayer = 0;
  bool _allSafeWin = false;

  int _cellSize = 70;
  int _cellGap = 10;
  int _gridX = 0;
  int _gridY = 0;
  int _gridW = 0;
  int _gridH = 0;

  static const Rect BACK_BUTTON;
  static const Rect DIFF_4_BUTTON;
  static const Rect DIFF_5_BUTTON;
  static const Rect REPLAY_BUTTON;
  static const Rect HOME_BUTTON;

  void drawDifficultyScreen();
  void startGame(int gridSize, int bombCount);
  void resetBoard();
  void placeBombs();
  void randomizeColors();

  void computeGridLayout();
  Rect getCellRect(int cellIndex) const;
  int getGridCellAt(int x, int y) const;

  void drawGameScreen();
  void drawHeader();
  void drawGrid();
  void drawCell(int cellIndex);
  void drawFooter();
  void drawGameOverScreen();

  void revealAllBombs();
  void handleCellTouch(int cellIndex);
};