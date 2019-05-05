#include "PixelHandler.h"
#include "LightService.h"

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include "color.h"

PixelHandler::PixelHandler(NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812xMethod> *strip, NeoPixelAnimator *animator):
  _strip(*strip),
  _animator(*animator)
{
}

void PixelHandler::handleQuery(int lightNumber, HueLightInfo newInfo, aJsonObject* raw) {
  // define the effect to apply, in this case linear blend
  HslColor newColor = HslColor(getHsb(newInfo.hue, newInfo.saturation, newInfo.brightness));
  HslColor originalColor = _strip.GetPixelColor(lightNumber);
  _info = newInfo;

  // cancel colorloop if one is running
  if (colorloopIndex >= 0) {
    _animator.StopAnimation(colorloopIndex);
    colorloopIndex = -1;
  }
  if (newInfo.on) {
    if (_info.effect == EFFECT_COLORLOOP) {
      //color loop at max brightness/saturation on a 60 second cycle
      const int SIXTY_SECONDS = 60000;
      _animator.StartAnimation(lightNumber, SIXTY_SECONDS, [ = ](const AnimationParam & param) {
        // save off animation index
        colorloopIndex = param.index;

        // progress will start at 0.0 and end at 1.0
        float currentHue = newColor.H + param.progress;
        if (currentHue > 1) currentHue -= 1;
        HslColor updatedColor = HslColor(currentHue, newColor.S, newColor.L);
        RgbColor currentColor = updatedColor;

        for(int i=lightNumber * NUM_PIXELS_PER_LIGHT; i < (lightNumber * NUM_PIXELS_PER_LIGHT) + NUM_PIXELS_PER_LIGHT; i++) {
          _strip.SetPixelColor(i, updatedColor);
        }

        // loop the animation until canceled
        if (param.state == AnimationState_Completed) {
          // done, time to restart this position tracking animation/timer
          _animator.RestartAnimation(param.index);
        }
      });
      return;
    }
    AnimUpdateCallback animUpdate = [ = ](const AnimationParam & param)
    {
      // progress will start at 0.0 and end at 1.0
      HslColor updatedColor = HslColor::LinearBlend<NeoHueBlendShortestDistance>(originalColor, newColor, param.progress);

      for(int i=lightNumber * NUM_PIXELS_PER_LIGHT; i < (lightNumber * NUM_PIXELS_PER_LIGHT) + NUM_PIXELS_PER_LIGHT; i++) {
        _strip.SetPixelColor(i, updatedColor);
      }
    };
    _animator.StartAnimation(lightNumber, _info.transitionTime, animUpdate);
  }
  else {
    AnimUpdateCallback animUpdate = [ = ](const AnimationParam & param)
    {
      // progress will start at 0.0 and end at 1.0
      HslColor updatedColor = HslColor::LinearBlend<NeoHueBlendShortestDistance>(originalColor, black, param.progress);
      
      for(int i=lightNumber * NUM_PIXELS_PER_LIGHT; i < (lightNumber * NUM_PIXELS_PER_LIGHT) + NUM_PIXELS_PER_LIGHT; i++) {
        _strip.SetPixelColor(i, updatedColor);
      }
    };
    _animator.StartAnimation(lightNumber, _info.transitionTime, animUpdate);
  }
}

HueLightInfo PixelHandler::getInfo(int lightNumber) {
  return _info;
}