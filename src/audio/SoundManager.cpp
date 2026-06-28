#include "SoundManager.h"
#include "../config/AppConfig.h"

using namespace ESPLAY;

void SoundManager::begin() {
  if (SOUND_ENABLED && SOUND_PIN >= 0) {
    pinMode(SOUND_PIN, OUTPUT);
  }
}

void SoundManager::playClick() {
  if (SOUND_ENABLED && SOUND_PIN >= 0) {
    tone(SOUND_PIN, 1200, 35);
  }
}

void SoundManager::playSequenceStep(int cellIndex) {
  if (SOUND_ENABLED && SOUND_PIN >= 0) {
    int freq = 440 + (cellIndex * 45);
    tone(SOUND_PIN, freq, 80);
  }
}

void SoundManager::playSuccess() {
  if (SOUND_ENABLED && SOUND_PIN >= 0) {
    tone(SOUND_PIN, 880, 90);
  }
}

void SoundManager::playError() {
  if (SOUND_ENABLED && SOUND_PIN >= 0) {
    tone(SOUND_PIN, 180, 180);
  }
}

void SoundManager::playGameOver() {
  if (SOUND_ENABLED && SOUND_PIN >= 0) {
    tone(SOUND_PIN, 220, 160);
  }
}