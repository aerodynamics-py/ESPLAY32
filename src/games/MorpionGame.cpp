#include "MorpionGame.h"
#include "../config/AppConfig.h"
#include "../ui/UIHelpers.h"

using namespace ESPLAY;

const Rect MorpionGame::BACK_BUTTON   = { 650, 20, 120, 44 };
const Rect MorpionGame::REPLAY_BUTTON = { 220, 390, 160, 58 };
const Rect MorpionGame::HOME_BUTTON   = { 420, 390, 160, 58 };

const char* MorpionGame::getName() const {
  return "MORPION";
}

const char* MorpionGame::getSubtitle() const {
  return "X contre O";
}

uint16_t MorpionGame::getAccentColor() const {
  return COL_TEAL;
}

void MorpionGame::begin(lgfx::LGFX_Device& lcd, SoundManager& sound) {
  _lcd = &lcd;
  _sound = &sound;
  _wantsHome = false;

  resetGame();
  drawScreen();
}

void MorpionGame::update() {
}

void MorpionGame::handleTouch(int x, int y) {
  if (pointInRect(x, y, BACK_BUTTON)) {
    _sound->playClick();
    _wantsHome = true;
    return;
  }

  if (_gameOver) {
    if (pointInRect(x, y, REPLAY_BUTTON)) {
      _sound->playClick();
      resetGame();
      drawScreen();
      return;
    }

    if (pointInRect(x, y, HOME_BUTTON)) {
      _sound->playClick();
      _wantsHome = true;
      return;
    }

    return;
  }

  int cell = getCellAt(x, y);

  if (cell >= 0) {
    handleCellTouch(cell);
  }
}

bool MorpionGame::wantsHome() const {
  return _wantsHome;
}

void MorpionGame::clearHomeRequest() {
  _wantsHome = false;
}

void MorpionGame::resetGame() {
  for (int i = 0; i < 9; i++) {
    _board[i] = 0;
  }

  _activePlayer = 1;
  _winnerPlayer = 0;
  _drawGame = false;
  _gameOver = false;
}

void MorpionGame::drawScreen() {
  _lcd->fillScreen(COL_BLACK);

  drawHeader();
  drawGrid();
  drawMarks();
  drawFooter();
}

void MorpionGame::drawHeader() {
  _lcd->fillRect(0, 0, SCREEN_W, 86, COL_DARKGREY);

  _lcd->setTextDatum(TL_DATUM);

  _lcd->setTextColor(COL_WHITE, COL_DARKGREY);
  _lcd->setTextSize(3);
  _lcd->drawString("MORPION", 32, 18);

  char playerText[32];
  snprintf(playerText, sizeof(playerText), "Joueur %d", _activePlayer);

  _lcd->setTextColor(COL_LIGHTGREY, COL_DARKGREY);
  _lcd->setTextSize(2);
  _lcd->drawString(playerText, 330, 24);

  UI::drawSmallButton(*_lcd, BACK_BUTTON, "ACCUEIL");
}

void MorpionGame::drawGrid() {
  _lcd->fillRoundRect(GRID_X - 8, GRID_Y - 8, GRID_SIZE + 16, GRID_SIZE + 16, 18, COL_BLACK);
  _lcd->drawRoundRect(GRID_X - 8, GRID_Y - 8, GRID_SIZE + 16, GRID_SIZE + 16, 18, COL_LIGHTGREY);

  _lcd->fillRect(GRID_X + CELL_SIZE - 2, GRID_Y, 4, GRID_SIZE, COL_LIGHTGREY);
  _lcd->fillRect(GRID_X + 2 * CELL_SIZE - 2, GRID_Y, 4, GRID_SIZE, COL_LIGHTGREY);

  _lcd->fillRect(GRID_X, GRID_Y + CELL_SIZE - 2, GRID_SIZE, 4, COL_LIGHTGREY);
  _lcd->fillRect(GRID_X, GRID_Y + 2 * CELL_SIZE - 2, GRID_SIZE, 4, COL_LIGHTGREY);
}

void MorpionGame::drawMarks() {
  for (int i = 0; i < 9; i++) {
    Rect r = getCellRect(i);

    if (_board[i] == 1) {
      drawX(r, COL_CYAN);
    }

    else if (_board[i] == 2) {
      drawO(r, COL_ORANGE);
    }
  }
}

void MorpionGame::drawFooter() {
  _lcd->fillRect(0, 426, SCREEN_W, 54, COL_BLACK);

  char line[64];

  if (_gameOver) {
    if (_winnerPlayer > 0) {
      snprintf(line, sizeof(line), "Joueur %d gagne", _winnerPlayer);
    } else {
      snprintf(line, sizeof(line), "Egalite");
    }
  } else {
    snprintf(line, sizeof(line), "Joueur %d : a toi", _activePlayer);
  }

  UI::drawCenteredText(*_lcd, line, SCREEN_W / 2, 450, 2, COL_WHITE, COL_BLACK);
}

void MorpionGame::drawGameOver() {
  drawHeader();
  drawGrid();
  drawMarks();

  _lcd->fillRect(0, 426, SCREEN_W, 54, COL_BLACK);
  drawFooter();

  UI::drawButton(*_lcd, REPLAY_BUTTON, "REJOUER");
  UI::drawButton(*_lcd, HOME_BUTTON, "ACCUEIL");
}

Rect MorpionGame::getCellRect(int index) const {
  int row = index / 3;
  int col = index % 3;

  Rect r;
  r.x = GRID_X + col * CELL_SIZE;
  r.y = GRID_Y + row * CELL_SIZE;
  r.w = CELL_SIZE;
  r.h = CELL_SIZE;

  return r;
}

int MorpionGame::getCellAt(int x, int y) const {
  if (x < GRID_X || x >= GRID_X + GRID_SIZE) {
    return -1;
  }

  if (y < GRID_Y || y >= GRID_Y + GRID_SIZE) {
    return -1;
  }

  int col = (x - GRID_X) / CELL_SIZE;
  int row = (y - GRID_Y) / CELL_SIZE;

  return row * 3 + col;
}

void MorpionGame::handleCellTouch(int cell) {
  if (cell < 0 || cell >= 9) {
    return;
  }

  if (_board[cell] != 0) {
    return;
  }

  _sound->playClick();

  _board[cell] = _activePlayer;

  _winnerPlayer = checkWinner();

  if (_winnerPlayer > 0) {
    _gameOver = true;
    _sound->playSuccess();
    drawGameOver();
    return;
  }

  if (isBoardFull()) {
    _gameOver = true;
    _drawGame = true;
    _sound->playGameOver();
    drawGameOver();
    return;
  }

  _activePlayer = (_activePlayer == 1) ? 2 : 1;
  drawScreen();
}

int MorpionGame::checkWinner() const {
  static const int lines[8][3] = {
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8},
    {0, 3, 6},
    {1, 4, 7},
    {2, 5, 8},
    {0, 4, 8},
    {2, 4, 6}
  };

  for (int i = 0; i < 8; i++) {
    int a = lines[i][0];
    int b = lines[i][1];
    int c = lines[i][2];

    if (_board[a] != 0 &&
        _board[a] == _board[b] &&
        _board[b] == _board[c]) {
      return _board[a];
    }
  }

  return 0;
}

bool MorpionGame::isBoardFull() const {
  for (int i = 0; i < 9; i++) {
    if (_board[i] == 0) {
      return false;
    }
  }

  return true;
}

void MorpionGame::drawThickLine(int x1, int y1, int x2, int y2, uint16_t color, int thickness) {
  for (int i = -thickness; i <= thickness; i++) {
    _lcd->drawLine(x1 + i, y1, x2 + i, y2, color);
    _lcd->drawLine(x1, y1 + i, x2, y2 + i, color);
  }
}

void MorpionGame::drawX(const Rect& r, uint16_t color) {
  int m = 25;

  drawThickLine(r.x + m, r.y + m, r.x + r.w - m, r.y + r.h - m, color, 3);
  drawThickLine(r.x + r.w - m, r.y + m, r.x + m, r.y + r.h - m, color, 3);
}

void MorpionGame::drawO(const Rect& r, uint16_t color) {
  int cx = r.x + r.w / 2;
  int cy = r.y + r.h / 2;
  int radius = 30;

  for (int i = 0; i < 5; i++) {
    _lcd->drawCircle(cx, cy, radius + i, color);
  }
}