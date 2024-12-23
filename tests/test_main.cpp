#include "../inc/LEDController.h"
#include "../inc/OFController.h"
#include <iostream>
#include <vector>

int main() {
  std::cout << "Running hardware library tests..." << std::endl;

  // 測試 LEDController 的功能
  // LED strip set up
  LEDController strip;
  vector<vector<int>> LEDstatus;
  vector<int> shape;
  int num_strip = 8;
  shape.clear();
  shape.resize(num_strip);
  for (int i = 0; i < num_strip; i++)
    shape[i] = 3;
  strip.init(shape);
  LEDstatus.resize(num_strip);
  for (int i = 0; i < num_strip; i++)
    LEDstatus[i].resize(shape[i]);

  // 測試 OFController 的功能
  // OFController ofController;
  // ofController.testFunction(); // 假設有這樣的測試函數

  while (true) {
    // breathe
    for (int a = 0; a <= 255; a++) // brightness increment from 0 to 255
    {

      // LED strip
      for (int i = 0; i < num_strip; i++) {
        for (int j = 0; j < shape[i]; j++) {
          LEDstatus[i][j] = 0xFFFFFF00 + a;
        }
      }
      strip.sendAll(LEDstatus);

      sleep(1 / 256); // sleep(<total time (sec)> / <range of a>)
    }

    sleep(1); // delay time when light up

    for (int a = 255; a >= 0; a--) // brightness decrement from 255 to 0
    {

      // LED strip
      for (int i = 0; i < num_strip; i++) {
        for (int j = 0; j < shape[i]; j++) {
          LEDstatus[i][j] = 0xFFFFFF00 + a;
        }
      }
      strip.sendAll(LEDstatus);

      sleep(1 / 256); // sleep(<total time (sec)> / <range of a>)
    }

    sleep(1); // delay time when light off
  }
  strip.finish();
  std::cout << "All tests passed!" << std::endl;
  return 0;
}
