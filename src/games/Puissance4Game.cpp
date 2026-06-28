#include "Puissance4Game.h"
#include "../config/AppConfig.h"
#include "../ui/UIHelpers.h"

using namespace ESPLAY;

const Rect Puissance4Game::BACK_BUTTON   = { 650, 20, 120, 44 };
const Rect Puissance4Game::REPLAY_BUTTON = { 220, 426, 160, 48 };
const Rect Puissance4Game::HOME_BUTTON   = { 420, 426, 160, 48 };

const char* Puissance4Game::getName() const {
  return "PUISSANCE 4";
}

const char* Puissance4Game::getSubtitle() const {
  return "Aligner 4";
}

uint16_t Puissance4Game::getAccentColor() const {
  return rgb565(20, 50, 140);
}

void Puissance4Game::begin(lgfx::LGFX_Device& lcd, SoundManager& sound) {
  _lcd = &lcd;
  _sound = &sound;
  _wantsHome = false;

  resetGame();
  drawScreen();
}

void Puissance4Game::update() {
}

void Puissance4Game::handleTouch(int x, int y) {
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

  int col = getColumnAt(x, y);

  if (col < 0) {
    return;
  }

  int placedRow = -1;

  if (!dropToken(col, placedRow)) {
    return;
  }

  _sound->playClick();

  if (checkWinnerFrom(placedRow, col)) {
    _winnerPlayer = _activePlayer;
    _gameOver = true;
    _sound->playSuccess();
    drawGameOver();
    return;
  }

  if (isBoardFull()) {
    _drawGame = true;
    _gameOver = true;
    _sound->playGameOver();
    drawGameOver();
    return;
  }

  _activePlayer = (_activePlayer == 1) ? 2 : 1;
  drawScreen();
}

bool Puissance4Game::wantsHome() const {
  return _wantsHome;
}

void Puissance4Game::clearHomeRequest() {
  _wantsHome = false;
}

void Puissance4Game::resetGame() {
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      _board[row][col] = 0;
    }
  }

  _activePlayer = 1;
  _winnerPlayer = 0;
  _drawGame = false;
  _gameOver = false;

  for (int i = 0; i < 4; i++) {
    _winningRow[i] = -1;
    _winningCol[i] = -1;
  }
}

void Puissance4Game::drawScreen() {
  _lcd->fillScreen(COL_BLACK);

  drawHeader();
  drawBoard();
  drawFooter();
}

void Puissance4Game::drawHeader() {
  _lcd->fillRect(0, 0, SCREEN_W, 86, COL_DARKGREY);

  _lcd->setTextDatum(TL_DATUM);

  _lcd->setTextColor(COL_WHITE, COL_DARKGREY);
  _lcd->setTextSize(3);
  _lcd->drawString("PUISSANCE 4", 32, 18);

  char playerText[32];
  snprintf(playerText, sizeof(playerText), "Joueur %d", _activePlayer);

  _lcd->setTextColor(COL_LIGHTGREY, COL_DARKGREY);
  _lcd->setTextSize(2);
  _lcd->drawString(playerText, 350, 24);

  UI::drawSmallButton(*_lcd, BACK_BUTTON, "ACCUEIL");
}

void Puissance4Game::drawBoard() {
  uint16_t boardBlue = rgb565(18, 86, 190);

  _lcd->fillRoundRect(
    GRID_X - 14,
    GRID_Y - 14,
    GRID_W + 28,
    GRID_H + 28,
    18,
    boardBlue
  );

  _lcd->drawRoundRect(
    GRID_X - 14,
    GRID_Y - 14,
    GRID_W + 28,
    GRID_H + 28,
    18,
    COL_LIGHTGREY
  );

  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      drawToken(row, col, _board[row][col]);
    }
  }
}

void Puissance4Game::drawToken(int row, int col, int player) {
  int x = GRID_X + col * (CELL_SIZE + CELL_GAP);
  int y = GRID_Y + row * (CELL_SIZE + CELL_GAP);

  int cx = x + CELL_SIZE / 2;
  int cy = y + CELL_SIZE / 2;
  int radius = CELL_SIZE / 2 - 4;

  uint16_t color = COL_BLACK;

  if (player == 1) {
    color = COL_RED;
  }

  else if (player == 2) {
    color = COL_YELLOW;
  }

  _lcd->fillCircle(cx, cy, radius, color);
  _lcd->drawCircle(cx, cy, radius, COL_LIGHTGREY);

  if (isWinningCell(row, col)) {
    _lcd->drawCircle(cx, cy, radius + 2, COL_WHITE);
    _lcd->drawCircle(cx, cy, radius + 3, COL_WHITE);
  }
}

void Puissance4Game::drawFooter() {
  _lcd->fillRect(0, 426, SCREEN_W, 54, COL_BLACK);

  char line[64];
  snprintf(line, sizeof(line), "Joueur %d : touche une colonne", _activePlayer);

  uint16_t color = (_activePlayer == 1) ? COL_RED : COL_YELLOW;

  UI::drawCenteredText(*_lcd, line, SCREEN_W / 2, 450, 2, color, COL_BLACK);
}

void Puissance4Game::drawGameOver() {
  _lcd->fillScreen(COL_BLACK);

  drawHeader();
  drawBoard();

  _lcd->fillRect(0, 416, SCREEN_W, 64, COL_BLACK);

  _lcd->setTextDatum(MC_DATUM);

  if (_drawGame) {
    _lcd->setTextColor(COL_WHITE, COL_BLACK);
    _lcd->setTextSize(2);
    _lcd->drawString("Egalite", SCREEN_W / 2, 410);
  } else {
    char text[48];
    snprintf(text, sizeof(text), "Joueur %d gagne", _winnerPlayer);

    uint16_t color = (_winnerPlayer == 1) ? COL_RED : COL_YELLOW;

    _lcd->setTextColor(color, COL_BLACK);
    _lcd->setTextSize(2);
    _lcd->drawString(text, SCREEN_W / 2, 410);
  }

  UI::drawButton(*_lcd, REPLAY_BUTTON, "REJOUER");
  UI::drawButton(*_lcd, HOME_BUTTON, "ACCUEIL");
}

int Puissance4Game::getColumnAt(int x, int y) const {
  int panelX = GRID_X - 14;
  int panelY = GRID_Y - 14;
  int panelW = GRID_W + 28;
  int panelH = GRID_H + 28;

  if (x < panelX || x >= panelX + panelW) {
    return -1;
  }

  if (y < panelY || y >= panelY + panelH) {
    return -1;
  }

  int relativeX = x - GRID_X;

  if (relativeX < 0) {
    relativeX = 0;
  }

  int col = relativeX / (CELL_SIZE + CELL_GAP);

  if (col < 0 || col >= COLS) {
    return -1;
  }

  return col;
}

bool Puissance4Game::dropToken(int col, int& placedRow) {
  if (col < 0 || col >= COLS) {
    return false;
  }

  for (int row = ROWS - 1; row >= 0; row--) {
    if (_board[row][col] == 0) {
      _board[row][col] = _activePlayer;
      placedRow = row;
      return true;
    }
  }

  return false;
}

bool Puissance4Game::checkWinnerFrom(int row, int col) {
  int player = _board[row][col];

  if (player == 0) {
    return false;
  }

  const int directions[4][2] = {
    {0, 1},   // horizontal
    {1, 0},   // vertical
    {1, 1},   // diagonal down
    {1, -1}   // diagonal up
  };

  for (int i = 0; i < 4; i++) {
    int dRow = directions[i][0];
    int dCol = directions[i][1];

    int total =
      1 +
      countDirection(row, col, dRow, dCol, player) +
      countDirection(row, col, -dRow, -dCol, player);

    if (total >= 4) {
      int index = 0;

      _winningRow[index] = row;
      _winningCol[index] = col;
      index++;

      int r = row + dRow;
      int c = col + dCol;

      while (
        index < 4 &&
        r >= 0 && r < ROWS &&
        c >= 0 && c < COLS &&
        _board[r][c] == player
      ) {
        _winningRow[index] = r;
        _winningCol[index] = c;
        index++;

        r += dRow;
        c += dCol;
      }

      r = row - dRow;
      c = col - dCol;

      while (
        index < 4 &&
        r >= 0 && r < ROWS &&
        c >= 0 && c < COLS &&
        _board[r][c] == player
      ) {
        _winningRow[index] = r;
        _winningCol[index] = c;
        index++;

        r -= dRow;
        c -= dCol;
      }

      return true;
    }
  }

  return false;
}

int Puissance4Game::countDirection(int row, int col, int dRow, int dCol, int player) const {
  int count = 0;

  int r = row + dRow;
  int c = col + dCol;

  while (
    r >= 0 && r < ROWS &&
    c >= 0 && c < COLS &&
    _board[r][c] == player
  ) {
    count++;

    r += dRow;
    c += dCol;
  }

  return count;
}

bool Puissance4Game::isBoardFull() const {
  for (int col = 0; col < COLS; col++) {
    if (_board[0][col] == 0) {
      return false;
    }
  }

  return true;
}

bool Puissance4Game::isWinningCell(int row, int col) const {
  for (int i = 0; i < 4; i++) {
    if (_winningRow[i] == row && _winningCol[i] == col) {
      return true;
    }
  }

  return false;
}

void Puissance4Game::drawColumnHint(int col) {
  (void)col;
}