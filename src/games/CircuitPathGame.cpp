#include "CircuitPathGame.h"
#include "../config/AppConfig.h"
#include "../ui/UIHelpers.h"

using namespace ESPLAY;

// ---------------------------------------------------------
// Buttons
// ---------------------------------------------------------

const Rect CircuitPathGame::BACK_BUTTON   = { 650, 20, 120, 44 };
const Rect CircuitPathGame::RESET_BUTTON  = { 510, 20, 120, 44 };

const Rect CircuitPathGame::GRID_4_BUTTON = {  60, 130, 200, 88 };
const Rect CircuitPathGame::GRID_5_BUTTON = { 300, 130, 200, 88 };
const Rect CircuitPathGame::GRID_6_BUTTON = { 540, 130, 200, 88 };

const Rect CircuitPathGame::SHORT_BUTTON  = {  60, 255, 200, 88 };
const Rect CircuitPathGame::MEDIUM_BUTTON = { 300, 255, 200, 88 };
const Rect CircuitPathGame::LONG_BUTTON   = { 540, 255, 200, 88 };

const Rect CircuitPathGame::START_BUTTON  = { 300, 385, 200, 58 };

const Rect CircuitPathGame::NEW_BUTTON    = { 140, 426, 160, 48 };
const Rect CircuitPathGame::CONFIG_BUTTON = { 320, 426, 160, 48 };
const Rect CircuitPathGame::HOME_BUTTON   = { 500, 426, 160, 48 };

// ---------------------------------------------------------
// Game identity
// ---------------------------------------------------------

const char* CircuitPathGame::getName() const {
  return "CIRCUIT PATH";
}

const char* CircuitPathGame::getSubtitle() const {
  return "Puzzle solo";
}

uint16_t CircuitPathGame::getAccentColor() const {
  return rgb565(8, 80, 52);
}

// ---------------------------------------------------------
// Game API
// ---------------------------------------------------------

void CircuitPathGame::begin(lgfx::LGFX_Device& lcd, SoundManager& sound) {
  _lcd = &lcd;
  _sound = &sound;
  _wantsHome = false;

  _selectedGridOption = 1;
  _selectedLengthMode = LENGTH_MEDIUM;

  changePhase(PHASE_CONFIG);
  drawConfigScreen();
}

void CircuitPathGame::update() {
}

void CircuitPathGame::handleTouch(int x, int y) {
  if (pointInRect(x, y, BACK_BUTTON)) {
    _sound->playClick();
    _wantsHome = true;
    return;
  }

  if (_phase == PHASE_CONFIG) {
    if (pointInRect(x, y, GRID_4_BUTTON)) {
      _selectedGridOption = 0;
      _sound->playClick();
      drawConfigScreen();
      return;
    }

    if (pointInRect(x, y, GRID_5_BUTTON)) {
      _selectedGridOption = 1;
      _sound->playClick();
      drawConfigScreen();
      return;
    }

    if (pointInRect(x, y, GRID_6_BUTTON)) {
      _selectedGridOption = 2;
      _sound->playClick();
      drawConfigScreen();
      return;
    }

    if (pointInRect(x, y, SHORT_BUTTON)) {
      _selectedLengthMode = LENGTH_SHORT;
      _sound->playClick();
      drawConfigScreen();
      return;
    }

    if (pointInRect(x, y, MEDIUM_BUTTON)) {
      _selectedLengthMode = LENGTH_MEDIUM;
      _sound->playClick();
      drawConfigScreen();
      return;
    }

    if (pointInRect(x, y, LONG_BUTTON)) {
      _selectedLengthMode = LENGTH_LONG;
      _sound->playClick();
      drawConfigScreen();
      return;
    }

    if (pointInRect(x, y, START_BUTTON)) {
      _sound->playClick();
      generateNewPuzzle();
      changePhase(PHASE_PLAYING);
      drawScreen();
      return;
    }

    return;
  }

  if (_phase == PHASE_LEVEL_COMPLETE) {
    if (pointInRect(x, y, NEW_BUTTON)) {
      _sound->playClick();
      generateNewPuzzle();
      changePhase(PHASE_PLAYING);
      drawScreen();
      return;
    }

    if (pointInRect(x, y, CONFIG_BUTTON)) {
      _sound->playClick();
      changePhase(PHASE_CONFIG);
      drawConfigScreen();
      return;
    }

    if (pointInRect(x, y, HOME_BUTTON)) {
      _sound->playClick();
      _wantsHome = true;
      return;
    }

    return;
  }

  if (_phase == PHASE_PLAYING) {
    if (pointInRect(x, y, RESET_BUTTON)) {
      _sound->playClick();
      resetGeneratedPuzzle();
      return;
    }

    int tile = getTileAt(x, y);

    if (tile >= 0 && isRotatable(tile)) {
      rotateTile(tile);

      _connected = checkConnected();

      drawScreen();

      if (_connected) {
        _sound->playSuccess();
        changePhase(PHASE_LEVEL_COMPLETE);
        drawLevelCompleteScreen();
      }

      return;
    }
  }
}

bool CircuitPathGame::wantsHome() const {
  return _wantsHome;
}

void CircuitPathGame::clearHomeRequest() {
  _wantsHome = false;
}

// ---------------------------------------------------------
// Configuration screen
// ---------------------------------------------------------

void CircuitPathGame::drawConfigScreen() {
  _lcd->fillScreen(COL_BLACK);

  UI::drawHeader(
    *_lcd,
    "CIRCUIT PATH",
    "Configuration du puzzle"
  );

  UI::drawSmallButton(*_lcd, BACK_BUTTON, "ACCUEIL");

  _lcd->setTextDatum(MC_DATUM);
  _lcd->setTextColor(COL_LIGHTGREY, COL_BLACK);
  _lcd->setTextSize(1);
  _lcd->drawString("Taille de la grille", SCREEN_W / 2, 105);

  drawConfigCard(GRID_4_BUTTON, "4x4", "Compact", _selectedGridOption == 0);
  drawConfigCard(GRID_5_BUTTON, "5x5", "Classique", _selectedGridOption == 1);
  drawConfigCard(GRID_6_BUTTON, "6x5", "Large", _selectedGridOption == 2);

  _lcd->setTextColor(COL_LIGHTGREY, COL_BLACK);
  _lcd->setTextSize(1);
  _lcd->drawString("Longueur du reseau", SCREEN_W / 2, 235);

  drawConfigCard(SHORT_BUTTON, "COURT", "Rapide", _selectedLengthMode == LENGTH_SHORT);
  drawConfigCard(MEDIUM_BUTTON, "MOYEN", "Equilibre", _selectedLengthMode == LENGTH_MEDIUM);
  drawConfigCard(LONG_BUTTON, "LONG", "Sinueux", _selectedLengthMode == LENGTH_LONG);

  UI::drawButton(*_lcd, START_BUTTON, "GENERER");
}

void CircuitPathGame::drawConfigCard(
  const Rect& r,
  const char* title,
  const char* subtitle,
  bool selected
) {
  uint16_t bg = selected ? rgb565(20, 105, 70) : rgb565(20, 40, 32);
  uint16_t border = selected ? COL_YELLOW : COL_LIGHTGREY;

  _lcd->fillRoundRect(r.x, r.y, r.w, r.h, 16, bg);
  _lcd->drawRoundRect(r.x, r.y, r.w, r.h, 16, border);

  if (selected) {
    _lcd->drawRoundRect(r.x + 3, r.y + 3, r.w - 6, r.h - 6, 13, border);
  }

  _lcd->setTextDatum(MC_DATUM);

  _lcd->setTextColor(COL_WHITE, bg);
  _lcd->setTextSize(3);
  _lcd->drawString(title, r.x + r.w / 2, r.y + 34);

  _lcd->setTextColor(COL_LIGHTGREY, bg);
  _lcd->setTextSize(1);
  _lcd->drawString(subtitle, r.x + r.w / 2, r.y + 66);
}

void CircuitPathGame::applySelectedGrid() {
  if (_selectedGridOption == 0) {
    _cols = 4;
    _rows = 4;
  }

  else if (_selectedGridOption == 1) {
    _cols = 5;
    _rows = 5;
  }

  else {
    _cols = 6;
    _rows = 5;
  }

  _cellCount = _cols * _rows;
  computeGridLayout();
}

// ---------------------------------------------------------
// Procedural generation
// ---------------------------------------------------------

void CircuitPathGame::generateNewPuzzle() {
  applySelectedGrid();
  computeMinPathLength();

  clearAllGenerationData();

  bool success = generateSolutionPath();

  if (!success) {
    buildSnakeFallbackPath();
  }

  buildConnectionsFromPath();
  addDecoyNetworks();
  convertConnectionsToTiles();

  scrambleRotations();
  saveInitialBoard();

  _moves = 0;
  _connected = checkConnected();

  int attempts = 0;

  while (_connected && attempts < 30) {
    scrambleRotations();
    saveInitialBoard();
    _moves = 0;
    _connected = checkConnected();
    attempts++;
  }

  Serial.print("[CIRCUIT] Generated ");
  Serial.print(_cols);
  Serial.print("x");
  Serial.print(_rows);
  Serial.print(" path=");
  Serial.print(_pathLength);
  Serial.print(" occupied=");
  Serial.println(countOccupiedCells());
}

void CircuitPathGame::clearAllGenerationData() {
  for (int i = 0; i < MAX_CELLS; i++) {
    _type[i] = TILE_EMPTY;
    _rotation[i] = 0;

    _initialType[i] = TILE_EMPTY;
    _initialRotation[i] = 0;

    _visited[i] = false;
    _onSolutionPath[i] = false;
    _path[i] = -1;

    for (int d = 0; d < 4; d++) {
      _connections[i][d] = false;
    }
  }

  _pathLength = 0;
  _sourceIndex = -1;
  _lampIndex = -1;
  _moves = 0;
  _connected = false;
}

void CircuitPathGame::clearTilesAndConnections() {
  for (int i = 0; i < MAX_CELLS; i++) {
    _type[i] = TILE_EMPTY;
    _rotation[i] = 0;

    for (int d = 0; d < 4; d++) {
      _connections[i][d] = false;
    }
  }
}

void CircuitPathGame::computeMinPathLength() {
  if (_cols == 4 && _rows == 4) {
    if (_selectedLengthMode == LENGTH_SHORT)  _minPathLength = 6;
    if (_selectedLengthMode == LENGTH_MEDIUM) _minPathLength = 9;
    if (_selectedLengthMode == LENGTH_LONG)   _minPathLength = 12;
    return;
  }

  if (_cols == 5 && _rows == 5) {
    if (_selectedLengthMode == LENGTH_SHORT)  _minPathLength = 8;
    if (_selectedLengthMode == LENGTH_MEDIUM) _minPathLength = 13;
    if (_selectedLengthMode == LENGTH_LONG)   _minPathLength = 18;
    return;
  }

  if (_selectedLengthMode == LENGTH_SHORT)  _minPathLength = 10;
  if (_selectedLengthMode == LENGTH_MEDIUM) _minPathLength = 16;
  if (_selectedLengthMode == LENGTH_LONG)   _minPathLength = 22;
}

bool CircuitPathGame::generateSolutionPath() {
  int originalMin = _minPathLength;

  for (int relaxedMin = originalMin; relaxedMin >= 5; relaxedMin--) {
    _minPathLength = relaxedMin;

    for (int attempt = 0; attempt < 90; attempt++) {
      if (attemptGeneratePath()) {
        return true;
      }
    }
  }

  _minPathLength = originalMin;
  return false;
}

bool CircuitPathGame::attemptGeneratePath() {
  for (int i = 0; i < MAX_CELLS; i++) {
    _visited[i] = false;
    _onSolutionPath[i] = false;
    _path[i] = -1;
  }

  int sourceRow = random(0, _rows);
  int lampRow = random(0, _rows);

  _sourceIndex = sourceRow * _cols;
  _lampIndex = lampRow * _cols + (_cols - 1);

  _path[0] = _sourceIndex;
  _visited[_sourceIndex] = true;
  _pathLength = 1;
  _dfsBudget = 2500;

  return dfsBuildPath(_sourceIndex, 1);
}

bool CircuitPathGame::dfsBuildPath(int current, int length) {
  _dfsBudget--;

  if (_dfsBudget <= 0) {
    return false;
  }

  if (current == _lampIndex) {
    if (length >= _minPathLength) {
      _pathLength = length;

      for (int i = 0; i < _pathLength; i++) {
        _onSolutionPath[_path[i]] = true;
      }

      return true;
    }

    return false;
  }

  if (length >= _cellCount) {
    return false;
  }

  int dirs[4] = { DIR_N, DIR_E, DIR_S, DIR_W };
  shuffleDirections(dirs);

  for (int i = 0; i < 4; i++) {
    int dir = dirs[i];
    int next = neighborIndex(current, dir);

    if (next < 0) {
      continue;
    }

    if (_visited[next]) {
      continue;
    }

    if (next == _lampIndex && length + 1 < _minPathLength) {
      continue;
    }

    _visited[next] = true;
    _path[length] = next;

    if (dfsBuildPath(next, length + 1)) {
      return true;
    }

    _visited[next] = false;
    _path[length] = -1;
  }

  return false;
}

void CircuitPathGame::buildSnakeFallbackPath() {
  for (int i = 0; i < MAX_CELLS; i++) {
    _path[i] = -1;
    _onSolutionPath[i] = false;
  }

  int index = 0;

  for (int row = 0; row < _rows; row++) {
    if (row % 2 == 0) {
      for (int col = 0; col < _cols; col++) {
        _path[index++] = row * _cols + col;
      }
    } else {
      for (int col = _cols - 1; col >= 0; col--) {
        _path[index++] = row * _cols + col;
      }
    }
  }

  _pathLength = index;
  _sourceIndex = _path[0];
  _lampIndex = _path[_pathLength - 1];

  for (int i = 0; i < _pathLength; i++) {
    _onSolutionPath[_path[i]] = true;
  }
}

void CircuitPathGame::buildConnectionsFromPath() {
  for (int i = 0; i < _pathLength - 1; i++) {
    addConnectionBetween(_path[i], _path[i + 1]);
  }
}

void CircuitPathGame::addDecoyNetworks() {
  int target = targetOccupiedCells();
  int guard = 0;

  while (countOccupiedCells() < target && guard < 220) {
    guard++;

    bool attachToNetwork = random(0, 100) < 65;

    int start = -1;

    if (attachToNetwork) {
      start = pickRandomOccupiedCell(false);
    } else {
      start = findRandomEmptyCell();
    }

    if (start < 0) {
      continue;
    }

    int walkLength = 2;

    if (_selectedLengthMode == LENGTH_SHORT) {
      walkLength = random(2, 4);
    }

    else if (_selectedLengthMode == LENGTH_MEDIUM) {
      walkLength = random(3, 6);
    }

    else {
      walkLength = random(4, 8);
    }

    addDecoyWalk(start, walkLength);
  }
}

bool CircuitPathGame::addDecoyWalk(int startIndex, int maxSteps) {
  int current = startIndex;
  bool added = false;

  for (int step = 0; step < maxSteps; step++) {
    int dirs[4] = { DIR_N, DIR_E, DIR_S, DIR_W };
    shuffleDirections(dirs);

    bool moved = false;

    for (int i = 0; i < 4; i++) {
      int dir = dirs[i];
      int next = neighborIndex(current, dir);

      if (next < 0) {
        continue;
      }

      if (next == _sourceIndex || next == _lampIndex) {
        continue;
      }

      int opposite = oppositeDir(dir);

      if (_connections[current][dir] || _connections[next][opposite]) {
        continue;
      }

      if (_onSolutionPath[next] && hasAnyConnection(next)) {
        continue;
      }

      addConnectionBetween(current, next);

      current = next;
      moved = true;
      added = true;

      break;
    }

    if (!moved) {
      break;
    }
  }

  return added;
}

int CircuitPathGame::findRandomEmptyCell() const {
  for (int attempt = 0; attempt < 80; attempt++) {
    int index = random(0, _cellCount);

    if (index == _sourceIndex || index == _lampIndex) {
      continue;
    }

    if (!hasAnyConnection(index)) {
      return index;
    }
  }

  for (int i = 0; i < _cellCount; i++) {
    if (i != _sourceIndex && i != _lampIndex && !hasAnyConnection(i)) {
      return i;
    }
  }

  return -1;
}

int CircuitPathGame::pickRandomOccupiedCell(bool allowEndpoints) const {
  for (int attempt = 0; attempt < 80; attempt++) {
    int index = random(0, _cellCount);

    if (!allowEndpoints && (index == _sourceIndex || index == _lampIndex)) {
      continue;
    }

    if (hasAnyConnection(index)) {
      return index;
    }
  }

  for (int i = 0; i < _cellCount; i++) {
    if (!allowEndpoints && (i == _sourceIndex || i == _lampIndex)) {
      continue;
    }

    if (hasAnyConnection(i)) {
      return i;
    }
  }

  return -1;
}

int CircuitPathGame::countOccupiedCells() const {
  int count = 0;

  for (int i = 0; i < _cellCount; i++) {
    if (hasAnyConnection(i) || i == _sourceIndex || i == _lampIndex) {
      count++;
    }
  }

  return count;
}

int CircuitPathGame::targetOccupiedCells() const {
  int percent = 65;

  if (_selectedLengthMode == LENGTH_MEDIUM) {
    percent = 80;
  }

  else if (_selectedLengthMode == LENGTH_LONG) {
    percent = 92;
  }

  int target = (_cellCount * percent) / 100;

  if (target < _pathLength + 3) {
    target = _pathLength + 3;
  }

  if (target > _cellCount) {
    target = _cellCount;
  }

  return target;
}

void CircuitPathGame::convertConnectionsToTiles() {
  for (int i = 0; i < _cellCount; i++) {
    setTileFromConnections(i);
  }
}

void CircuitPathGame::setTileFromConnections(int index) {
  bool conns[4];

  for (int d = 0; d < 4; d++) {
    conns[d] = _connections[index][d];
  }

  int count = 0;

  for (int d = 0; d < 4; d++) {
    if (conns[d]) {
      count++;
    }
  }

  if (index == _sourceIndex) {
    _type[index] = TILE_SOURCE;

    for (int d = 0; d < 4; d++) {
      if (conns[d]) {
        _rotation[index] = d;
        return;
      }
    }

    _rotation[index] = DIR_E;
    return;
  }

  if (index == _lampIndex) {
    _type[index] = TILE_LAMP;

    for (int d = 0; d < 4; d++) {
      if (conns[d]) {
        _rotation[index] = d;
        return;
      }
    }

    _rotation[index] = DIR_W;
    return;
  }

  if (count == 0) {
    _type[index] = TILE_EMPTY;
    _rotation[index] = 0;
    return;
  }

  if (count == 1) {
    _type[index] = TILE_END;

    for (int d = 0; d < 4; d++) {
      if (conns[d]) {
        _rotation[index] = d;
        return;
      }
    }
  }

  if (count == 2) {
    if (conns[DIR_N] && conns[DIR_S]) {
      _type[index] = TILE_STRAIGHT;
      _rotation[index] = 0;
      return;
    }

    if (conns[DIR_E] && conns[DIR_W]) {
      _type[index] = TILE_STRAIGHT;
      _rotation[index] = 1;
      return;
    }

    _type[index] = TILE_ELBOW;

    if (conns[DIR_N] && conns[DIR_E]) {
      _rotation[index] = 0;
    }

    else if (conns[DIR_E] && conns[DIR_S]) {
      _rotation[index] = 1;
    }

    else if (conns[DIR_S] && conns[DIR_W]) {
      _rotation[index] = 2;
    }

    else {
      _rotation[index] = 3;
    }

    return;
  }

  if (count == 3) {
    _type[index] = TILE_TEE;

    if (!conns[DIR_S]) _rotation[index] = 0;
    else if (!conns[DIR_W]) _rotation[index] = 1;
    else if (!conns[DIR_N]) _rotation[index] = 2;
    else _rotation[index] = 3;

    return;
  }

  _type[index] = TILE_CROSS;
  _rotation[index] = 0;
}

void CircuitPathGame::scrambleRotations() {
  for (int i = 0; i < _cellCount; i++) {
    if (!isRotatable(i)) {
      continue;
    }

    uint8_t add = random(1, 4);
    _rotation[i] = (_rotation[i] + add) % 4;
  }
}

void CircuitPathGame::saveInitialBoard() {
  for (int i = 0; i < MAX_CELLS; i++) {
    _initialType[i] = _type[i];
    _initialRotation[i] = _rotation[i];
  }
}

void CircuitPathGame::resetGeneratedPuzzle() {
  for (int i = 0; i < MAX_CELLS; i++) {
    _type[i] = _initialType[i];
    _rotation[i] = _initialRotation[i];
  }

  _moves = 0;
  _connected = checkConnected();

  changePhase(PHASE_PLAYING);
  drawScreen();
}

void CircuitPathGame::addConnectionBetween(int a, int b) {
  if (a < 0 || b < 0 || a >= _cellCount || b >= _cellCount) {
    return;
  }

  int dirAB = directionBetween(a, b);
  int dirBA = oppositeDir(dirAB);

  _connections[a][dirAB] = true;
  _connections[b][dirBA] = true;
}

bool CircuitPathGame::hasAnyConnection(int index) const {
  if (index < 0 || index >= _cellCount) {
    return false;
  }

  for (int d = 0; d < 4; d++) {
    if (_connections[index][d]) {
      return true;
    }
  }

  return false;
}

int CircuitPathGame::directionBetween(int fromIndex, int toIndex) const {
  int fromRow = fromIndex / _cols;
  int fromCol = fromIndex % _cols;

  int toRow = toIndex / _cols;
  int toCol = toIndex % _cols;

  if (toRow < fromRow) return DIR_N;
  if (toCol > fromCol) return DIR_E;
  if (toRow > fromRow) return DIR_S;

  return DIR_W;
}

void CircuitPathGame::shuffleDirections(int dirs[4]) {
  for (int i = 3; i > 0; i--) {
    int j = random(0, i + 1);

    int tmp = dirs[i];
    dirs[i] = dirs[j];
    dirs[j] = tmp;
  }
}

// ---------------------------------------------------------
// Drawing
// ---------------------------------------------------------

void CircuitPathGame::drawScreen() {
  _lcd->fillScreen(COL_BLACK);

  drawHeader();
  drawGrid();

  if (_connected) {
    drawFooter("Circuit connecte");
  } else {
    drawFooter("Relie le depart a l'arrivee");
  }
}

void CircuitPathGame::drawHeader() {
  _lcd->fillRect(0, 0, SCREEN_W, 86, COL_DARKGREY);

  _lcd->setTextDatum(TL_DATUM);

  _lcd->setTextColor(COL_WHITE, COL_DARKGREY);
  _lcd->setTextSize(3);
  _lcd->drawString("CIRCUIT PATH", 32, 18);

  const char* lenText = "Court";

  if (_selectedLengthMode == LENGTH_MEDIUM) {
    lenText = "Moyen";
  }

  else if (_selectedLengthMode == LENGTH_LONG) {
    lenText = "Long";
  }

  char info[96];
  snprintf(
    info,
    sizeof(info),
    "%dx%d | Reseau %s | Chemin %d | Coups %d",
    _cols,
    _rows,
    lenText,
    _pathLength,
    _moves
  );

  _lcd->setTextColor(COL_LIGHTGREY, COL_DARKGREY);
  _lcd->setTextSize(1);
  _lcd->drawString(info, 36, 58);

  if (_phase == PHASE_PLAYING) {
    UI::drawSmallButton(*_lcd, RESET_BUTTON, "RESET");
  }

  UI::drawSmallButton(*_lcd, BACK_BUTTON, "ACCUEIL");
}

void CircuitPathGame::computeGridLayout() {
  const int areaX = 70;
  const int areaY = 96;
  const int areaW = 660;
  const int areaH = 320;

  _cellGap = 8;

  int sizeByWidth = (areaW - ((_cols - 1) * _cellGap)) / _cols;
  int sizeByHeight = (areaH - ((_rows - 1) * _cellGap)) / _rows;

  _cellSize = min(sizeByWidth, sizeByHeight);

  if (_cellSize > 72) {
    _cellSize = 72;
  }

  _gridW = _cols * _cellSize + (_cols - 1) * _cellGap;
  _gridH = _rows * _cellSize + (_rows - 1) * _cellGap;

  _gridX = areaX + (areaW - _gridW) / 2;
  _gridY = areaY + (areaH - _gridH) / 2;
}

void CircuitPathGame::drawGrid() {
  _lcd->fillRoundRect(
    _gridX - 12,
    _gridY - 12,
    _gridW + 24,
    _gridH + 24,
    18,
    rgb565(8, 55, 38)
  );

  _lcd->drawRoundRect(
    _gridX - 12,
    _gridY - 12,
    _gridW + 24,
    _gridH + 24,
    18,
    COL_LIGHTGREY
  );

  for (int i = 0; i < _cellCount; i++) {
    drawTile(i);
  }
}

void CircuitPathGame::drawTile(int index) {
  Rect r = getTileRect(index);

  uint8_t type = _type[index];
  uint8_t rot = _rotation[index];

  uint16_t tileBg = rgb565(16, 32, 26);
  uint16_t border = rgb565(70, 120, 95);
  uint16_t wire = _connected ? rgb565(255, 220, 60) : rgb565(90, 255, 170);

  if (type == TILE_EMPTY) {
    _lcd->fillRoundRect(r.x, r.y, r.w, r.h, 10, COL_BLACK);
    _lcd->drawRoundRect(r.x, r.y, r.w, r.h, 10, rgb565(22, 55, 40));
    return;
  }

  _lcd->fillRoundRect(r.x, r.y, r.w, r.h, 10, tileBg);
  _lcd->drawRoundRect(r.x, r.y, r.w, r.h, 10, border);

  for (int dir = 0; dir < 4; dir++) {
    if (hasConnection(type, rot, dir)) {
      drawWireSegment(r, dir, wire);
    }
  }

  _lcd->fillCircle(
    r.x + r.w / 2,
    r.y + r.h / 2,
    7,
    wire
  );

  if (type == TILE_SOURCE) {
    drawSourceIcon(r);
  }

  if (type == TILE_LAMP) {
    drawLampIcon(r, _connected);
  }
}

void CircuitPathGame::drawFooter(const char* text) {
  _lcd->fillRect(0, 426, SCREEN_W, 54, COL_BLACK);

  UI::drawCenteredText(
    *_lcd,
    text,
    SCREEN_W / 2,
    450,
    2,
    COL_WHITE,
    COL_BLACK
  );
}

void CircuitPathGame::drawLevelCompleteScreen() {
  _lcd->fillScreen(COL_BLACK);

  drawHeader();
  drawGrid();

  _lcd->fillRect(0, 404, SCREEN_W, 76, COL_BLACK);

  char line[80];
  snprintf(
    line,
    sizeof(line),
    "Resolu en %d coups",
    _moves
  );

  UI::drawCenteredText(
    *_lcd,
    line,
    SCREEN_W / 2,
    412,
    1,
    COL_YELLOW,
    COL_BLACK
  );

  UI::drawButton(*_lcd, NEW_BUTTON, "NOUVEAU");
  UI::drawButton(*_lcd, CONFIG_BUTTON, "CONFIG");
  UI::drawButton(*_lcd, HOME_BUTTON, "ACCUEIL");
}

// ---------------------------------------------------------
// Grid helpers
// ---------------------------------------------------------

Rect CircuitPathGame::getTileRect(int index) const {
  int row = index / _cols;
  int col = index % _cols;

  Rect r;
  r.x = _gridX + col * (_cellSize + _cellGap);
  r.y = _gridY + row * (_cellSize + _cellGap);
  r.w = _cellSize;
  r.h = _cellSize;

  return r;
}

int CircuitPathGame::getTileAt(int x, int y) const {
  for (int i = 0; i < _cellCount; i++) {
    Rect r = getTileRect(i);

    if (pointInRect(x, y, r)) {
      return i;
    }
  }

  return -1;
}

bool CircuitPathGame::isRotatable(int index) const {
  if (index < 0 || index >= _cellCount) {
    return false;
  }

  if (_type[index] == TILE_EMPTY) {
    return false;
  }

  if (_type[index] == TILE_SOURCE) {
    return false;
  }

  if (_type[index] == TILE_LAMP) {
    return false;
  }

  return true;
}

void CircuitPathGame::rotateTile(int index) {
  _rotation[index] = (_rotation[index] + 1) % 4;
  _moves++;

  _sound->playClick();
}

// ---------------------------------------------------------
// Connection logic
// ---------------------------------------------------------

bool CircuitPathGame::checkConnected() {
  if (_sourceIndex < 0 || _lampIndex < 0) {
    return false;
  }

  bool visited[MAX_CELLS];

  for (int i = 0; i < MAX_CELLS; i++) {
    visited[i] = false;
  }

  int queue[MAX_CELLS];
  int head = 0;
  int tail = 0;

  queue[tail++] = _sourceIndex;
  visited[_sourceIndex] = true;

  while (head < tail) {
    int current = queue[head++];

    if (current == _lampIndex) {
      return true;
    }

    uint8_t currentType = _type[current];
    uint8_t currentRot = _rotation[current];

    for (int dir = 0; dir < 4; dir++) {
      if (!hasConnection(currentType, currentRot, dir)) {
        continue;
      }

      int next = neighborIndex(current, dir);

      if (next < 0) {
        continue;
      }

      if (visited[next]) {
        continue;
      }

      uint8_t nextType = _type[next];
      uint8_t nextRot = _rotation[next];

      if (!hasConnection(nextType, nextRot, oppositeDir(dir))) {
        continue;
      }

      visited[next] = true;
      queue[tail++] = next;
    }
  }

  return false;
}

bool CircuitPathGame::hasConnection(uint8_t type, uint8_t rotation, int dir) const {
  rotation = rotation % 4;

  if (type == TILE_EMPTY) {
    return false;
  }

  if (type == TILE_CROSS) {
    return true;
  }

  if (type == TILE_END) {
    return dir == rotation;
  }

  if (type == TILE_SOURCE || type == TILE_LAMP) {
    return dir == rotation;
  }

  if (type == TILE_STRAIGHT) {
    if (rotation == 0 || rotation == 2) {
      return dir == DIR_N || dir == DIR_S;
    }

    return dir == DIR_E || dir == DIR_W;
  }

  if (type == TILE_ELBOW) {
    if (rotation == 0) return dir == DIR_N || dir == DIR_E;
    if (rotation == 1) return dir == DIR_E || dir == DIR_S;
    if (rotation == 2) return dir == DIR_S || dir == DIR_W;
    if (rotation == 3) return dir == DIR_W || dir == DIR_N;
  }

  if (type == TILE_TEE) {
    if (rotation == 0) return dir == DIR_N || dir == DIR_E || dir == DIR_W;
    if (rotation == 1) return dir == DIR_N || dir == DIR_E || dir == DIR_S;
    if (rotation == 2) return dir == DIR_E || dir == DIR_S || dir == DIR_W;
    if (rotation == 3) return dir == DIR_N || dir == DIR_S || dir == DIR_W;
  }

  return false;
}

int CircuitPathGame::neighborIndex(int index, int dir) const {
  int row = index / _cols;
  int col = index % _cols;

  if (dir == DIR_N) row--;
  if (dir == DIR_E) col++;
  if (dir == DIR_S) row++;
  if (dir == DIR_W) col--;

  if (row < 0 || row >= _rows || col < 0 || col >= _cols) {
    return -1;
  }

  return row * _cols + col;
}

int CircuitPathGame::oppositeDir(int dir) const {
  if (dir == DIR_N) return DIR_S;
  if (dir == DIR_E) return DIR_W;
  if (dir == DIR_S) return DIR_N;
  return DIR_E;
}

// ---------------------------------------------------------
// Circuit drawing helpers
// ---------------------------------------------------------

void CircuitPathGame::drawWireSegment(const Rect& r, int dir, uint16_t color) {
  const int thickness = 8;

  int cx = r.x + r.w / 2;
  int cy = r.y + r.h / 2;

  if (dir == DIR_N) {
    _lcd->fillRect(cx - thickness / 2, r.y + 6, thickness, cy - (r.y + 6), color);
  }

  else if (dir == DIR_E) {
    _lcd->fillRect(cx, cy - thickness / 2, (r.x + r.w - 6) - cx, thickness, color);
  }

  else if (dir == DIR_S) {
    _lcd->fillRect(cx - thickness / 2, cy, thickness, (r.y + r.h - 6) - cy, color);
  }

  else if (dir == DIR_W) {
    _lcd->fillRect(r.x + 6, cy - thickness / 2, cx - (r.x + 6), thickness, color);
  }
}

void CircuitPathGame::drawSourceIcon(const Rect& r) {
  int cx = r.x + r.w / 2;
  int cy = r.y + r.h / 2;

  _lcd->fillCircle(cx, cy, 19, rgb565(20, 130, 70));
  _lcd->drawCircle(cx, cy, 21, COL_WHITE);
  _lcd->drawCircle(cx, cy, 25, rgb565(90, 255, 170));

  _lcd->setTextDatum(MC_DATUM);
  _lcd->setTextColor(COL_WHITE, rgb565(20, 130, 70));
  _lcd->setTextSize(2);
  _lcd->drawString("D", cx, cy);
}

void CircuitPathGame::drawLampIcon(const Rect& r, bool lit) {
  int cx = r.x + r.w / 2;
  int cy = r.y + r.h / 2;

  uint16_t fill = lit ? COL_YELLOW : rgb565(90, 70, 20);
  uint16_t text = lit ? COL_BLACK : COL_LIGHTGREY;

  _lcd->fillCircle(cx, cy, 19, fill);
  _lcd->drawCircle(cx, cy, 21, COL_WHITE);

  if (lit) {
    _lcd->drawCircle(cx, cy, 26, COL_YELLOW);
    _lcd->drawCircle(cx, cy, 31, COL_YELLOW);
  }

  _lcd->setTextDatum(MC_DATUM);
  _lcd->setTextColor(text, fill);
  _lcd->setTextSize(2);
  _lcd->drawString("A", cx, cy);
}

// ---------------------------------------------------------
// Phase helper
// ---------------------------------------------------------

void CircuitPathGame::changePhase(Phase phase) {
  _phase = phase;
}