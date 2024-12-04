#include "../inc/PCA9955B.h"
#include <fcntl.h>
#include <iostream>
#include <linux/i2c-dev.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

PCA9955B::PCA9955B(const std::string &i2cPath, int address)
    : i2cPath(i2cPath), address(address), i2cFileDescriptor(-1) {}

void PCA9955B::initialize() {
  i2cFileDescriptor = open(i2cPath.c_str(), O_RDWR);
  if (i2cFileDescriptor < 0) {
    handleError("無法開啟 I2C 總線");
  }

  if (ioctl(i2cFileDescriptor, I2C_SLAVE, address) < 0) {
    handleError("無法設置 I2C 地址");
  }

  // 初始化 PCA9955B
  writeRegister(0x00, {0x89}); // MODE1: 正常模式
  writeRegister(0x45, {0xFF}); // 全部 LED 開啟
}

void PCA9955B::setBrightness(int led, uint8_t brightness) {
  if (led < 0 || led >= 16) {
    handleError("LED 編號超出範圍（必須是 0 ~ 15）");
  }

  uint8_t reg = 0x08 + led; // 對應 PWM 寄存器
  writeRegister(reg, {brightness});
}

void PCA9955B::setColor(int led, uint32_t colorCode) {
  const int R = (colorCode >> 16) & 0xFF;
  const int G = (colorCode >> 8) & 0xFF;
  const int B = colorCode & 0xFF;

  setBrightness(led, R);     // 設置紅色通道亮度
  setBrightness(led + 1, G); // 設置綠色通道亮度
  setBrightness(led + 2, B); // 設置藍色通道亮度
}

void PCA9955B::playBreathingEffect(int duration) {
  for (int i = 255; i >= 0; i -= 16) {
    for (int led = 0; led < 16; ++led) {
      setBrightness(led, i);
    }
    usleep(duration);
  }

  for (int i = 0; i <= 255; i += 16) {
    for (int led = 0; led < 16; ++led) {
      setBrightness(led, i);
    }
    usleep(duration);
  }
}

void PCA9955B::turnOffAll() {
  for (int led = 0; led < 16; ++led) {
    setBrightness(led, 0);
  }
}

void PCA9955B::writeRegister(uint8_t reg, const std::vector<uint8_t> &data) {
  std::vector<uint8_t> buffer = {reg};
  buffer.insert(buffer.end(), data.begin(), data.end());

  if (write(i2cFileDescriptor, buffer.data(), buffer.size()) !=
      static_cast<ssize_t>(buffer.size())) {
    handleError("寫入 I2C 寄存器失敗");
  }
}

void PCA9955B::handleError(const std::string &errorMessage) {
  std::cerr << "錯誤: " << errorMessage << std::endl;
  if (i2cFileDescriptor >= 0) {
    close(i2cFileDescriptor);
  }
  throw std::runtime_error(errorMessage);
}
