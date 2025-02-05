# Prerequest
1. A RPI
2. Enable I2C in RPI
``` bash
sudo raspi-config
```
press `3 Interface Options > I5 I2C > Enable`

3. Install necessary package
``` bash
sudo apt-get update
sudo apt-get install git vim cmake i2c-tools
```

4. Install ws2811 library
``` bash
git clone https://github.com/jgarff/rpi_ws281x.git
mkdir build
cd build
cmake -D BUILD_SHARED=OFF -D BUILD_TEST=ON ..
```
See also for available options in CMakeLists.txt.

Type `cmake --build .` to build

To install built binaries and headers into your system type:
```bash
sudo make install
```

5. git clone 2025library 
```bash
git clone git@github.com:Samuel20040521/2025Library.git
```
# How to use
1. build code
```bash
make
```
2. 接腳(待補)

3. 執行
```bash
sudo ./app
```

# 常用指令
## 使用I2C診斷工具查看目前掛在i2c bus 上的設備
```
sudo i2cdetect -y 1
```
更多指令請詳閱[讓你的Raspberry Pi 透過I2C讀取eeprom](https://coldnew.github.io/f0528f55/)


# file structure
## lib
我們使用 https://github.com/jgarff/rpi_ws281x 並且把他編譯成.o存在lib

## src
儲存所有的.cpp

## inc
儲存所有的.h

## doc
儲存所有的 data sheet

## history
儲存所有之前的小練習
