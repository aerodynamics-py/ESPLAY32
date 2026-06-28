#include "UIHelpers.h"
#include "../config/AppConfig.h"

using namespace ESPLAY;

namespace UI {

void drawHeader(
  lgfx::LGFX_Device& lcd,
  const char* title,
  const char* subtitle
) {
  lcd.fillRect(0, 0, SCREEN_W, 86, COL_DARKGREY);

  lcd.setTextDatum(TL_DATUM);

  lcd.setTextColor(COL_WHITE, COL_DARKGREY);
  lcd.setTextSize(3);
  lcd.drawString(title, 32, 20);

  lcd.setTextColor(COL_LIGHTGREY, COL_DARKGREY);
  lcd.setTextSize(1);
  lcd.drawString(subtitle, 36, 58);
}

void drawButton(
  lgfx::LGFX_Device& lcd,
  const Rect& r,
  const char* label
) {
  lcd.fillRoundRect(r.x, r.y, r.w, r.h, 12, COL_DARKGREY);
  lcd.drawRoundRect(r.x, r.y, r.w, r.h, 12, COL_WHITE);

  lcd.setTextDatum(MC_DATUM);
  lcd.setTextColor(COL_WHITE, COL_DARKGREY);
  lcd.setTextSize(2);
  lcd.drawString(label, r.x + r.w / 2, r.y + r.h / 2);
}

void drawSmallButton(
  lgfx::LGFX_Device& lcd,
  const Rect& r,
  const char* label
) {
  lcd.fillRoundRect(r.x, r.y, r.w, r.h, 10, COL_DARKGREY);
  lcd.drawRoundRect(r.x, r.y, r.w, r.h, 10, COL_LIGHTGREY);

  lcd.setTextDatum(MC_DATUM);
  lcd.setTextColor(COL_WHITE, COL_DARKGREY);
  lcd.setTextSize(1);
  lcd.drawString(label, r.x + r.w / 2, r.y + r.h / 2);
}

void drawCenteredText(
  lgfx::LGFX_Device& lcd,
  const char* text,
  int x,
  int y,
  int textSize,
  uint16_t color,
  uint16_t bg
) {
  lcd.setTextDatum(MC_DATUM);
  lcd.setTextColor(color, bg);
  lcd.setTextSize(textSize);
  lcd.drawString(text, x, y);
}

}