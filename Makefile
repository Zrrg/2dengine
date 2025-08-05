CXX := g++
CXXFLAGS := -std=c++23 -I./src -I./src/Physics -I./imgui -I./imgui/backends -MMD -MP
LDFLAGS := -lSDL2 -lSDL2_image -lSDL2_gfx -lm -limm32

ifeq ($(OS),Windows_NT)
    LDFLAGS += -lSDL2main -mwindows
    CONSOLE_LDFLAGS = -mconsole
    TARGET := app.exe
else
    TARGET := app
endif

# Директории
OBJ_DIR := build
PHYS_OBJ_DIR := $(OBJ_DIR)/Physics
IMGUI_OBJ_DIR := $(OBJ_DIR)/imgui

# Исходные файлы
APP_SOURCES := $(wildcard src/*.cpp) $(wildcard src/Physics/*.cpp)
IMGUI_SOURCES := $(wildcard imgui/*.cpp) $(wildcard imgui/backends/*.cpp)

# Объектные файлы
APP_OBJS := $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(APP_SOURCES))
PHYS_OBJS := $(patsubst src/Physics/%.cpp,$(PHYS_OBJ_DIR)/%.o,$(filter src/Physics/%,$(APP_SOURCES)))
IMGUI_OBJS := $(patsubst imgui/%.cpp,$(IMGUI_OBJ_DIR)/%.o,$(IMGUI_SOURCES))

OBJS := $(APP_OBJS) $(PHYS_OBJS) $(IMGUI_OBJS)

# Зависимости
DEPS := $(OBJS:.o=.d)

# Создаем список всех необходимых поддиректорий
DIRS_TO_CREATE := $(OBJ_DIR) $(PHYS_OBJ_DIR) $(IMGUI_OBJ_DIR)

all: $(TARGET)

# Правило для создания директорий
$(DIRS_TO_CREATE):
	mkdir -p $@

# Правила компиляции
$(OBJ_DIR)/%.o: src/%.cpp | $(DIRS_TO_CREATE)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PHYS_OBJ_DIR)/%.o: src/Physics/%.cpp | $(DIRS_TO_CREATE)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(IMGUI_OBJ_DIR)/%.o: imgui/%.cpp | $(DIRS_TO_CREATE)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

console: $(OBJS)
	$(CXX) $^ $(LDFLAGS) $(CONSOLE_LDFLAGS) -o $(TARGET)

run: $(TARGET)
ifeq ($(OS),Windows_NT)
	.\$(TARGET)
else
	./$(TARGET)
endif

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Включаем зависимости
-include $(DEPS)

.PHONY: all clean run console