V=1
SOURCE_DIR=src
BUILD_DIR=build_n64
N64_DFS_OFFSET=2M
include $(N64_INST)/include/n64.mk
include Makefile_sdl2.mk

N64_CFLAGS += -Ilibs/sdl_n64/SDL2/include -DTARGET_N64 -DTYRIAN_DIR='"rom://tyrian21"'

N64_ROM_TITLE = "opentyrian 64"
N64_ROM_SAVETYPE = none
N64_ROM_REGIONFREE = true
N64_ROM_RTC = false

C_ROOT_FILES := $(wildcard src/*.c)

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