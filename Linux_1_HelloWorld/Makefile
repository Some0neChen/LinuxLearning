# 1. 目标与原料
TARGET = main

# 小插曲，不想目录下生成一堆.o和.d文件，可以将它们丢到./build/目录下
BUILD_DIR 	= ./build
SRC_DIR 	= ./

# 2. 点名：把当前目录下所有 .c 抓进来，存到 SRCS 变量里
SRCS = $(wildcard $(SRC_DIR)/*.c)

# 3.根据 SRCS，生成对应的 .o 名单
# notdir代表去掉目录前缀，防止源文件是 src/main.c，最后.o文件在build/src/main.o
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))

# 4.【新增】: 预测生成的“秘密日记” (.d 文件)
# 把 a.c, b.c -> a.d, b.d
DEPS = $(patsubst %.c, $(BUILD_DIR)/%.d, $(notdir $(SRCS)))

# 5.开始操作
# 刑具参数 (Compiler Flags)
CC = gcc

# MMD	: 生成.o文件依赖的.c文件和.h文件
# -g	: 开启GDB
# -Wall	: 显示告警
# -MP	: 为头文件生成伪目标，防止头文件被删后报错
# 默认是 debug，但在命令行输入 make BUILD=release 可以切换
# 如果发布版本需要优化代码（-O2）且不需要调试信息
BUILD ?= debug
ifeq ($(BUILD), release)
	CFLAGS = -O2 -Wall -MMD -MP
else
	CFLAGS = -g -Wall -MMD -MP
endif

# ---规则区---

# 终极合体
$(TARGET) : $(OBJS)
	@echo ">>>正在合体$(TARGET)..."
	$(CC) $^ -o $@

# 批量调教
# -p ： 第一次编译时如果没有./build/文件，先创一个，后续有也不报错，忽略即可
$(BUILD_DIR)/%.o : %.c
	mkdir -p $(BUILD_DIR)
	@echo ">>> 正在调教 $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# 6.【核心大招】：告诉 Make：“读一下这些 .d 文件，把里面的依赖关系加到现在的规则里！”
# 	-include 前面的减号表示：如果日记还没生成（第一次编译），别报错，继续干。
-include $(DEPS)

# 7.清理现场
.PHONY : clean
clean :
	rm -rf $(TARGET) $(BUILD_DIR)