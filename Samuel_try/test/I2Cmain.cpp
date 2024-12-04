#include "../inc/PCA9955B.h"
#include <iostream>

int main() {
  try {
    PCA9955B ledDriver("/dev/i2c-1", 0x23);

    // 初始化
    ledDriver.initialize();

    // 設置顏色
    ledDriver.setColor(0, 0xFF0000); // 紅色
    ledDriver.setColor(3, 0x00FF00); // 綠色
    ledDriver.setColor(6, 0x0000FF); // 藍色

    // 播放呼吸燈效果
    ledDriver.playBreathingEffect(50000);

    // 關閉所有 LED
    ledDriver.turnOffAll();

  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}
