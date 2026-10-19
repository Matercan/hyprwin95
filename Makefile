CXX = g++

CXXFLAGS ?= -O2
CXXFLAGS += -std=gnu++23 -Wall -Werror -fPIC -shared

INCLUDES = $(shell pkg-config --cflags pixman-1 libdrm hyprland libinput libudev wayland-server xkbcommon)
LIBS     = $(shell pkg-config --libs pixman-1 libdrm hyprland libinput libudev wayland-server xkbcommon)

INCLUDES += includes/
SRC = src/main.cpp


TARGET = lib/hyprwin95.so

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
