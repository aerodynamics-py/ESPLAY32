#pragma once

#include <Arduino.h>

// =========================================================
// ESPLAY32 - Global configuration
// =========================================================

namespace ESPLAY {

// ---------------------------------------------------------
// Display
// ---------------------------------------------------------

static const int SCREEN_W = 800;
static const int SCREEN_H = 480;

// Validated landscape rotation for the Elecrow 7" setup.
static const uint8_t DISPLAY_ROTATION = 2;

// ---------------------------------------------------------
// RGB565 helper
// ---------------------------------------------------------

constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
  return (uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3));
}

// ---------------------------------------------------------
// Colors - RGB565
// ---------------------------------------------------------

static const uint16_t COL_BLACK     = 0x0000;
static const uint16_t COL_WHITE     = 0xFFFF;
static const uint16_t COL_LIGHTGREY = 0xC618;
static const uint16_t COL_MIDGREY   = 0x8410;
static const uint16_t COL_DARKGREY  = 0x4208;
static const uint16_t COL_NAVY      = 0x000F;
static const uint16_t COL_DARKBLUE  = 0x0010;

static const uint16_t COL_RED       = 0xF800;
static const uint16_t COL_ORANGE    = 0xFD20;
static const uint16_t COL_YELLOW    = 0xFFE0;
static const uint16_t COL_GREEN     = 0x07E0;
static const uint16_t COL_CYAN      = 0x07FF;
static const uint16_t COL_BLUE      = 0x001F;
static const uint16_t COL_PURPLE    = 0x780F;
static const uint16_t COL_PINK      = 0xF81F;
static const uint16_t COL_LIME      = 0xAFE5;
static const uint16_t COL_TEAL      = 0x0438;
static const uint16_t COL_SKY       = 0x867D;
static const uint16_t COL_AMBER     = 0xFBE0;
static const uint16_t COL_VIOLET    = 0x801F;
static const uint16_t COL_MINT      = 0x87F0;
static const uint16_t COL_CORAL     = 0xFBEA;
static const uint16_t COL_GOLD      = 0xFEA0;

// ---------------------------------------------------------
// Touch
// ---------------------------------------------------------

static const uint32_t TOUCH_DEBOUNCE_MS = 180;

// ---------------------------------------------------------
// Sound
// ---------------------------------------------------------
//
// Disabled by default.
// Later, set SOUND_ENABLED = true and SOUND_PIN to a confirmed
// free GPIO if using a passive buzzer.
// ---------------------------------------------------------

static const bool SOUND_ENABLED = false;
static const int SOUND_PIN = -1;

}