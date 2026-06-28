#pragma once

#include <Arduino.h>
#include <LovyanGFX.hpp>

#include "Game.h"
#include "../ui/HomeScreen.h"
#include "../audio/SoundManager.h"

// =========================================================
// ESPLAY32 - Game manager
// =========================================================

class GameManager {
public:
  static const int MAX_GAMES = 6;

  void begin(lgfx::LGFX_Device& lcd, SoundManager& sound);

  bool registerGame(Game* game);

  void showHome();
  void update();
  void handleTouch(int x, int y);

  bool isHome() const;

private:
  lgfx::LGFX_Device* _lcd = nullptr;
  SoundManager* _sound = nullptr;

  HomeScreen _homeScreen;

  Game* _games[MAX_GAMES];
  int _gameCount = 0;

  int _activeGameIndex = -1;
  bool _home = true;

  void launchGame(int index);
};