# 编译器设置
CXX = g++
CXXFLAGS = -std=c++17  -I include

# 目录设置
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

# 源文件和目标文件
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
TARGET = $(BIN_DIR)/dominion

# 头文件依赖
DEPS = $(wildcard $(INC_DIR)/*.h)

# 默认目标
all: directories $(TARGET)

# 创建必要的目录
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# 编译源文件生成目标文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理编译产物
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# 运行程序
run: all
	./$(TARGET)

# 调试版本
debug: CXXFLAGS += -g
debug: all

# 发布版本
release: CXXFLAGS += -O2
release: all

# 声明伪目标
.PHONY: all clean run debug release directories

# 显示帮助信息
help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  clean    - Remove build files"
	@echo "  run      - Build and run the project"
	@echo "  debug    - Build with debug information"
	@echo "  release  - Build with optimizations"
	@echo "  help     - Show this help message"