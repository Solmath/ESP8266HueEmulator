#pragma once

#include "LightService.h"
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

// Settings for the NeoPixels
#define NUM_PIXELS_PER_LIGHT 10 // How many physical LEDs per emulated bulb

class PixelHandler : public LightHandler{
  private:
    HueLightInfo _info;
    NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812xMethod> _strip;
    NeoPixelAnimator _animator;
    int16_t colorloopIndex = -1;
  public:
    PixelHandler(NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812xMethod> strip, NeoPixelAnimator animator);

    void handleQuery(int lightNumber, HueLightInfo newInfo, aJsonObject* raw); 
    HueLightInfo getInfo(int lightNumber);
};