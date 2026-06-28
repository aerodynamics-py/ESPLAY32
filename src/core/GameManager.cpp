#include "GameManager.h"

void GameManager::begin(lgfx::LGFX_Device& lcd, SoundManager& sound) {
  _lcd = &lcd;
  _sound = &sound;
  _home = true;
  _activeGameIndex = -1;
}

bool GameManager::registerGame(Game* game) {
  if (game == nullptr) {
    return false;
  }

  if (_gameCount >= MAX_GAMES) {
    return false;
  }

  _games[_gameCount] = game;
  _gameCount++;

  return true;
}

void GameManager::showHome() {
  _home = true;
  _activeGameIndex = -1;

  _homeScreen.draw(*_lcd, _games, _gameCount);
}

void GameManager::launchGame(int index) {
  if (index < 0 || index >= _gameCount) {
    return;
  }

  _activeGameIndex = index;
  _home = false;

  Serial.print("[GAME_MANAGER] Launching ");
  Serial.println(_games[index]->getName());

  _games[index]->begin(*_lcd, *_sound);
}

void GameManager::update() {
  if (_home) {
    return;
  }

  if (_activeGameIndex < 0 || _activeGameIndex >= _gameCount) {
    showHome();
    return;
  }

  Game* activeGame = _games[_activeGameIndex];

  activeGame->update();

  if (activeGame->wantsHome()) {
    activeGame->clearHomeRequest();
    showHome();
  }
}

void GameManager::handleTouch(int x, int y) {
  if (_home) {
    int touchedIndex = _homeScreen.getTouchedGameIndex(x, y, _gameCount);

    if (touchedIndex >= 0) {
      _sound->playClick();
      launchGame(touchedIndex);
    }

    return;
  }

  if (_activeGameIndex >= 0 && _activeGameIndex < _gameCount) {
    _games[_activeGameIndex]->handleTouch(x, y);
  }
}

bool GameManager::isHome() const {
  return _home;
}