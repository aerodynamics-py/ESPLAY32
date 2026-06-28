#pragma once

#include <Arduino.h>
#include "../core/Game.h"
#include "../core/Types.h"

// =========================================================
// ARC-EN-CIEL - Memory game
// =========================================================

class ArcEnCielGame : public Game {
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
    PHASE_READY_GO,
    PHASE_PRE_SHOW,
    PHASE_SHOW_SEQUENCE,
    PHASE_WAIT_INPUT,
    PHASE_INPUT_FEEDBACK,
    PHASE_SUCCESS_FEEDBACK,
    PHASE_ERROR_FEEDBACK,
    PHASE_GAME_OVER
  };

  lgfx::LGFX_Device* _lcd = nullptr;
  SoundManager* _sound = nullptr;

  bool _wantsHome = false;

  Phase _phase = PHASE_DIFFICULTY;
  uint32_t _phaseTime = 0;
  uint32_t _preShowDelayMs = 0;

  int _gridSize = 3;
  int _cellCount = 9;

  static const int MAX_CELLS = 25;
  static const int MAX_SEQUENCE = 128;

  int _sequence[MAX_SEQUENCE];
  int _sequenceLength = 0;
  int _inputIndex = 0;

  int _activePlayer = 1;
  int _winnerPlayer = 0;
  int _loserPlayer = 0;

  int _showIndex = 0;
  bool _showCellLit = false;

  int _lastTouchedCell = -1;
  bool _lastTouchCleared = true;

  int _cellSize = 92;
  int _cellGap = 12;
  int _gridX = 0;
  int _gridY = 0;
  int _gridW = 0;
  int _gridH = 0;

  uint16_t _cellColors[MAX_CELLS];

  static const uint32_t INITIAL_GO_DELAY_MS = 1000;
  static const uint32_t ROUND_PRE_SHOW_MS   = 650;
  static const uint32_t SHOW_CELL_ON_MS     = 430;
  static const uint32_t SHOW_CELL_OFF_MS    = 170;
  static const uint32_t INPUT_FLASH_MS      = 140;

  static const Rect BACK_BUTTON;
  static const Rect DIFF_3_BUTTON;
  static const Rect DIFF_4_BUTTON;
  static const Rect DIFF_5_BUTTON;
  static const Rect GO_BUTTON;
  static const Rect REPLAY_BUTTON;
  static const Rect HOME_BUTTON;

  void drawDifficultyScreen();
  void drawReadyScreen();
  void drawGameFrame(const char* footer1, const char* footer2);
  void drawHeader();
  void drawFooter(const char* line1, const char* line2);
  void drawFooterWaitInput();
  void drawGameOverScreen();

  void drawSequencePreview(int x, int y, int w, int h);

  void computeGridLayout();
  Rect getCellRect(int cellIndex) const;
  int getGridCellAt(int x, int y) const;
  void eraseCellArea(int cellIndex);
  void drawCell(int cellIndex, bool highlighted);
  void drawGrid();

  void randomizeCellColors();

  void startGame(int gridSize);
  void beginNextRound(bool waitForGo);
  void appendRandomCell();
  void startSequenceDisplayAfterGo();
  void startSequenceDisplayAfterDelay(uint32_t delayMs);
  void updateShowSequence();
  void handleCellInput(int cellIndex);

  void changePhase(Phase newPhase);
};