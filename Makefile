V=1
SOURCE_DIR=src
BUILD_DIR=build_n64
include $(N64_INST)/include/n64.mk

SDL2_SOURCE_DIR = libs/sdl_n64/SDL2/src

N64_CFLAGS += -Ilibs/sdl_n64/SDL2/include -D_POSIX_C_SOURCE=0 -DTARGET_N64 -DTYRIAN_DIR='"rom://tyrian21"' -D__MIPS__
# warnings from original code
N64_CFLAGS += -Wno-error=format-truncation -Wno-format-truncation -Wno-error=format -Wno-error=array-bounds
N64_CFLAGS += -Wno-error=format-overflow= -Wno-error=restrict -Wno-error=maybe-uninitialized

N64_ROM_TITLE = "opentyrian 64"
N64_ROM_SAVETYPE = none
N64_ROM_REGIONFREE = true
N64_ROM_RTC = false

C_ROOT_FILES := $(wildcard src/*.c)
C_SDL_FILES  := $(wildcard ${SDL2_SOURCE_DIR}/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/atomic/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/audio/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/cpuinfo/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/dynapi/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/events/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/file/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/haptic/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/hidapi/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/libm/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/locale/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/misc/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/power/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/render/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/render/*/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/sensor/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/stdlib/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/thread/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/timer/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/video/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/video/yuv2rgb/*.c)

C_SDL_FILES  += $(wildcard ${SDL2_SOURCE_DIR}/audio/dummy/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/sensor/dummy/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/haptic/dummy/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/locale/dummy/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/misc/dummy/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/thread/generic/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/joystick/*.c)

C_SDL_FILES  += $(wildcard ${SDL2_SOURCE_DIR}/timer/n64/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/joystick/n64/*.c) \
				$(wildcard ${SDL2_SOURCE_DIR}/video/n64/*.c)

SRC = $(C_ROOT_FILES) $(C_SDL_FILES)
OBJS = $(SRC:%.c=%.o)
DEPS = $(SRC:%.c=%.d)

all: opentyrian.z64
opentyrian.z64: $(BUILD_DIR)/opentyrian.dfs

$(BUILD_DIR)/opentyrian.dfs: $(wildcard filesystem/*)
	$(N64_MKDFS) $@ filesystem

$(BUILD_DIR)/opentyrian.elf: $(OBJS)

clean:
	find . -name '*.v64' -delete
	find . -name '*.z64' -delete
	find . -name '*.elf' -delete
	find . -name '*.o' -delete
	find . -name '*.d' -delete
	find . -name '*.bin' -delete
	find . -name '*.plan_bak*' -delete
	find . -name '*.dfs' -delete
	find . -name '*.raw' -delete
	find . -name '*.z64' -delete
	find . -name '*.n64' -delete

-include $(DEPS)

.PHONY: all clean