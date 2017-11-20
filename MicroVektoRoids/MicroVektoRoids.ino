#include <SPI.h>
#include <Wire.h>
#include "game_common.h"
#include <string.h>



void initUSB() {
  #ifndef WIN32
/*  USBDevice.init();
  USBDevice.attach();
  SerialUSB.begin(57600);
  SerialUSB.setTimeout(500);*/
  #endif
}

void deinitUSB() {
  #ifndef WIN32
  //USBDevice.detach();
  #endif
}

void setup() {
    initUSB();
    Wire.begin();
    display.begin();

    display.setFlip(0);
    display.setBrightness(8);
    display.setBitDepth(buffer.is16bit());
    buffer.setClearBackground(true,RGB565(200,205,220));

    Game::initialize();
}


void loop() {
    static uint32_t t = 0;
    Joystick::updateJoystick();
    int rest = -1;
    while (1) {
        Time::millis = millis();
        uint32_t dt = Time::millis - t;
        if (rest < 0) rest = dt;
        if (dt > 33) {
            //printf("%d\n",dt);
            break;
        }
    }
    Game::tick();

    //buffer.drawText("Restart",8,vpos+18,80,16,0,0,false, FontAsset::hemifont, 200, RenderCommandBlendMode::bitwiseOr);
    buffer.setOffset(0,0);
    if (Game::showDebugInfo)
        buffer.drawText(stringBuffer.start().putDec(rest).get(), 0,1,95,16,1,-1,false,FontAsset::font,245);
    buffer.flush(display);
    stringBuffer.reset();
    t = Time::millis;
}


