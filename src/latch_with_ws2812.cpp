/*
 * newtest.c
 *
 * Copyright (c) 2014 Jeremy Garff <jer @ jers.net>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *     1.  Redistributions of source code must retain the above copyright notice, this list of
 *         conditions and the following disclaimer.
 *     2.  Redistributions in binary form must reproduce the above copyright notice, this list
 *         of conditions and the following disclaimer in the documentation and/or other materials
 *         provided with the distribution.
 *     3.  Neither the name of the owner nor the names of its contributors may be used to endorse
 *         or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


static char VERSION[] = "XX.YY.ZZ";

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdarg.h>
#include <getopt.h>


//for ws2812
#include "clk.h"
#include "gpio.h"
#include "dma.h"
#include "pwm.h"
#include "version.h"

#include "ws2811.h"

//for latch
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>





#define ARRAY_SIZE(stuff)       (sizeof(stuff) / sizeof(stuff[0]))

// defaults for cmdline options
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
#define STRIP_TYPE            WS2811_STRIP_GRB		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE              WS2811_STRIP_GBR		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE            SK6812_STRIP_RGBW		// SK6812RGBW (NOT SK6812RGB)

#define WIDTH                   4
#define HEIGHT                  1
#define LED_COUNT               (WIDTH * HEIGHT)

//define gpio pin
const int A0_PIN = 23;
const int A1_PIN = 24;
const int A2_PIN = 25;
const int D_PIN = GPIO_PIN;

int width = WIDTH;
int height = HEIGHT;
int led_count = LED_COUNT;

int clear_on_exit = 0;


ws2811_t ledstring =
{
    .freq = TARGET_FREQ,
    .dmanum = DMA,
    .channel =
    {
        [0] =
        {
            .gpionum = GPIO_PIN,
            .invert = 0,
            .count = LED_COUNT,
            .strip_type = STRIP_TYPE,
            .brightness = 255,
        },
        [1] =
        {
            .gpionum = 0,
            .invert = 0,
            .count = 0,
            .brightness = 0,
        },
    },
};

//Latch part
void exportGPIO(int pin) {
  std::ofstream exportFile(GPIO_BASE_PATH + "export");
  if (exportFile.is_open()) {
    exportFile << pin;
    exportFile.close();
  }
}
 void setDirection(int pin, const std::string &direction) {
   std::ofstream directionFile(GPIO_BASE_PATH + "gpio" + std::to_string(pin) +
                              "/direction");
  if (directionFile.is_open()) {
    directionFile << direction;
    directionFile.close();
 }
}
void setValue(int pin, int value) {
  std::ofstream valueFile(GPIO_BASE_PATH + "gpio" + std::to_string(pin) +
                          "/value");
  if (valueFile.is_open()) {
    valueFile << value;
    valueFile.close();
  }
}
void setup() {
  // 導出所有需要使用的 GPIO
  exportGPIO(A0_PIN);
  exportGPIO(A1_PIN);
  exportGPIO(A2_PIN);
  exportGPIO(D_PIN);

  // 設置 GPIO 方向為輸出
  setDirection(A0_PIN, "out");
  setDirection(A1_PIN, "out");
  setDirection(A2_PIN, "out");
  setDirection(D_PIN, "out");

  // 初始化所有 GPIO 為低電平
  setValue(A0_PIN, 0);
  setValue(A1_PIN, 0);
  setValue(A2_PIN, 0);
}
//ws2811_led_t *matrix;

static uint8_t running = 1;

static void ctrl_c_handler(int signum)
{
	(void)(signum);
    running = 0;
}

static void setup_handlers(void)
{
    struct sigaction sa;
    sa.sa_handler = ctrl_c_handler;
    sa.sa_flags = 0;  // 如果需要額外初始化，可以在這裡設置其他成員
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}




int main(int argc, char *argv[])
{
    setup();
    ws2811_return_t ret;

    sprintf(VERSION, "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);

    setup_handlers();

    if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS)
    {
        fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
        return ret;
    }
    int color_number = 0;
    while (running)
    {
        
        ws2811_led_t colors[4] = 
        {
          0x00200000,
          0x00002000,
          0x00000020,
          0x00000000
        };
        for(int x = 0; x < width; x++)
        {
          for(int y = 0; y < height; y++)
          {
            ledstring.channel[0].leds[(y*width + x)] = colors[color_number];
          }
        }
        color_number++;
        if(color_number == 4) color_number = 0;

        if ((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS)
        {
            fprintf(stderr, "ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
            break;
        }

        // 15 frames /sec
        usleep(10000000/15);

        
    }

    if (clear_on_exit) {
	    ws2811_render(&ledstring);
    }

    ws2811_fini(&ledstring);

    printf ("\n");
    return ret;
}

