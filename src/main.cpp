#include <Arduino.h>
#include <esp_system.h>

#include "LGFX_Config.h"

#include "config/AppConfig.h"
#include "audio/SoundManager.h"
#include "core/GameManager.h"

#include "games/ArcEnCielGame.h"
#include "games/MorpionGame.h"
#include "games/BombColorGame.h"
#include "games/DuelTargetGame.h"
#include "games/Puissance4Game.h"
#include "games/CircuitPathGame.h"

// =========================================================
// ESPLAY32 - Multi-game platform
// Games:
// - ARC-EN-CIEL
// - MORPION
// - BOMB COLOR
// - DUEL TARGET
// - PUISSANCE 4
// - CIRCUIT PATH
// =========================================================

// ---------------------------------------------------------
// Global instances
// ---------------------------------------------------------

LGFX lcd;

SoundManager sound;
GameManager gameManager;

ArcEnCielGame arcEnCielGame;
MorpionGame morpionGame;
BombColorGame bombColorGame;
DuelTargetGame duelTargetGame;
Puissance4Game puissance4Game;
CircuitPathGame circuitPathGame;

// ---------------------------------------------------------
// Touch handling
// ---------------------------------------------------------

bool wasTouching = false;
uint32_t lastTouchTime = 0;

bool getTouchPressed(int& x, int& y) {
  uint16_t tx = 0;
  uint16_t ty = 0;

  bool touching = lcd.getTouch(&tx, &ty);
  uint32_t now = millis();

  if (
    touching &&
    !wasTouching &&
    (now - lastTouchTime > ESPLAY::TOUCH_DEBOUNCE_MS)
  ) {
    wasTouching = true;
    lastTouchTime = now;

    x = tx;
    y = ty;

    return true;
  }

  if (!touching) {
    wasTouching = false;
  }

  return false;
}

// ---------------------------------------------------------
// Display setup
// ---------------------------------------------------------

void setupDisplay() {
  lcd.init();

  lcd.setRotation(ESPLAY::DISPLAY_ROTATION);
  lcd.setBrightness(180);

  Serial.print("[DISPLAY] rotation=");
  Serial.print(ESPLAY::DISPLAY_ROTATION);
  Serial.print(" width=");
  Serial.print(lcd.width());
  Serial.print(" height=");
  Serial.println(lcd.height());

  if (
    lcd.width() != ESPLAY::SCREEN_W ||
    lcd.height() != ESPLAY::SCREEN_H
  ) {
    Serial.println("[DISPLAY][WARNING] Expected strict 800x480.");
    Serial.println("[DISPLAY][WARNING] Keep DISPLAY_ROTATION = 2.");
  }
}

// ---------------------------------------------------------
// Setup
// ---------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(300);

  Serial.println();
  Serial.println("=================================");
  Serial.println("ESPLAY32 - Multi-game platform");
  Serial.println("Strict landscape 800x480 UI");
  Serial.println("=================================");

  randomSeed((uint32_t)esp_random());

  setupDisplay();

  sound.begin();

  gameManager.begin(lcd, sound);

  gameManager.registerGame(&arcEnCielGame);
  gameManager.registerGame(&morpionGame);
  gameManager.registerGame(&bombColorGame);
  gameManager.registerGame(&duelTargetGame);
  gameManager.registerGame(&puissance4Game);
  gameManager.registerGame(&circuitPathGame);

  gameManager.showHome();
}

// ---------------------------------------------------------
// Main loop
// ---------------------------------------------------------

void loop() {
  gameManager.update();

  int x = 0;
  int y = 0;

  if (getTouchPressed(x, y)) {
    Serial.print("[TOUCH] x=");
    Serial.print(x);
    Serial.print(" y=");
    Serial.println(y);

    gameManager.handleTouch(x, y);
  }

  delay(5);
}