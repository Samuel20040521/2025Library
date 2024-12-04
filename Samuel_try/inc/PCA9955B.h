#ifndef PCA9955B_H
#define PCA9955B_H

#include <string>
#include <vector>

class PCA9955B {
public:
  // 建構函數，指定 I2C 地址與設備路徑
  PCA9955B(const std::string &i2cPath, int address);

  // 初始化設備
  void initialize();

  // 設置單個 LED 的亮度
  void setBrightness(int led, uint8_t brightness);

  // 設置 RGB 顏色
  void setColor(int led, uint32_t colorCode);

  // 播放呼吸燈效果
  void playBreathingEffect(int duration);

  // 關閉所有 LED
  void turnOffAll();

private:
  std::string i2cPath;
  int address;
  int i2cFileDescriptor;

  // 實用函數
  void writeRegister(uint8_t reg, const std::vector<uint8_t> &data);
  void handleError(const std::string &errorMessage);
};

#endif // PCA9955B_H
