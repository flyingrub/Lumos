/*
This example will receive multiple universes via Artnet and control a strip of ws2811 leds via
Paul Stoffregen's excellent OctoWS2811 library: https://www.pjrc.com/teensy/td_libs_OctoWS2811.html
This example may be copied under the terms of the MIT license, see the LICENSE file for details
*/

#include <Artnet.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <OctoWS2811.h>

// OctoWS2811 settings
const int numLedsPerUniverse = 150;
const int ledsPerStrip = 5*60*2; // change for your setup
const byte numStrips= 2; // change for your setup
const int numLeds = ledsPerStrip * numStrips;
const int numberOfChannels = numLeds * 3; // Total number of channels you want to receive (1 led = 3 channels)
DMAMEM int displayMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;
OctoWS2811 leds(ledsPerStrip, displayMemory, NULL, config);

// Artnet settings
Artnet artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.

// Check if we got all universes
const int maxUniverses = 8; // CHANGE FOR YOUR SETUP
bool universesReceived[maxUniverses];
bool sendFrame = 1;

// Change ip and mac address for your setup
byte ip[] = {192, 168, 1, 169};
byte broadcast[] = {192, 168, 1, 130};
byte mac[] = {0x04, 0xE9, 0xE5, 0x00, 0x69, 0xEC};

void setup()
{
  //Serial.begin(115200);
  artnet.setBroadcast(broadcast);
  artnet.begin(mac, ip);
  leds.begin();
  initTest();

  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data, IPAddress remoteIP)
{
  sendFrame = 1;
  //Serial.println(universe);
  // Store which universe has got in
  if ((universe - startUniverse) < maxUniverses)
    universesReceived[universe - startUniverse] = 1;

  for (int i = 0 ; i < maxUniverses ; i++)
  {
    if (universesReceived[i] == 0)
    {
      sendFrame = 0;
      break;
    }
  }

  // read universe and put into the right part of the display buffer
  for (int i = 0; i < numLedsPerUniverse; i++)
  {
    int led = i + (universe - startUniverse) * numLedsPerUniverse;
    if (led < numLeds)
      leds.setPixel(led, data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  }

  if (sendFrame)
  {
    leds.show();
    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }
}

void initTest()
{
  for (int i = 0 ; i < numLeds ; i++)
    leds.setPixel(i, 127, 0, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds  ; i++)
    leds.setPixel(i, 0, 127, 0);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds  ; i++)
    leds.setPixel(i, 0, 0, 127);
  leds.show();
  delay(500);
  for (int i = 0 ; i < numLeds  ; i++)
    leds.setPixel(i, 0, 0, 0);
  leds.show();
}
