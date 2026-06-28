#pragma once

#include <Arduino.h>

// =========================================================
// ESPLAY32 - Sound manager
// =========================================================

class SoundManager {
public:
  void begin();

  void playClick();
  void playSequenceStep(int cellIndex);
  void playSuccess();
  void playError();
  void playGameOver();
};