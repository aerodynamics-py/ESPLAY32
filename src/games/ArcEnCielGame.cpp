#include "ArcEnCielGame.h"
#include "../config/AppConfig.h"
#include "../ui/UIHelpers.h"

using namespace ESPLAY;

const Rect ArcEnCielGame::BACK_BUTTON   = { 650, 20, 120, 44 };
const Rect ArcEnCielGame::DIFF_3_BUTTON = {  60, 150, 200, 180 };
const Rect ArcEnCielGame::DIFF_4_BUTTON = { 300, 150, 200, 180 };
const Rect ArcEnCielGame::DIFF_5_BUTTON = { 540, 150, 200, 180 };
const Rect ArcEnCielGame::GO_BUTTON     = { 325, 432, 150, 42 };
const Rect ArcEnCielGame::REPLAY_BUTTON = { 220, 390, 160, 58 };
const Rect ArcEnCielGame::HOME_BUTTON   = { 420, 390, 160, 58 };

static const uint16_t ARC_COLOR_PALETTE[] = {
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

static const int ARC_COLOR_COUNT =
  sizeof(ARC_COLOR_PALETTE) / sizeof(ARC_COLOR_PALETTE[0]);

const char* ArcEnCielGame::getName() const {
  return "ARC-EN-CIEL";
}

const char* ArcEnCielGame::getSubtitle() const {
  return "Memoire";
}

uint16_t ArcEnCielGame::getAccentColor() const {
  return COL_PURPLE;
}

void ArcEnCielGame::begin(lgfx::LGFX_Device& lcd, SoundManager& sound) {
  _lcd = &lcd;
  _sound = &sound;
  _wantsHome = false;

  changePhase(PHASE_DIFFICULTY);
  drawDifficultyScreen();
}

void ArcEnCielGame::update() {
  uint32_t now = millis();

  if (_phase == PHASE_READY_GO) {
    return;
  }

  else if (_phase == PHASE_PRE_SHOW) {
    if (now - _phaseTime >= _preShowDelayMs) {
      _showIndex = 0;
      _showCellLit = false;

      drawGameFrame("Memorise la sequence", nullptr);
      changePhase(PHASE_SHOW_SEQUENCE);

      _phaseTime = millis() - SHOW_CELL_OFF_MS;
    }
  }

  else if (_phase == PHASE_SHOW_SEQUENCE) {
    updateShowSequence();
  }

  else if (_phase == PHASE_INPUT_FEEDBACK) {
    if (now - _phaseTime >= INPUT_FLASH_MS) {
      if (_lastTouchedCell >= 0) {
        drawCell(_lastTouchedCell, false);
      }

      _lastTouchedCell = -1;
      _lastTouchCleared = true;

      drawFooterWaitInput();
      changePhase(PHASE_WAIT_INPUT);
    }
  }

  else if (_phase == PHASE_SUCCESS_FEEDBACK) {
    if (now - _phaseTime >= INPUT_FLASH_MS) {
      if (_lastTouchedCell >= 0) {
        drawCell(_lastTouchedCell, false);
      }

      _lastTouchedCell = -1;
      _lastTouchCleared = true;

      _activePlayer = (_activePlayer == 1) ? 2 : 1;
      beginNextRound(false);
    }
  }

  else if (_phase == PHASE_ERROR_FEEDBACK) {
    if (now - _phaseTime >= INPUT_FLASH_MS) {
      if (_lastTouchedCell >= 0) {
        drawCell(_lastTouchedCell, false);
      }

      _lastTouchedCell = -1;
      _lastTouchCleared = true;

      changePhase(PHASE_GAME_OVER);
      drawGameOverScreen();
    }
  }
}

void ArcEnCielGame::handleTouch(int x, int y) {
  if (_phase == PHASE_DIFFICULTY) {
    if (pointInRect(x, y, BACK_BUTTON)) {
      _sound->playClick();
      _wantsHome = true;
      return;
    }

    if (pointInRect(x, y, DIFF_3_BUTTON)) {
      _sound->playClick();
      startGame(3);
      return;
    }

    if (pointInRect(x, y, DIFF_4_BUTTON)) {
      _sound->playClick();
      startGame(4);
      return;
    }

    if (pointInRect(x, y, DIFF_5_BUTTON)) {
      _sound->playClick();
      startGame(5);
      return;
    }

    return;
  }

  if (_phase == PHASE_GAME_OVER) {
    if (pointInRect(x, y, REPLAY_BUTTON)) {
      _sound->playClick();
      changePhase(PHASE_DIFFICULTY);
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

  if (_phase == PHASE_READY_GO) {
    if (pointInRect(x, y, GO_BUTTON)) {
      _sound->playClick();
      startSequenceDisplayAfterGo();
      return;
    }

    return;
  }

  if (_phase == PHASE_WAIT_INPUT) {
    int cell = getGridCellAt(x, y);

    if (cell >= 0) {
      handleCellInput(cell);
    }
  }
}

bool ArcEnCielGame::wantsHome() const {
  return _wantsHome;
}

void ArcEnCielGame::clearHomeRequest() {
  _wantsHome = false;
}

void ArcEnCielGame::changePhase(Phase newPhase) {
  _phase = newPhase;
  _phaseTime = millis();
}

void ArcEnCielGame::drawDifficultyScreen() {
  _lcd->fillScreen(COL_BLACK);

  UI::drawHeader(*_lcd, "ARC-EN-CIEL", "Choisis la difficulte");
  UI::drawSmallButton(*_lcd, BACK_BUTTON, "ACCUEIL");

  auto drawDifficultyCard = [&](const Rect& r, const char* title, const char* subtitle, const char* detail, uint16_t color) {
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

  drawDifficultyCard(DIFF_3_BUTTON, "3x3", "Facile", "9 cases", COL_NAVY);
  drawDifficultyCard(DIFF_4_BUTTON, "4x4", "Normal", "16 cases", COL_DARKBLUE);
  drawDifficultyCard(DIFF_5_BUTTON, "5x5", "Difficile", "25 cases", COL_PURPLE);

  UI::drawCenteredText(
    *_lcd,
    "Une sequence commune. Chaque reussite ajoute une case.",
    SCREEN_W / 2,
    430,
    1,
    COL_LIGHTGREY,
    COL_BLACK
  );
}

void ArcEnCielGame::startGame(int gridSize) {
  _gridSize = gridSize;
  _cellCount = _gridSize * _gridSize;

  computeGridLayout();
  randomizeCellColors();

  _sequenceLength = 0;
  _inputIndex = 0;
  _activePlayer = 1;
  _winnerPlayer = 0;
  _loserPlayer = 0;

  _showIndex = 0;
  _showCellLit = false;
  _lastTouchedCell = -1;
  _lastTouchCleared = true;

  beginNextRound(true);
}

void ArcEnCielGame::beginNextRound(bool waitForGo) {
  appendRandomCell();

  _inputIndex = 0;
  _showIndex = 0;
  _showCellLit = false;
  _lastTouchedCell = -1;
  _lastTouchCleared = true;

  if (waitForGo) {
    drawReadyScreen();
    changePhase(PHASE_READY_GO);
    return;
  }

  char footer1[64];
  char footer2[64];

  snprintf(footer1, sizeof(footer1), "Joueur %d", _activePlayer);
  snprintf(
    footer2,
    sizeof(footer2),
    "Sequence de %d case%s",
    _sequenceLength,
    (_sequenceLength > 1) ? "s" : ""
  );

  drawGameFrame(footer1, footer2);
  startSequenceDisplayAfterDelay(ROUND_PRE_SHOW_MS);
}

void ArcEnCielGame::appendRandomCell() {
  if (_sequenceLength >= MAX_SEQUENCE) {
    return;
  }

  _sequence[_sequenceLength] = random(0, _cellCount);
  _sequenceLength++;
}

void ArcEnCielGame::startSequenceDisplayAfterGo() {
  drawGameFrame("Memorise la sequence", nullptr);
  startSequenceDisplayAfterDelay(INITIAL_GO_DELAY_MS);
}

void ArcEnCielGame::startSequenceDisplayAfterDelay(uint32_t delayMs) {
  _showIndex = 0;
  _showCellLit = false;
  _preShowDelayMs = delayMs;

  changePhase(PHASE_PRE_SHOW);
}

void ArcEnCielGame::computeGridLayout() {
  const int gridAreaY = 96;
  const int gridAreaH = 316;
  const int gridAreaW = 650;

  _cellGap = (_gridSize >= 5) ? 8 : 12;

  int sizeByHeight = (gridAreaH - ((_gridSize - 1) * _cellGap)) / _gridSize;
  int sizeByWidth  = (gridAreaW - ((_gridSize - 1) * _cellGap)) / _gridSize;

  _cellSize = min(sizeByHeight, sizeByWidth);

  if (_cellSize > 96) {
    _cellSize = 96;
  }

  _gridW = _gridSize * _cellSize + (_gridSize - 1) * _cellGap;
  _gridH = _gridSize * _cellSize + (_gridSize - 1) * _cellGap;

  _gridX = (SCREEN_W - _gridW) / 2;
  _gridY = gridAreaY + (gridAreaH - _gridH) / 2;
}

Rect ArcEnCielGame::getCellRect(int cellIndex) const {
  int row = cellIndex / _gridSize;
  int col = cellIndex % _gridSize;

  Rect r;
  r.x = _gridX + col * (_cellSize + _cellGap);
  r.y = _gridY + row * (_cellSize + _cellGap);
  r.w = _cellSize;
  r.h = _cellSize;

  return r;
}

int ArcEnCielGame::getGridCellAt(int x, int y) const {
  for (int i = 0; i < _cellCount; i++) {
    Rect r = getCellRect(i);

    if (pointInRect(x, y, r)) {
      return i;
    }
  }

  return -1;
}

void ArcEnCielGame::eraseCellArea(int cellIndex) {
  if (cellIndex < 0 || cellIndex >= _cellCount) {
    return;
  }

  Rect r = getCellRect(cellIndex);

  _lcd->fillRoundRect(
    r.x - 7,
    r.y - 7,
    r.w + 14,
    r.h + 14,
    18,
    COL_BLACK
  );
}

void ArcEnCielGame::drawCell(int cellIndex, bool highlighted) {
  if (cellIndex < 0 || cellIndex >= _cellCount) {
    return;
  }

  Rect r = getCellRect(cellIndex);
  uint16_t color = _cellColors[cellIndex];

  eraseCellArea(cellIndex);

  if (highlighted) {
    _lcd->fillRoundRect(r.x - 5, r.y - 5, r.w + 10, r.h + 10, 16, COL_WHITE);
    _lcd->fillRoundRect(r.x + 5, r.y + 5, r.w - 10, r.h - 10, 12, color);
  } else {
    _lcd->fillRoundRect(r.x, r.y, r.w, r.h, 12, color);
    _lcd->drawRoundRect(r.x, r.y, r.w, r.h, 12, COL_BLACK);
  }
}

void ArcEnCielGame::drawGrid() {
  for (int i = 0; i < _cellCount; i++) {
    drawCell(i, false);
  }
}

void ArcEnCielGame::randomizeCellColors() {
  uint16_t shuffled[ARC_COLOR_COUNT];

  for (int i = 0; i < ARC_COLOR_COUNT; i++) {
    shuffled[i] = ARC_COLOR_PALETTE[i];
  }

  for (int i = ARC_COLOR_COUNT - 1; i > 0; i--) {
    int j = random(0, i + 1);

    uint16_t tmp = shuffled[i];
    shuffled[i] = shuffled[j];
    shuffled[j] = tmp;
  }

  for (int i = 0; i < _cellCount; i++) {
    _cellColors[i] = shuffled[i];
  }
}

void ArcEnCielGame::drawHeader() {
  _lcd->fillRect(0, 0, SCREEN_W, 86, COL_DARKGREY);

  _lcd->setTextDatum(TL_DATUM);

  _lcd->setTextColor(COL_WHITE, COL_DARKGREY);
  _lcd->setTextSize(3);
  _lcd->drawString("ARC-EN-CIEL", 32, 18);

  char playerText[32];
  snprintf(playerText, sizeof(playerText), "Joueur %d", _activePlayer);

  _lcd->setTextColor(COL_LIGHTGREY, COL_DARKGREY);
  _lcd->setTextSize(2);
  _lcd->drawString(playerText, 310, 22);

  char sequenceText[48];
  snprintf(sequenceText, sizeof(sequenceText), "%dx%d | Sequence %d", _gridSize, _gridSize, _sequenceLength);

  _lcd->setTextSize(1);
  _lcd->drawString(sequenceText, 314, 54);

  UI::drawSmallButton(*_lcd, BACK_BUTTON, "ACCUEIL");
}

void ArcEnCielGame::drawFooter(const char* line1, const char* line2) {
  _lcd->fillRect(0, 426, SCREEN_W, 54, COL_BLACK);

  UI::drawCenteredText(*_lcd, line1, SCREEN_W / 2, 444, 2, COL_WHITE, COL_BLACK);

  if (line2 != nullptr) {
    UI::drawCenteredText(*_lcd, line2, SCREEN_W / 2, 468, 1, COL_LIGHTGREY, COL_BLACK);
  }
}

void ArcEnCielGame::drawFooterWaitInput() {
  char line1[64];
  char line2[64];

  snprintf(line1, sizeof(line1), "Joueur %d : reproduis la sequence", _activePlayer);
  snprintf(line2, sizeof(line2), "Entree %d / %d", _inputIndex + 1, _sequenceLength);

  drawFooter(line1, line2);
}

void ArcEnCielGame::drawReadyScreen() {
  _lcd->fillScreen(COL_BLACK);

  drawHeader();
  drawGrid();

  _lcd->fillRect(0, 412, SCREEN_W, 68, COL_BLACK);

  char readyText[64];
  snprintf(readyText, sizeof(readyText), "Joueur %d : pret ?", _activePlayer);

  UI::drawCenteredText(*_lcd, readyText, SCREEN_W / 2, 421, 1, COL_LIGHTGREY, COL_BLACK);
  UI::drawButton(*_lcd, GO_BUTTON, "GO");
}

void ArcEnCielGame::drawGameFrame(const char* footer1, const char* footer2) {
  _lcd->fillScreen(COL_BLACK);
  drawHeader();
  drawGrid();

  if (footer1 != nullptr) {
    drawFooter(footer1, footer2);
  }
}

void ArcEnCielGame::drawSequencePreview(int x, int y, int w, int h) {
  if (_sequenceLength <= 0) {
    return;
  }

  const int gap = 4;
  const int maxCols = 32;

  int cols = _sequenceLength;
  if (cols > maxCols) {
    cols = maxCols;
  }

  int rows = (_sequenceLength + cols - 1) / cols;

  int cellSizeByWidth = (w - ((cols - 1) * gap)) / cols;
  int cellSizeByHeight = (h - ((rows - 1) * gap)) / rows;

  int previewSize = min(cellSizeByWidth, cellSizeByHeight);

  if (previewSize > 18) {
    previewSize = 18;
  }

  if (previewSize < 6) {
    previewSize = 6;
  }

  int totalW = cols * previewSize + (cols - 1) * gap;
  int totalH = rows * previewSize + (rows - 1) * gap;

  int startX = x + (w - totalW) / 2;
  int startY = y + (h - totalH) / 2;

  for (int i = 0; i < _sequenceLength; i++) {
    int row = i / cols;
    int col = i % cols;
    int cellIndex = _sequence[i];

    if (cellIndex < 0 || cellIndex >= _cellCount) {
      continue;
    }

    int sx = startX + col * (previewSize + gap);
    int sy = startY + row * (previewSize + gap);

    _lcd->fillRoundRect(sx, sy, previewSize, previewSize, 3, _cellColors[cellIndex]);
    _lcd->drawRoundRect(sx, sy, previewSize, previewSize, 3, COL_BLACK);
  }
}

void ArcEnCielGame::drawGameOverScreen() {
  _lcd->fillScreen(COL_BLACK);

  UI::drawHeader(*_lcd, "ARC-EN-CIEL", "Fin de partie");

  _lcd->setTextDatum(MC_DATUM);

  char winText[48];
  snprintf(winText, sizeof(winText), "Joueur %d gagne", _winnerPlayer);

  _lcd->setTextColor(COL_WHITE, COL_BLACK);
  _lcd->setTextSize(3);
  _lcd->drawString(winText, SCREEN_W / 2, 125);

  char loseText[64];
  snprintf(loseText, sizeof(loseText), "Joueur %d s'est trompe", _loserPlayer);

  _lcd->setTextColor(COL_LIGHTGREY, COL_BLACK);
  _lcd->setTextSize(2);
  _lcd->drawString(loseText, SCREEN_W / 2, 175);

  char scoreText[80];
  snprintf(scoreText, sizeof(scoreText), "Longueur atteinte : %d | Difficulte : %dx%d", _sequenceLength, _gridSize, _gridSize);

  _lcd->setTextSize(1);
  _lcd->drawString(scoreText, SCREEN_W / 2, 218);

  _lcd->setTextColor(COL_WHITE, COL_BLACK);
  _lcd->setTextSize(1);
  _lcd->drawString("Sequence :", SCREEN_W / 2, 252);

  drawSequencePreview(60, 270, 680, 90);

  UI::drawButton(*_lcd, REPLAY_BUTTON, "REJOUER");
  UI::drawButton(*_lcd, HOME_BUTTON, "ACCUEIL");
}

void ArcEnCielGame::updateShowSequence() {
  uint32_t now = millis();

  if (_showIndex >= _sequenceLength) {
    drawGameFrame(nullptr, nullptr);
    drawFooterWaitInput();

    changePhase(PHASE_WAIT_INPUT);
    return;
  }

  if (!_showCellLit && (now - _phaseTime >= SHOW_CELL_OFF_MS)) {
    int cell = _sequence[_showIndex];

    drawCell(cell, true);
    _sound->playSequenceStep(cell);

    _showCellLit = true;
    _phaseTime = now;
    return;
  }

  if (_showCellLit && (now - _phaseTime >= SHOW_CELL_ON_MS)) {
    int cell = _sequence[_showIndex];

    drawCell(cell, false);

    _showCellLit = false;
    _showIndex++;
    _phaseTime = now;
  }
}

void ArcEnCielGame::handleCellInput(int cellIndex) {
  if (_phase != PHASE_WAIT_INPUT) {
    return;
  }

  _lastTouchedCell = cellIndex;
  _lastTouchCleared = false;

  drawCell(cellIndex, true);
  _sound->playClick();

  int expected = _sequence[_inputIndex];

  if (cellIndex != expected) {
    _loserPlayer = _activePlayer;
    _winnerPlayer = (_activePlayer == 1) ? 2 : 1;

    _sound->playError();
    _sound->playGameOver();

    changePhase(PHASE_ERROR_FEEDBACK);
    return;
  }

  _inputIndex++;

  if (_inputIndex >= _sequenceLength) {
    _sound->playSuccess();
    changePhase(PHASE_SUCCESS_FEEDBACK);
    return;
  }

  changePhase(PHASE_INPUT_FEEDBACK);
}