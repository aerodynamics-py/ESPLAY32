#pragma once

#include <Arduino.h>
#include <LovyanGFX.hpp>
#include "../audio/SoundManager.h"

// =========================================================
// ESPLAY32 - Generic game interface
// =========================================================

class Game {
public:
  virtual ~Game() {}

  virtual const char* getName() const = 0;
  virtual const char* getSubtitle() const = 0;
  virtual uint16_t getAccentColor() const = 0;

  virtual void begin(lgfx::LGFX_Device& lcd, SoundManager& sound) = 0;
  virtual void update() = 0;
  virtual void handleTouch(int x, int y) = 0;

  virtual bool wantsHome() const = 0;
  virtual void clearHomeRequest() = 0;
};