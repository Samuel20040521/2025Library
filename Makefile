# 定義變量
CXX = g++
CXXFLAGS = -std=c++17 -Iinc
LDFLAGS = -Llib -lws2811
OBJ_DIR = obj
SRC_DIR = src
TARGET = app

# 列舉所有的 .cpp 和對應的 .o 文件
SRC_FILES = $(SRC_DIR)/1230.cpp $(SRC_DIR)/LEDController.cpp $(SRC_DIR)/OFController.cpp
OBJS = $(OBJ_DIR)/1230.o $(OBJ_DIR)/LEDController.o $(OBJ_DIR)/OFController.o \
       $(OBJ_DIR)/mailbox.o $(OBJ_DIR)/ws2811.o $(OBJ_DIR)/pwm.o $(OBJ_DIR)/pcm.o $(OBJ_DIR)/dma.o $(OBJ_DIR)/rpihw.o

# 編譯最終執行檔
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# 通用的編譯規則，用於產生 .o 文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 編譯現有的 .o 文件（假設它們已經存在於 obj 資料夾中）
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Using existing object file $@"

# 清理編譯輸出
clean:
	rm -rf $(TARGET)

# 顯示 Makefile 使用的變數
print-%:
	@echo $* = $($*)

