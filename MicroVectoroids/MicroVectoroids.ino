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
    while (1) {
        Time::millis = millis();
        uint32_t dt = Time::millis - t;
        if (dt > 33) {
            //printf("%d\n",dt);
            break;
        }
    }
    Game::tick();
    buffer.flush(display);
    stringBuffer.reset();
    t = Time::millis;
}


