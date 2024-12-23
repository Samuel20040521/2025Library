
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h> // 用於 usleep 和 sleep

// 定義 GPIO 引腳號 (BCM 編號)
const std::string GPIO_BASE_PATH = "/sys/class/gpio/";
const int A0_PIN = 17; // BCM GPIO 17
const int A1_PIN = 18; // BCM GPIO 18
const int A2_PIN = 27; // BCM GPIO 27
const int D_PIN = 22;  // BCM GPIO 22
const int LE_PIN = 23; // BCM GPIO 23
const int MR_PIN = 24; // BCM GPIO 24

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

void turnoff(){
  setValue(A0_PIN, 0);
  setValue(A1_PIN, 0);
  setValue(A2_PIN, 0);
  setValue(D_PIN, 0);
  setValue(LE_PIN, 1); // 預設高電平
  setValue(MR_PIN, 1); // 預設低電平
  
}
void setup() {
  // 導出所有需要使用的 GPIO
  exportGPIO(A0_PIN);
  exportGPIO(A1_PIN);
  exportGPIO(A2_PIN);
  exportGPIO(D_PIN);
  exportGPIO(LE_PIN);
  exportGPIO(MR_PIN);

  // 設置 GPIO 方向為輸出
  setDirection(A0_PIN, "out");
  setDirection(A1_PIN, "out");
  setDirection(A2_PIN, "out");
  setDirection(D_PIN, "out");
  setDirection(LE_PIN, "out");
  setDirection(MR_PIN, "out");

  // 初始化所有 GPIO 為低電平
  turnoff();
}


void setAddress(int address) {
  setValue(A0_PIN, address & 0x01);
  setValue(A1_PIN, (address >> 1) & 0x01);
  setValue(A2_PIN, (address >> 2) & 0x01);
}

void latchData() {
  setValue(LE_PIN, 0);
  usleep(100); // 等待 100 微秒
  setValue(LE_PIN, 1);
}

void reset(){
  setValue(LE_PIN, 1); // 預設高電平
  setValue(MR_PIN, 0); // 預設低電平
  
}

void pullhigh(int address){
      setAddress(address);      // 設置地址
      setValue(D_PIN, 1); // 資料線高電平
      usleep(100);        // 資料穩定
      latchData();        // 鎖存
      setValue(D_PIN, 0); // 清除資料線
}
//// LED共陽寫法
void blue(){
    pullhigh(0);
    pullhigh(1);
    sleep(1);           // 保持 1 秒
}

void green(){
    pullhigh(0);
    pullhigh(2);
    sleep(1);           // 保持 1 秒
}
void red(){
    pullhigh(1);
    pullhigh(2);
    sleep(1);           // 保持 1 秒
}
int main() {
  setup();
  blue();
  setup();
  green();
  setup();
  red();
  setup();
  // while (true) {
    // for (int i = 0; i < 3; i++) {
    //   setAddress(i);      // 設置地址
    //   setValue(D_PIN, 1); // 資料線高電平
    //   usleep(100);        // 資料穩定
    //   latchData();        // 鎖存
    //   setValue(D_PIN, 0); // 清除資料線
    //   sleep(1);           // 保持 1 秒
    // }
    // blue();
    // reset();
    // green();
    // reset();
    // red();
  // }

  return 0;
}


