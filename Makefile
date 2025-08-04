CXX := g++
CXXFLAGS := -std=c++23 -I./src -I./src/Physics -MMD -MP
LDFLAGS := -lSDL2 -lSDL2_image -lSDL2_gfx -lm

ifeq ($(OS),Windows_NT)
    LDFLAGS += -lSDL2main -mwindows
	CONSOLE_LDFLAGS = -mconsole      # Консольный режим
    TARGET := app.exe
else
    TARGET := app
endif

OBJ_DIR := build
SRC_FILES := $(wildcard src/*.cpp) $(wildcard src/Physics/*.cpp)
OBJ_FILES := $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
PHYS_OBJ_DIR := $(OBJ_DIR)/Physics

# Создаем список всех необходимых поддиректорий
DIRS_TO_CREATE := $(OBJ_DIR) $(PHYS_OBJ_DIR)

all: $(TARGET)

# Правило для создания директорий
$(DIRS_TO_CREATE):
	mkdir -p $@

# Правило для компиляции с явной зависимостью от директорий
$(OBJ_DIR)/%.o: src/%.cpp | $(DIRS_TO_CREATE)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PHYS_OBJ_DIR)/%.o: src/Physics/%.cpp | $(DIRS_TO_CREATE)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJ_FILES)
	$(CXX) $^ $(LDFLAGS) -o $@

console: $(OBJ_FILES)
	$(CXX) $^ $(LDFLAGS) $(CONSOLE_LDFLAGS) -o $(TARGET)

run: $(TARGET)
ifeq ($(OS),Windows_NT)
	.\$(TARGET)
else
	./$(TARGET)
endif

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean run