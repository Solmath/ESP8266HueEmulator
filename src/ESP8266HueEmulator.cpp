/**
 * Emulate Philips Hue Bridge ; so far the Hue app finds the emulated Bridge and gets its config
 * and switch NeoPixels with it
 **/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include <ArduinoOTA.h>
#include <TimeLib.h>
#include <NtpClientLib.h>

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h> // instead of NeoPixelAnimator branch

#include "LightService.h"
#include "PixelHandler.h"
#include "update.h"
#include "color.h"

#define pixelCount 30
#define pixelPin 2 // Strip is attached to GPIO2 on ESP-01

NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1Ws2812xMethod> strip(MAX_LIGHT_HANDLERS * NUM_PIXELS_PER_LIGHT, pixelPin);
NeoPixelAnimator animator(MAX_LIGHT_HANDLERS * NUM_PIXELS_PER_LIGHT, NEO_MILLISECONDS); // NeoPixel animation management object

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

const char* host = "esp8266-webupdate";

LightServiceClass LightService;

void infoLight(RgbColor color) {
  // Flash the strip in the selected color. White = booted, green = WLAN connected, red = WLAN could not connect
  for (int i = 0; i < pixelCount; i++)
  {
    strip.SetPixelColor(i, color);
    strip.Show();
    delay(10);
    strip.SetPixelColor(i, black);
    strip.Show();
  }
}

void setup() {
  // this resets all the neopixels to an off state
  strip.Begin();
  strip.Show();

  // Show that the NeoPixels are alive
  delay(120); // Apparently needed to make the first few pixels animate correctly
  
  // begin serial and connect to WiFi
  Serial.begin(115200);
  while (!Serial) continue;

  Serial.println("----------------------------");
  Serial.println("Connecting to wifi");
  Serial.println("----------------------------");
  
  WiFiManager wifiManager;
  wifiManager.autoConnect();

  Serial.println("----------------------------");
  Serial.println("WiFi connected");
  Serial.println("----------------------------");

  initUpdate(&httpServer, &httpUpdater, host);

  // Sync our clock
  NTP.begin("pool.ntp.org", 0, true);

  // Show that we are connected
  infoLight(green);
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH

  LightService.begin(&httpServer);

  MDNS.addService("http", "tcp", 80);

/* TODO:  There seems to be a problem with the PixelHandler, that causes errors in the hue essentials app
          Runtime of PixelHandler too long? Debug PixelHandler. Does the issue also appear in the original verison (*.ino)?
          Maybe put the class back in the main file for testing purposes.
*/
  // setup pixels as lights
  for (int i = 0; i < MAX_LIGHT_HANDLERS && i < pixelCount; i++) {
    LightService.setLightHandler(i, new PixelHandler(&strip, &animator));
  }

  // We'll get the time eventually ...
  if (timeStatus() == timeSet) {
    Serial.println(NTP.getTimeDateString(now()));
  }
}

void loop() {
  handleOTA();
  
  LightService.update();

  static unsigned long update_strip_time = 0;  //  keeps track of pixel refresh rate... limits updates to 33 Hz
  if (millis() - update_strip_time > 30)
  {
    if ( animator.IsAnimating() ) animator.UpdateAnimations();
    strip.Show();
    update_strip_time = millis();
  }
}