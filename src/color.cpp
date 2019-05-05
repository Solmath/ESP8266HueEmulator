#include "color.h"

#include <NeoPixelBus.h>

RgbColor red = RgbColor(COLOR_SATURATION, 0, 0);
RgbColor green = RgbColor(0, COLOR_SATURATION, 0);
RgbColor blue = RgbColor(0, 0, COLOR_SATURATION);
RgbColor white = RgbColor(COLOR_SATURATION);
RgbColor black = RgbColor(0);

HsbColor getHsb(int hue, int sat, int bri) {
  float H, S, B;
  H = ((float)hue) / 182.04 / 360.0;
  S = ((float)sat) / COLOR_SATURATION;
  B = ((float)bri) / COLOR_SATURATION;
  return HsbColor(H, S, B);
}