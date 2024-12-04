#include "../inc/GPIO74HC259.h"
#include <fstream>
#include <iostream>
#include <unistd.h> // 用於 usleep

const std::string GPIO_BASE_PATH = "/sys/class/gpio/";

GPIO74HC259::GPIO74HC259(int a0, int a1, int a2, int d, int le, int mr)
    : a0_pin(a0), a1_pin(a1), a2_pin(a2), d_pin(d), le_pin(le), mr_pin(mr) {}

void GPIO74HC259::initialize() {
  try {
    exportGPIO(a0_pin);
    exportGPIO(a1_pin);
    exportGPIO(a2_pin);
    exportGPIO(d_pin);
    exportGPIO(le_pin);
    exportGPIO(mr_pin);

    setDirection(a0_pin, "out");
    setDirection(a1_pin, "out");
    setDirection(a2_pin, "out");
    setDirection(d_pin, "out");
    setDirection(le_pin, "out");
    setDirection(mr_pin, "out");

    // 初始化為預設狀態
    setValue(a0_pin, 0);
    setValue(a1_pin, 0);
    setValue(a2_pin, 0);
    setValue(d_pin, 0);
    setValue(le_pin, 1); // 預設高電平
    setValue(mr_pin, 0); // 預設低電平
  } catch (const std::exception &e) {
    throw std::runtime_error("初始化 GPIO 失敗: " + std::string(e.what()));
  }
}

void GPIO74HC259::setAddress(int address) {
  if (!isValidAddress(address)) {
    throw std::invalid_argument("地址超出範圍: 必須在 0 到 7 之間");
  }
  setValue(a0_pin, address & 0x01);
  setValue(a1_pin, (address >> 1) & 0x01);
  setValue(a2_pin, (address >> 2) & 0x01);
}

void GPIO74HC259::setData(bool value) { setValue(d_pin, value ? 1 : 0); }

void GPIO74HC259::latchData() {
  setValue(le_pin, 0);
  usleep(100); // 微秒級延遲，確保資料穩定
  setValue(le_pin, 1);
}

void GPIO74HC259::reset() {
  setValue(mr_pin, 0);
  usleep(100); // 短暫延遲
  setValue(mr_pin, 1);
}

void GPIO74HC259::exportGPIO(int pin) {
  std::ofstream exportFile(GPIO_BASE_PATH + "export");
  if (!exportFile.is_open()) {
    throw std::runtime_error("無法打開 export 文件");
  }
  exportFile << pin;
  exportFile.close();
}

void GPIO74HC259::setDirection(int pin, const std::string &direction) {
  std::ofstream directionFile(GPIO_BASE_PATH + "gpio" + std::to_string(pin) +
                              "/direction");
  if (!directionFile.is_open()) {
    throw std::runtime_error("無法設定 GPIO " + std::to_string(pin) +
                             " 的方向");
  }
  directionFile << direction;
  directionFile.close();
}

void GPIO74HC259::setValue(int pin, int value) {
  std::ofstream valueFile(GPIO_BASE_PATH + "gpio" + std::to_string(pin) +
                          "/value");
  if (!valueFile.is_open()) {
    throw std::runtime_error("無法設置 GPIO " + std::to_string(pin) + " 的值");
  }
  valueFile << value;
  valueFile.close();
}

bool GPIO74HC259::isValidAddress(int address) {
  return address >= 0 && address < 8;
}
