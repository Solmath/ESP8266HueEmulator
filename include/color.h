#ifndef COLOR_H
#define COLOR_H

#include <NeoPixelBus.h>

#define COLOR_SATURATION 255.0f

HsbColor getHsb(int hue, int sat, int bri);

extern RgbColor red;
extern RgbColor green;
extern RgbColor blue;
extern RgbColor white;
extern RgbColor black;

struct rgbcolor {
  rgbcolor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {};
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)
struct hsvcolor {
  hsvcolor(const rgbcolor& color) {
    float r = ((float)color.r)/COLOR_SATURATION;
    float g = ((float)color.g)/COLOR_SATURATION;
    float b = ((float)color.b)/COLOR_SATURATION;
    float mi = MIN(MIN(r,g),b);
    float ma = MAX(MAX(r,g),b);
    float diff = ma - mi;
    v = ma;
    h = 0;
    s = (!v)?0:(diff/ma);
    if (diff) {
      if (r == v) {
            h = (g - b) / diff + (g < b ? 6.0f : 0.0f);
        } else if (g == v) {
            h = (b - r) / diff + 2.0f;
        } else {
            h = (r - g) / diff + 4.0f;
        }
        h /= 6.0f;
    }
  };
  float h;
  float s;
  float v;
};

#endif // COLOR_H