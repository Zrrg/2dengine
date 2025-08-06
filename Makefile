CXX := g++
BASE_CXXFLAGS := -std=c++23 -I./src -I./src/Physics -MMD -MP
LDFLAGS := -lSDL2 -lSDL2_image -lSDL2_gfx -lm -limm32

# Флаги управления ImGui
IMGUI_ENABLED ?= 0
SKIP_IMGUI_RECOMPILE ?= 1

# Динамически добавляем флаги для ImGui
ifeq ($(IMGUI_ENABLED),1)
    CXXFLAGS := $(BASE_CXXFLAGS) -I./imgui -I./imgui/backends -DIMGUI_ENABLED=1
else
    CXXFLAGS := $(BASE_CXXFLAGS) -DIMGUI_ENABLED=0
endif

ifeq ($(OS),Windows_NT)
    LDFLAGS += -lSDL2main -mwindows
    CONSOLE_LDFLAGS = -mconsole
    TARGET := app.exe
else
    TARGET := app
endif

OBJ_DIR := build
PHYS_OBJ_DIR := $(OBJ_DIR)/Physics
IMGUI_OBJ_DIR := $(OBJ_DIR)/imgui

APP_SOURCES := $(wildcard src/*.cpp) $(wildcard src/Physics/*.cpp)
IMGUI_SOURCES := $(wildcard imgui/*.cpp) $(wildcard imgui/backends/*.cpp)

APP_OBJS := $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(APP_SOURCES))
PHYS_OBJS := $(patsubst src/Physics/%.cpp,$(PHYS_OBJ_DIR)/%.o,$(filter src/Physics/%,$(APP_SOURCES)))
IMGUI_OBJS := $(patsubst imgui/%.cpp,$(IMGUI_OBJ_DIR)/%.o,$(IMGUI_SOURCES))

OBJS := $(APP_OBJS) $(PHYS_OBJS)
ifeq ($(IMGUI_ENABLED),1)
    OBJS += $(IMGUI_OBJS)
endif

DEPS := $(OBJS:.o=.d)
DIRS_TO_CREATE := $(OBJ_DIR) $(PHYS_OBJ_DIR)
ifeq ($(IMGUI_ENABLED),1)
    DIRS_TO_CREATE += $(IMGUI_OBJ_DIR)
endif

all: $(TARGET)

$(DIRS_TO_CREATE):
	mkdir -p $@

$(OBJ_DIR)/%.o: src/%.cpp | $(DIRS_TO_CREATE)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PHYS_OBJ_DIR)/%.o: src/Physics/%.cpp | $(DIRS_TO_CREATE)
	$(CXX) $(CXXFLAGS) -c $< -o $@

ifeq ($(IMGUI_ENABLED),1)
ifeq ($(SKIP_IMGUI_RECOMPILE),0)
$(IMGUI_OBJ_DIR)/%.o: imgui/%.cpp | $(DIRS_TO_CREATE)
	$(CXX) $(CXXFLAGS) -c $< -o $@
endif
endif

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
	rm -rf $(OBJ_DIR)/*.o $(PHYS_OBJ_DIR)/*.o $(TARGET)
ifeq ($(SKIP_IMGUI_RECOMPILE),0)
	rm -rf $(IMGUI_OBJ_DIR)/*.o
endif

clean-all:
	rm -rf $(OBJ_DIR) $(TARGET)

-include $(DEPS)

.PHONY: all clean clean-all run console