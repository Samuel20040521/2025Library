#include "../inc/GPIO74HC259.h"
#include <iostream>
#include <unistd.h>

int main() {
  GPIO74HC259 gpio74HC259(17, 18, 27, 22, 23, 24);

  try {
    gpio74HC259.initialize();
    std::cout << "GPIO 初始化成功。" << std::endl;

    while (true) {
      for (int i = 0; i < 8; i++) {
        gpio74HC259.setAddress(i);
        gpio74HC259.setData(true);
        gpio74HC259.latchData();
        usleep(500000); // 500 毫秒
        gpio74HC259.setData(false);
      }
    }
  } catch (const std::exception &e) {
    std::cerr << "錯誤: " << e.what() << std::endl;
  }

  return 0;
}
