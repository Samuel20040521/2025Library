# 定義變量
CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -Iinc
CFLAGS = -Iinc
LDFLAGS = -Llib -lws2811
OBJ_DIR = obj
C_SRC_DIR = Csrc
CPP_SRC_DIR = src
TARGET = app

# 手動列舉所有的 .c 和 .cpp 文件
C_SRC_FILES = Csrc/dma.c Csrc/mailbox.c Csrc/pcm.c Csrc/pwm.c Csrc/rpihw.c Csrc/ws2811.c
CPP_SRC_FILES = src/LEDController.cpp src/1230.cpp src/OFController.cpp

# 對應的 .o 文件
C_OBJS = $(OBJ_DIR)/dma.o $(OBJ_DIR)/mailbox.o $(OBJ_DIR)/pcm.o $(OBJ_DIR)/pwm.o $(OBJ_DIR)/rpihw.o $(OBJ_DIR)/ws2811.o
CPP_OBJS = $(OBJ_DIR)/LEDController.o $(OBJ_DIR)/1230.o $(OBJ_DIR)/OFController.o
OBJS = $(C_OBJS) $(CPP_OBJS)

# 編譯最終執行檔
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# 通用的編譯規則，用於產生 .o 文件（處理 .cpp 文件）
$(OBJ_DIR)/%.o: $(CPP_SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 通用的編譯規則，用於產生 .o 文件（處理 .c 文件）
$(OBJ_DIR)/%.o: $(C_SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 清理編譯輸出
clean:
	rm -rf $(TARGET) $(OBJ_DIR)/*.o

# 顯示 Makefile 使用的變數
print-%:
	@echo $* = $($*)

