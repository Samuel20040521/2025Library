#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <stdio.h>

#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

// include for GPIO
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// library for WS2812
#include "clk.h"
#include "dma.h"
#include "gpio.h"
#include "pwm.h"
#include "ws2811.h"

#include "Config.h"

// parameters for WS2812
#define TARGET_FREQ WS2811_TARGET_FREQ
#define GPIO_PIN 18
#define DMA 10
#define STRIP_TYPE WS2811_STRIP_GRB

class LEDColor {
public:
    LEDColor();
    void setColor(const int &colorCode);
    uint32_t getRGB();

private:
    int r, g, b;
    uint32_t rgb;
};

class LEDController {
public:
    LEDController();
    int init(const std::vector<int> &shape);
    int sendAll(const std::vector<std::vector<int>> &statusLists);
    void finish();
    int num_channel;

private:
    void gpioInit();
    void close_gpio();
    void setValue(int pin, int value);
    void select_channel(int channel);
    int play(const std::vector<std::vector<int>> &statusLists);
    int A0, A1, A2;
    static const int LATCH_LED_STRIP_NUM = 8;
    ws2811_t ledString[LATCH_LED_STRIP_NUM];
};
#endif
