// compile: g++ -o LEDController.o -I./include -c LEDController.cpp
#include "../inc/LEDController.h"
#include <fstream>
#include <iostream>
#include <unistd.h> // 用於 usleep

const std::string GPIO_BASE_PATH = "/sys/class/gpio/";

// LEDColor
LEDColor::LEDColor() : r(0), g(0), b(0), rgb(0) {}
void LEDColor::setColor(const int &colorCode) {
  const int R = (colorCode >> 24) & 0xff;
  const int G = (colorCode >> 16) & 0xff;
  const int B = (colorCode >> 8) & 0xff;
  const int A = (colorCode >> 0) & 0xff;

  float r_cal = 0.0, g_cal = 0.0, b_cal = 0.0;
  if ((R + G + B) > 0) {
    float a = A / 255.0;
    r_cal = (1.0) * (R / 255.0) * a;
    g_cal = (1.0) * (G / 255.0) * a;
    b_cal = (1.0) * (B / 255.0) * a;

    r_cal = pow(r_cal, Config::GAMMA_LED_R);
    g_cal = pow(g_cal, Config::GAMMA_LED_G);
    b_cal = pow(b_cal, Config::GAMMA_LED_B);

    r_cal *= Config::LED_MAX_BRIGHTNESS;
    g_cal *= Config::LED_MAX_BRIGHTNESS;
    b_cal *= Config::LED_MAX_BRIGHTNESS;

    r = int(r_cal);
    g = int(g_cal);
    b = int(b_cal);

    rgb = (r << 16) + (g << 8) + b;
  } else {
    r = g = b = 0;
    rgb = 0;
  }
}
uint32_t LEDColor::getRGB() { return rgb; }

// LEDController
LEDController::LEDController() { num_channel = 0; }
int LEDController::init(const std::vector<int> &numLedsEachStrip) {
  close_gpio();
  // initialize WS2812B
  ws2811_return_t return_WS2811; // an enum for WS2812 status
  num_channel = numLedsEachStrip.size(); // num_channel: number of led strips
  for (int i = 0; i < num_channel; i++) {
    ledString[i].channel[0].count =
        numLedsEachStrip[i]; // setup led number for each strip
    if ((return_WS2811 = ws2811_init(&ledString[i])) != WS2811_SUCCESS) {
      fprintf(stderr, "ws2811_init %d failed: %s\n", i,
              ws2811_get_return_t_str(return_WS2811));
      return return_WS2811;
    }
  }

  // initialize GPIO_PIN
  gpioInit();
  for (int i = 0; i < num_channel; i++) {
    // let all LEDs off
    select_channel(i);
    for (int j = 0; j < ledString[i].channel[0].count; j++) {
      ledString[i].channel[0].leds[j] = 0;
    }
    // render
    if ((return_WS2811 = ws2811_render(&ledString[i])) != WS2811_SUCCESS) {
      fprintf(stderr, "ws2811_render %d failed: %s\n", i,
              ws2811_get_return_t_str(return_WS2811));
      return return_WS2811;
    }
    usleep(ledString[i].channel[0].count * 30);
  }
  return WS2811_SUCCESS;
}
int LEDController::sendAll(const std::vector<std::vector<int>> &statusLists) {
  // Check if data size is consistent with stored during initialization
  for (int i = 0; i < num_channel; i++) {
    if (int(statusLists[i].size()) > int(ledString[i].channel[0].count)) {
      //            fprintf(stderr, "Error: Strip %d is longer then init
      //            settings: %d\n", (int)statusLists[i].size(),
      //            ledString[i].channel[0].count);
      return -1;
    }
  }
  // Push data to LED strips
  ws2811_return_t return_WS2811;
  LEDColor led;
  for (int i = 0; i < num_channel; i++) {
    select_channel(i);
    for (int j = 0; j < ledString[i].channel[0].count; j++) {
      led.setColor(statusLists[i][j]);
      ledString[i].channel[0].leds[j] = led.getRGB();
    }
    if ((return_WS2811 = ws2811_render(&ledString[i])) != WS2811_SUCCESS) {
      // fprintf(stderr, "ws2811_render %d failed: %s\n", i,
      // ws2811_get_return_t_str(return_WS2811));
      return return_WS2811;
    }
    usleep(ledString[i].channel[0].count * 30);
  }
  return 0;
}

void LEDController::gpioInit() {
  try {
    for (int pin : GPIOPINS) {
      exportGPIO(pin);
      setDirection(pin, "out");
    }

    // 初始化 GPIO 值文件
    A0_FD = openGPIOValueFile(23);
    A1_FD = openGPIOValueFile(24);
    A2_FD = openGPIOValueFile(25);
  } catch (const std::exception &e) {
    throw std::runtime_error("GPIO 初始化失敗: " + std::string(e.what()));
  }
}

void LEDController::setDirection(int pin, const std::string &direction) {
  std::ofstream directionFile(GPIO_BASE_PATH + "gpio" + std::to_string(pin) +
                              "/direction");
  if (!directionFile.is_open()) {
    throw std::runtime_error("無法設置 GPIO " + std::to_string(pin) +
                             " 的方向");
  }
  directionFile << direction;
  directionFile.close();
}

void LEDController::exportGPIO(int pin) {
  std::ofstream exportFile(GPIO_BASE_PATH + "export");
  if (!exportFile.is_open()) {
    throw std::runtime_error("無法打開 GPIO export 文件");
  }
  exportFile << pin;
  exportFile.close();
}

int LEDController::openGPIOValueFile(int pin) {
  int fd =
      open((GPIO_BASE_PATH + "gpio" + std::to_string(pin) + "/value").c_str(),
           O_WRONLY);
  if (fd == -1) {
    throw std::runtime_error("無法打開 GPIO " + std::to_string(pin) +
                             " 的值文件");
  }
  return fd;
}

void LEDController::select_channel(int address) {
  if (address > 7 || address < 0) {
    throw std::invalid_argument(
        "Address out of range: must be in between 0 to 7");
  }
  setValue(A0_PIN, address & 0x01);
  setValue(A1_PIN, (address >> 1) & 0x01);
  setValue(A2_PIN, (address >> 2) & 0x01);
}
/*
void LEDController::select_channel(int address) {
switch (address) {
        case 0:
            if (write(A0, "0", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio23/value");
                exit(1);
            }
            if (write(A1, "0", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio24/value");
                exit(1);
            }
            if (write(A2, "0", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio25/value");
                exit(1);
            }
            break;
        case 1:
            if (write(A0, "1", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio23/value");
                exit(1);
            }
            if (write(A1, "0", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio24/value");
                exit(1);
            }
            if (write(A2, "0", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio25/value");
                exit(1);
            };
            break;
        case 2:
            if (write(A0, "0", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio23/value");
                exit(1);
            }
            if (write(A1, "1", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio24/value");
                exit(1);
            }
            if (write(A2, "0", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio25/value");
                exit(1);
            }
            break;
        case 3:
            if (write(A0, "1", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio23/value");
                exit(1);
            }
            if (write(A1, "1", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio24/value");
                exit(1);
            }
            if (write(A2, "0", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio25/value");
                exit(1);
            }
            break;
        case 4:
            if (write(A0, "0", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio23/value");
                exit(1);
            }
            if (write(A1, "0", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio24/value");
                exit(1);
            }
            if (write(A2, "1", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio25/value");
                exit(1);
            };
            break;
        case 5:
            if (write(A0, "1", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio23/value");
                exit(1);
            }
            if (write(A1, "0", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio24/value");
                exit(1);
            }
            if (write(A2, "1", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio25/value");
                exit(1);
            };
            break;
        case 6:
            if (write(A0, "0", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio23/value");
                exit(1);
            }
            if (write(A1, "1", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio24/value");
                exit(1);
            }
            if (write(A2, "1", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio25/value");
                exit(1);
            }
            break;
        case 7:
            if (write(A0, "1", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio23/value");
                exit(1);
            }
            if (write(A1, "1", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio24/value");
                exit(1);
            }
            if (write(A2, "1", 1) != 1) {
                perror("Error writing to /sys/class/gpio/gpio25/value");
                exit(1);
            }
            break;
    }
}
*/

void LEDController::setValue(int pin, int value) {
  std::ofstream valueFile(GPIO_BASE_PATH + "gpio" + std::to_string(pin) +
                          "/value");
  if (!valueFile.is_open()) {
    throw std::runtime_error("Can't set GPIO pin" + std::to_string(pin) +
                             "'s value");
  }
  valueFile << value;
  valueFile.close();
}

void LEDController::finish() {
  for (int i = 0; i < num_channel; i++)
    ws2811_fini(&ledString[i]);
  printf("LED GPIO finished.\n");
  close(A0_FD);
  close(A1_FD);
  close(A2_FD);
}
void LEDController::close_gpio() {
  int fd = open("/sys/class/gpio/unexport", O_WRONLY);
  if (fd == -1) {
    perror("Unable to open /sys/class/gpio/unexport");
    exit(1);
  }

  if (write(fd, "23", 2) != 2) {
    fprintf(stderr, "Error writing to /sys/class/gpio/unexport: 23\n");
  }

  if (write(fd, "24", 2) != 2) {
    fprintf(stderr, "Error writing to /sys/class/gpio/unexport: 24\n");
  }

  if (write(fd, "25", 2) != 2) {
    fprintf(stderr, "Error writing to /sys/class/gpio/unexport: 25\n");
  }
}
