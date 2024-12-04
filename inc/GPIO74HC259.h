#ifndef GPIO74HC259_H
#define GPIO74HC259_H

#include <stdexcept>
#include <string>

// GPIO 控制 74HC259 的類
class GPIO74HC259 {
public:
  // 建構函數，指定所有的 GPIO 引腳
  GPIO74HC259(int a0, int a1, int a2, int d, int le, int mr);

  // 初始化 GPIO
  void initialize();

  // 設置地址線 A0, A1, A2
  void setAddress(int address);

  // 設置資料線 D
  void setData(bool value);

  // 啟用 LE 鎖存資料
  void latchData();

  // 重置 (MR)
  void reset();

private:
  // GPIO 引腳號
  int a0_pin;
  int a1_pin;
  int a2_pin;
  int d_pin;
  int le_pin;
  int mr_pin;

  // 實用函數
  void exportGPIO(int pin);
  void setDirection(int pin, const std::string &direction);
  void setValue(int pin, int value);
  bool isValidAddress(int address);
};

#endif // GPIO74HC259_H
