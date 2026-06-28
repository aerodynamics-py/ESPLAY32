#include "DuelTargetGame.h"
#include "../config/AppConfig.h"
#include "../ui/UIHelpers.h"

using namespace ESPLAY;

const Rect DuelTargetGame::BACK_BUTTON   = { 650, 20, 120, 44 };
const Rect DuelTargetGame::START_BUTTON  = { 300, 380, 200, 58 };
const Rect DuelTargetGame::REPLAY_BUTTON = { 220, 390, 160, 58 };
const Rect DuelTargetGame::HOME_BUTTON   = { 420, 390, 160, 58 };

const char* DuelTargetGame::getName() const {
  return "DUEL TARGET";
}

const char* DuelTargetGame::getSubtitle() const {
  return "Reflexe";
}

uint16_t DuelTargetGame::getAccentColor() const {
  return rgb565(18, 52, 86);
}

void DuelTargetGame::begin(lgfx::LGFX_Device& lcd, SoundManager& sound) {
  _lcd = &lcd;
  _sound = &sound;
  _wantsHome = false;

  resetMatch();
  changePhase(PHASE_READY);
  drawReadyScreen();
}

void DuelTargetGame::update() {
  uint32_t now = millis();

  if (_phase == PHASE_WAIT_TARGET) {
    if (now >= _targetAppearTime) {
      showTarget();
    }
  }

  else if (_phase == PHASE_ROUND_RESULT) {
    if (now - _phaseTime >= ROUND_RESULT_MS) {
      if (_winnerPlayer > 0) {
        changePhase(PHASE_GAME_OVER);
        drawGameOverScreen();
      } else {
        startRound();
      }
    }
  }
}

void DuelTargetGame::handleTouch(int x, int y) {
  if (pointInRect(x, y, BACK_BUTTON)) {
    _sound->playClick();
    _wantsHome = true;
    return;
  }

  if (_phase == PHASE_READY) {
    if (pointInRect(x, y, START_BUTTON)) {
      _sound->playClick();
      startRound();
    }

    return;
  }

  if (_phase == PHASE_GAME_OVER) {
    if (pointInRect(x, y, REPLAY_BUTTON)) {
      _sound->playClick();
      resetMatch();
      changePhase(PHASE_READY);
      drawReadyScreen();
      return;
    }

    if (pointInRect(x, y, HOME_BUTTON)) {
      _sound->playClick();
      _wantsHome = true;
      return;
    }

    return;
  }

  if (_phase != PHASE_TARGET_VISIBLE) {
    return;
  }

  if (isInsideCircle(x, y, _targetLeftX, _targetY, _targetRadius)) {
    scorePoint(1);
    return;
  }

  if (isInsideCircle(x, y, _targetRightX, _targetY, _targetRadius)) {
    scorePoint(2);
    return;
  }
}

bool DuelTargetGame::wantsHome() const {
  return _wantsHome;
}

void DuelTargetGame::clearHomeRequest() {
  _wantsHome = false;
}

void DuelTargetGame::resetMatch() {
  _scoreP1 = 0;
  _scoreP2 = 0;
  _roundWinner = 0;
  _winnerPlayer = 0;

  _targetLeftX = 160;
  _targetRightX = 640;
  _targetY = 240;
  _targetRadius = 42;
}

void DuelTargetGame::startRound() {
  _roundWinner = 0;

  int leftMinX = 80;
  int leftMaxX = 320;

  _targetLeftX = random(leftMinX, leftMaxX + 1);
  _targetRightX = SCREEN_W - _targetLeftX;

  _targetY = random(145, 350);
  _targetRadius = random(34, 48);

  uint32_t delayMs = random(900, 2300);
  _targetAppearTime = millis() + delayMs;

  changePhase(PHASE_WAIT_TARGET);
  drawWaitingScreen();
}

void DuelTargetGame::showTarget() {
  changePhase(PHASE_TARGET_VISIBLE);
  drawTargetScreen();
}

void DuelTargetGame::scorePoint(int player) {
  _roundWinner = player;

  if (player == 1) {
    _scoreP1++;
  } else {
    _scoreP2++;
  }

  if (_scoreP1 >= TARGET_SCORE) {
    _winnerPlayer = 1;
  }

  if (_scoreP2 >= TARGET_SCORE) {
    _winnerPlayer = 2;
  }

  _sound->playSuccess();

  changePhase(PHASE_ROUND_RESULT);
  drawRoundResultScreen();
}

void DuelTargetGame::changePhase(Phase phase) {
  _phase = phase;
  _phaseTime = millis();
}

void DuelTargetGame::drawHeader(const char* subtitle) {
  _lcd->fillRect(0, 0, SCREEN_W, 86, COL_DARKGREY);

  _lcd->setTextDatum(TL_DATUM);

  _lcd->setTextColor(COL_WHITE, COL_DARKGREY);
  _lcd->setTextSize(3);
  _lcd->drawString("DUEL TARGET", 32, 18);

  _lcd->setTextColor(COL_LIGHTGREY, COL_DARKGREY);
  _lcd->setTextSize(1);
  _lcd->drawString(subtitle, 36, 58);

  UI::drawSmallButton(*_lcd, BACK_BUTTON, "ACCUEIL");
}

void DuelTargetGame::drawScores() {
  char p1[32];
  char p2[32];

  snprintf(p1, sizeof(p1), "J1  %d", _scoreP1);
  snprintf(p2, sizeof(p2), "J2  %d", _scoreP2);

  _lcd->setTextDatum(MC_DATUM);

  _lcd->setTextColor(COL_CYAN, COL_BLACK);
  _lcd->setTextSize(3);
  _lcd->drawString(p1, 170, 112);

  _lcd->setTextColor(COL_ORANGE, COL_BLACK);
  _lcd->setTextSize(3);
  _lcd->drawString(p2, 630, 112);

  _lcd->setTextColor(COL_LIGHTGREY, COL_BLACK);
  _lcd->setTextSize(1);
  _lcd->drawString("Premier a 5", SCREEN_W / 2, 114);
}

void DuelTargetGame::drawArenaBase() {
  _lcd->drawLine(SCREEN_W / 2, 96, SCREEN_W / 2, 410, COL_DARKGREY);

  _lcd->drawRoundRect(30, 135, 340, 260, 18, COL_CYAN);
  _lcd->drawRoundRect(430, 135, 340, 260, 18, COL_ORANGE);

  _lcd->setTextDatum(MC_DATUM);
  _lcd->setTextSize(1);

  _lcd->setTextColor(COL_CYAN, COL_BLACK);
  _lcd->drawString("JOUEUR 1", 200, 385);

  _lcd->setTextColor(COL_ORANGE, COL_BLACK);
  _lcd->drawString("JOUEUR 2", 600, 385);
}

void DuelTargetGame::drawTarget(int cx, int cy, uint16_t color) {
  _lcd->fillCircle(cx, cy, _targetRadius, rgb565(20, 20, 20));
  _lcd->drawCircle(cx, cy, _targetRadius, color);
  _lcd->drawCircle(cx, cy, _targetRadius - 12, color);
  _lcd->fillCircle(cx, cy, 10, color);
}

void DuelTargetGame::drawFooter(const char* text) {
  _lcd->fillRect(0, 426, SCREEN_W, 54, COL_BLACK);
  UI::drawCenteredText(*_lcd, text, SCREEN_W / 2, 450, 2, COL_WHITE, COL_BLACK);
}

void DuelTargetGame::drawReadyScreen() {
  _lcd->fillScreen(COL_BLACK);

  drawHeader("Jeu de reflexe a deux");
  drawScores();
  drawArenaBase();

  UI::drawCenteredText(
    *_lcd,
    "Chaque joueur touche sa cible. Le plus rapide marque.",
    SCREEN_W / 2,
    250,
    1,
    COL_LIGHTGREY,
    COL_BLACK
  );

  UI::drawButton(*_lcd, START_BUTTON, "START");
}

void DuelTargetGame::drawWaitingScreen() {
  _lcd->fillScreen(COL_BLACK);

  drawHeader("Attends la cible");
  drawScores();
  drawArenaBase();

  UI::drawCenteredText(
    *_lcd,
    "PRET...",
    SCREEN_W / 2,
    260,
    4,
    COL_WHITE,
    COL_BLACK
  );

  drawFooter("La cible va apparaitre");
}

void DuelTargetGame::drawTargetScreen() {
  _lcd->fillScreen(COL_BLACK);

  drawHeader("Touche ta cible");
  drawScores();
  drawArenaBase();

  drawTarget(_targetLeftX, _targetY, COL_CYAN);
  drawTarget(_targetRightX, _targetY, COL_ORANGE);

  drawFooter("GO !");
}

void DuelTargetGame::drawRoundResultScreen() {
  _lcd->fillScreen(COL_BLACK);

  drawHeader("Point marque");
  drawScores();
  drawArenaBase();

  char text[48];
  snprintf(text, sizeof(text), "Joueur %d marque", _roundWinner);

  uint16_t color = (_roundWinner == 1) ? COL_CYAN : COL_ORANGE;

  UI::drawCenteredText(
    *_lcd,
    text,
    SCREEN_W / 2,
    260,
    3,
    color,
    COL_BLACK
  );
}

void DuelTargetGame::drawGameOverScreen() {
  _lcd->fillScreen(COL_BLACK);

  UI::drawHeader(*_lcd, "DUEL TARGET", "Fin de partie");

  _lcd->setTextDatum(MC_DATUM);

  char title[48];
  snprintf(title, sizeof(title), "Joueur %d gagne", _winnerPlayer);

  _lcd->setTextColor(COL_WHITE, COL_BLACK);
  _lcd->setTextSize(3);
  _lcd->drawString(title, SCREEN_W / 2, 145);

  char score[48];
  snprintf(score, sizeof(score), "Score final : %d - %d", _scoreP1, _scoreP2);

  _lcd->setTextColor(COL_LIGHTGREY, COL_BLACK);
  _lcd->setTextSize(2);
  _lcd->drawString(score, SCREEN_W / 2, 215);

  UI::drawButton(*_lcd, REPLAY_BUTTON, "REJOUER");
  UI::drawButton(*_lcd, HOME_BUTTON, "ACCUEIL");
}

bool DuelTargetGame::isInsideCircle(int x, int y, int cx, int cy, int radius) const {
  long dx = x - cx;
  long dy = y - cy;

  return (dx * dx + dy * dy) <= (long)radius * (long)radius;
}