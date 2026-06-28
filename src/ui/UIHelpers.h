#pragma once

#include <LovyanGFX.hpp>
#include "../core/Types.h"

// =========================================================
// ESPLAY32 - Common UI helpers
// =========================================================

namespace UI {

void drawHeader(
  lgfx::LGFX_Device& lcd,
  const char* title,
  const char* subtitle
);

void drawButton(
  lgfx::LGFX_Device& lcd,
  const Rect& r,
  const char* label
);

void drawSmallButton(
  lgfx::LGFX_Device& lcd,
  const Rect& r,
  const char* label
);

void drawCenteredText(
  lgfx::LGFX_Device& lcd,
  const char* text,
  int x,
  int y,
  int textSize,
  uint16_t color,
  uint16_t bg
);

}