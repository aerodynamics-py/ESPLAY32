#include "BombColorGame.h"
#include "../config/AppConfig.h"
#include "../ui/UIHelpers.h"

using namespace ESPLAY;

const Rect BombColorGame::BACK_BUTTON   = { 650, 20, 120, 44 };
const Rect BombColorGame::DIFF_4_BUTTON = { 160, 150, 220, 180 };
const Rect BombColorGame::DIFF_5_BUTTON = { 420, 150, 220, 180 };
const Rect BombColorGame::REPLAY_BUTTON = { 220, 390, 160, 58 };
const Rect BombColorGame::HOME_BUTTON   = { 420, 390, 160, 58 };

static const uint16_t BOMB_COLOR_PALETTE[] = {
  rgb565(230,  57,  70), rgb565(255, 127,  17), rgb565(255, 190,  11),
  rgb565(255, 235,  59), rgb565(190, 230,  45), rgb565( 76, 175,  80),
  rgb565(  0, 150, 136), rgb565(  0, 188, 212), rgb565(  3, 169, 244),
  rgb565( 33, 150, 243), rgb565( 63,  81, 181), rgb565(103,  58, 183),
  rgb565(156,  39, 176), rgb565(233,  30,  99), rgb565(255,  64, 129),
  rgb565(255, 112,  67), rgb565(121,  85,  72), rgb565(158, 158, 158),
  rgb565( 96, 125, 139), rgb565(174, 213, 129), rgb565(128, 222, 234),
  rgb565(144, 202, 249), rgb565(206, 147, 216), rgb565(244, 143, 177),
  rgb565(255, 204, 128)
};

static const int BOMB_COLOR_COUNT =
  sizeof(BOMB_COLOR_PALETTE) / sizeof(BOMB_COLOR_PALETTE[0]);

const char* BombColorGame::getName() const {
  return "BOMB COLOR";
}

const char* BombColorGame::getSubtitle() const {
  return "Pieges caches";
}

uint16_t BombColorGame::getAccentColor() const {
  return COL_RED;
}

void BombColorGame::begin(lgfx::LGFX_Device& lcd, SoundManager& sound) {
  _lcd = &lcd;
  _sound = &sound;
  _wantsHome = false;

  _phase = PHASE_DIFFICULTY;
  drawDifficultyScreen();
}

void BombColorGame::update() {
}

void BombColorGame::handleTouch(int x, int y) {
  if (_phase == PHASE_DIFFICULTY) {
    if (pointInRect(x, y, BACK_BUTTON)) {
      _sound->playClick();
      _wantsHome = true;
      return;
    }

    if (pointInRect(x, y, DIFF_4_BUTTON)) {
      _sound->playClick();
      startGame(4, 1);
      return;
    }

    if (pointInRect(x, y, DIFF_5_BUTTON)) {
      _sound->playClick();
      startGame(5, 2);
      return;
    }

    return;
  }

  if (_phase == PHASE_GAME_OVER) {
    if (pointInRect(x, y, REPLAY_BUTTON)) {
      _sound->playClick();
      _phase = PHASE_DIFFICULTY;
      drawDifficultyScreen();
      return;
    }

    if (pointInRect(x, y, HOME_BUTTON)) {
      _sound->playClick();
      _wantsHome = true;
      return;
    }

    return;
  }

  if (pointInRect(x, y, BACK_BUTTON)) {
    _sound->playClick();
    _wantsHome = true;
    return;
  }

  int cell = getGridCellAt(x, y);

  if (cell >= 0) {
    handleCellTouch(cell);
  }
}

bool BombColorGame::wantsHome() const {
  return _wantsHome;
}

void BombColorGame::clearHomeRequest() {
  _wantsHome = false;
}

void BombColorGame::drawDifficultyScreen() {
  _lcd->fillScreen(COL_BLACK);

  UI::drawHeader(*_lcd, "BOMB COLOR", "Choisis la difficulte");
  UI::drawSmallButton(*_lcd, BACK_BUTTON, "ACCUEIL");

  auto drawCard = [&](const Rect& r, const char* title, const char* subtitle, const char* detail, uint16_t color) {
    _lcd->fillRoundRect(r.x, r.y, r.w, r.h, 18, color);
    _lcd->drawRoundRect(r.x, r.y, r.w, r.h, 18, COL_WHITE);

    _lcd->setTextDatum(MC_DATUM);
    _lcd->setTextColor(COL_WHITE, color);

    _lcd->setTextSize(4);
    _lcd->drawString(title, r.x + r.w / 2, r.y + 55);

    _lcd->setTextSize(2);
    _lcd->drawString(subtitle, r.x + r.w / 2, r.y + 105);

    _lcd->setTextColor(COL_LIGHTGREY, color);
    _lcd->setTextSize(1);
    _lcd->drawString(detail, r.x + r.w / 2, r.y + 145);
  };

  drawCard(DIFF_4_BUTTON, "4x4", "1 bombe", "Simple et rapide", COL_DARKBLUE);
  drawCard(DIFF_5_BUTTON, "5x5", "2 bombes", "Plus de tension", COL_RED);

  UI::drawCenteredText(
    *_lcd,
    "Chaque joueur choisit une case. Celui qui trouve une bombe perd.",
    SCREEN_W / 2,
    430,
    1,
    COL_LIGHTGREY,
    COL_BLACK
  );
}

void BombColorGame::startGame(int gridSize, int bombCount) {
  _gridSize = gridSize;
  _cellCount = _gridSize * _gridSize;
  _bombCount = bombCount;
  _safeRevealed = 0;

  _activePlayer = 1;
  _winnerPlayer = 0;
  _loserPlayer = 0;
  _allSafeWin = false;

  computeGridLayout();
  resetBoard();
  randomizeColors();
  placeBombs();

  _phase = PHASE_PLAYING;
  drawGameScreen();
}

void BombColorGame::resetBoard() {
  for (int i = 0; i < MAX_CELLS; i++) {
    _bombs[i] = false;
    _revealed[i] = false;
  }
}

void BombColorGame::placeBombs() {
  int placed = 0;

  while (placed < _bombCount) {
    int cell = random(0, _cellCount);

    if (!_bombs[cell]) {
      _bombs[cell] = true;
      placed++;
    }
  }
}

void BombColorGame::randomizeColors() {
  uint16_t shuffled[BOMB_COLOR_COUNT];

  for (int i = 0; i < BOMB_COLOR_COUNT; i++) {
    shuffled[i] = BOMB_COLOR_PALETTE[i];
  }

  for (int i = BOMB_COLOR_COUNT - 1; i > 0; i--) {
    int j = random(0, i + 1);

    uint16_t tmp = shuffled[i];
    shuffled[i] = shuffled[j];
    shuffled[j] = tmp;
  }

  for (int i = 0; i < _cellCount; i++) {
    _cellColors[i] = shuffled[i];
  }
}

void BombColorGame::computeGridLayout() {
  const int gridAreaY = 100;
  const int gridAreaH = 310;
  const int gridAreaW = 600;

  _cellGap = (_gridSize >= 5) ? 8 : 12;

  int sizeByHeight = (gridAreaH - ((_gridSize - 1) * _cellGap)) / _gridSize;
  int sizeByWidth  = (gridAreaW - ((_gridSize - 1) * _cellGap)) / _gridSize;

  _cellSize = min(sizeByHeight, sizeByWidth);

  if (_cellSize > 76) {
    _cellSize = 76;
  }

  _gridW = _gridSize * _cellSize + (_gridSize - 1) * _cellGap;
  _gridH = _gridSize * _cellSize + (_gridSize - 1) * _cellGap;

  _gridX = (SCREEN_W - _gridW) / 2;
  _gridY = gridAreaY + (gridAreaH - _gridH) / 2;
}

Rect BombColorGame::getCellRect(int cellIndex) const {
  int row = cellIndex / _gridSize;
  int col = cellIndex % _gridSize;

  Rect r;
  r.x = _gridX + col * (_cellSize + _cellGap);
  r.y = _gridY + row * (_cellSize + _cellGap);
  r.w = _cellSize;
  r.h = _cellSize;

  return r;
}

int BombColorGame::getGridCellAt(int x, int y) const {
  for (int i = 0; i < _cellCount; i++) {
    Rect r = getCellRect(i);

    if (pointInRect(x, y, r)) {
      return i;
    }
  }

  return -1;
}

void BombColorGame::drawGameScreen() {
  _lcd->fillScreen(COL_BLACK);

  drawHeader();
  drawGrid();
  drawFooter();
}

void BombColorGame::drawHeader() {
  _lcd->fillRect(0, 0, SCREEN_W, 86, COL_DARKGREY);

  _lcd->setTextDatum(TL_DATUM);

  _lcd->setTextColor(COL_WHITE, COL_DARKGREY);
  _lcd->setTextSize(3);
  _lcd->drawString("BOMB COLOR", 32, 18);

  char playerText[32];
  snprintf(playerText, sizeof(playerText), "Joueur %d", _activePlayer);

  _lcd->setTextColor(COL_LIGHTGREY, COL_DARKGREY);
  _lcd->setTextSize(2);
  _lcd->drawString(playerText, 330, 24);

  char infoText[48];
  snprintf(infoText, sizeof(infoText), "%dx%d | Bombes : %d", _gridSize, _gridSize, _bombCount);

  _lcd->setTextSize(1);
  _lcd->drawString(infoText, 334, 56);

  UI::drawSmallButton(*_lcd, BACK_BUTTON, "ACCUEIL");
}

void BombColorGame::drawGrid() {
  for (int i = 0; i < _cellCount; i++) {
    drawCell(i);
  }
}

void BombColorGame::drawCell(int cellIndex) {
  Rect r = getCellRect(cellIndex);

  if (!_revealed[cellIndex]) {
    _lcd->fillRoundRect(r.x, r.y, r.w, r.h, 12, COL_DARKGREY);
    _lcd->drawRoundRect(r.x, r.y, r.w, r.h, 12, COL_LIGHTGREY);

    _lcd->setTextDatum(MC_DATUM);
    _lcd->setTextColor(COL_LIGHTGREY, COL_DARKGREY);
    _lcd->setTextSize(2);
    _lcd->drawString("?", r.x + r.w / 2, r.y + r.h / 2);

    return;
  }

  if (_bombs[cellIndex]) {
    _lcd->fillRoundRect(r.x, r.y, r.w, r.h, 12, COL_RED);
    _lcd->drawRoundRect(r.x, r.y, r.w, r.h, 12, COL_WHITE);

    _lcd->setTextDatum(MC_DATUM);
    _lcd->setTextColor(COL_BLACK, COL_RED);
    _lcd->setTextSize(3);
    _lcd->drawString("B", r.x + r.w / 2, r.y + r.h / 2);

    return;
  }

  _lcd->fillRoundRect(r.x, r.y, r.w, r.h, 12, _cellColors[cellIndex]);
  _lcd->drawRoundRect(r.x, r.y, r.w, r.h, 12, COL_BLACK);
}

void BombColorGame::drawFooter() {
  _lcd->fillRect(0, 426, SCREEN_W, 54, COL_BLACK);

  char line[64];
  snprintf(line, sizeof(line), "Joueur %d : choisis une case", _activePlayer);

  UI::drawCenteredText(*_lcd, line, SCREEN_W / 2, 450, 2, COL_WHITE, COL_BLACK);
}

void BombColorGame::handleCellTouch(int cellIndex) {
  if (cellIndex < 0 || cellIndex >= _cellCount) {
    return;
  }

  if (_revealed[cellIndex]) {
    return;
  }

  _sound->playClick();

  _revealed[cellIndex] = true;

  if (_bombs[cellIndex]) {
    _loserPlayer = _activePlayer;
    _winnerPlayer = (_activePlayer == 1) ? 2 : 1;
    _allSafeWin = false;

    _sound->playError();
    _sound->playGameOver();

    revealAllBombs();
    _phase = PHASE_GAME_OVER;
    drawGameOverScreen();
    return;
  }

  _safeRevealed++;

  if (_safeRevealed >= _cellCount - _bombCount) {
    _winnerPlayer = _activePlayer;
    _loserPlayer = 0;
    _allSafeWin = true;

    _sound->playSuccess();

    _phase = PHASE_GAME_OVER;
    drawGameOverScreen();
    return;
  }

  _activePlayer = (_activePlayer == 1) ? 2 : 1;
  drawGameScreen();
}

void BombColorGame::revealAllBombs() {
  for (int i = 0; i < _cellCount; i++) {
    if (_bombs[i]) {
      _revealed[i] = true;
    }
  }
}

void BombColorGame::drawGameOverScreen() {
  _lcd->fillScreen(COL_BLACK);

  UI::drawHeader(*_lcd, "BOMB COLOR", "Fin de partie");

  _lcd->setTextDatum(MC_DATUM);

  char title[64];

  if (_allSafeWin) {
    snprintf(title, sizeof(title), "Joueur %d gagne", _winnerPlayer);
  } else {
    snprintf(title, sizeof(title), "Joueur %d explose", _loserPlayer);
  }

  _lcd->setTextColor(COL_WHITE, COL_BLACK);
  _lcd->setTextSize(3);
  _lcd->drawString(title, SCREEN_W / 2, 125);

  char subtitle[80];

  if (_allSafeWin) {
    snprintf(subtitle, sizeof(subtitle), "Toutes les cases sures ont ete trouvees");
  } else {
    snprintf(subtitle, sizeof(subtitle), "Joueur %d gagne", _winnerPlayer);
  }

  _lcd->setTextColor(COL_LIGHTGREY, COL_BLACK);
  _lcd->setTextSize(2);
  _lcd->drawString(subtitle, SCREEN_W / 2, 180);

  char info[80];
  snprintf(info, sizeof(info), "Difficulte : %dx%d | Bombes : %d", _gridSize, _gridSize, _bombCount);

  _lcd->setTextSize(1);
  _lcd->drawString(info, SCREEN_W / 2, 230);

  // Small revealed board preview
  int oldGridX = _gridX;
  int oldGridY = _gridY;
  int oldCellSize = _cellSize;
  int oldCellGap = _cellGap;

  _cellSize = 28;
  _cellGap = 5;
  _gridW = _gridSize * _cellSize + (_gridSize - 1) * _cellGap;
  _gridH = _gridSize * _cellSize + (_gridSize - 1) * _cellGap;
  _gridX = (SCREEN_W - _gridW) / 2;
  _gridY = 260;

  for (int i = 0; i < _cellCount; i++) {
    _revealed[i] = true;
    drawCell(i);
  }

  _gridX = oldGridX;
  _gridY = oldGridY;
  _cellSize = oldCellSize;
  _cellGap = oldCellGap;

  UI::drawButton(*_lcd, REPLAY_BUTTON, "REJOUER");
  UI::drawButton(*_lcd, HOME_BUTTON, "ACCUEIL");
}