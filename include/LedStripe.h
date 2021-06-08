#ifndef LedStripe_h
#define LedStripe_h

#include <Arduino.h>
#include "Adafruit_NeoPixel.h"

enum ledEffect
{
  eOff,
  eRGBLoop,
  eFadeInOut,
  eStrobe,
  eHalloweenEyes,
  eCylonBounce,
  eNewKITT,
  eTwinkle,
  eTwinkleRandom,
  eSparkle,
  eSnowSparkle,
  eRunningLights,
  eColorWipe,
  eRainbowCycle,
  eTheaterChase,
  eTheaterChaseRainbow,
  eFire,
  eBouncingColoredBalls,
  eMultipleBouncingColoredBalls,
  eMeteorRain,
  eNumLedEffects     // muss immer an letzter Stelle stehen
};

class LedStripe
{
  public:
    LedStripe(uint16_t numPins, byte ledPin);
    ~LedStripe();
    void setup();
    bool changeEffect(byte newEffect);
    bool isLedEffectValid(byte effect);
    void loop();

  private:

    uint16_t NUM_LEDS;
    Adafruit_NeoPixel strip;
    byte* pHeat;
    byte selectedEffect;

    void turnOffLeds();
    void RGBLoop();
    void FadeInOut(byte red, byte green, byte blue);
    void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause);
    void HalloweenEyes(byte red, byte green, byte blue, int EyeWidth, int EyeSpace, boolean Fade, int Steps, int FadeDelay, int EndPause);
    void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne);
    void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne);
    void Sparkle(byte red, byte green, byte blue, int SpeedDelay);
    void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay);
    void RunningLights(byte red, byte green, byte blue, int WaveDelay);
    void colorWipe(byte red, byte green, byte blue, int SpeedDelay);
    void rainbowCycle(int SpeedDelay);
    void theaterChase(byte red, byte green, byte blue, int SpeedDelay);
    void theaterChaseRainbow(int SpeedDelay);
    void Fire(int Cooling, int Sparking, int SpeedDelay);
    void BouncingColoredBalls(int BallCount, byte colors[][3], boolean continuous);
    void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
    void setAll(byte red, byte green, byte blue);
    void showStrip();
    void setPixel(int Pixel, byte red, byte green, byte blue);
    void fadeToBlack(int ledNo, byte fadeValue);
    void setPixelHeatColor (int Pixel, byte temperature);
    byte * Wheel(byte WheelPos);
    void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
    void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
};

#endif