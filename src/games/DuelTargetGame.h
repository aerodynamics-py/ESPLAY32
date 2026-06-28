#pragma once

#include <Arduino.h>
#include "../core/Game.h"
#include "../core/Types.h"

// =========================================================
// DUEL TARGET - Two-player reflex game
// =========================================================
//
// Each player has one half of the screen.
// A target appears on both sides.
// First player to touch their own target scores.
// First to TARGET_SCORE wins.
// =========================================================

class DuelTargetGame : public Game {
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
    PHASE_READY,
    PHASE_WAIT_TARGET,
    PHASE_TARGET_VISIBLE,
    PHASE_ROUND_RESULT,
    PHASE_GAME_OVER
  };

  lgfx::LGFX_Device* _lcd = nullptr;
  SoundManager* _sound = nullptr;

  bool _wantsHome = false;

  Phase _phase = PHASE_READY;
  uint32_t _phaseTime = 0;
  uint32_t _targetAppearTime = 0;

  int _scoreP1 = 0;
  int _scoreP2 = 0;
  int _roundWinner = 0;
  int _winnerPlayer = 0;

  int _targetLeftX = 160;
  int _targetRightX = 640;
  int _targetY = 240;
  int _targetRadius = 42;

  static const int TARGET_SCORE = 5;

  static const uint32_t ROUND_RESULT_MS = 850;

  static const Rect BACK_BUTTON;
  static const Rect START_BUTTON;
  static const Rect REPLAY_BUTTON;
  static const Rect HOME_BUTTON;

  void resetMatch();
  void startRound();
  void showTarget();

  void drawReadyScreen();
  void drawWaitingScreen();
  void drawTargetScreen();
  void drawRoundResultScreen();
  void drawGameOverScreen();

  void drawHeader(const char* subtitle);
  void drawScores();
  void drawArenaBase();
  void drawTarget(int cx, int cy, uint16_t color);
  void drawFooter(const char* text);

  bool isInsideCircle(int x, int y, int cx, int cy, int radius) const;
  void scorePoint(int player);
  void changePhase(Phase phase);
};