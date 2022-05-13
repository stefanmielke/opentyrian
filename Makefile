V=1
SOURCE_DIR=src
BUILD_DIR=build
include $(N64_INST)/include/n64.mk

N64_CFLAGS += -Ilibs/sdl_n64/lib -D_POSIX_C_SOURCE=0 -DTARGET_N64 -DTYRIAN_DIR='"rom://tyrian21"'
# warnings from original code
N64_CFLAGS += -Wno-error=format-truncation -Wno-format-truncation -Wno-error=format -Wno-error=array-bounds

N64_ROM_TITLE = "opentyrian 64"
N64_ROM_SAVETYPE = none
N64_ROM_REGIONFREE = true
N64_ROM_RTC = false

C_ROOT_FILES := $(wildcard src/*.c)
C_SDL_FILES  := $(wildcard libs/sdl_n64/lib/SDL2/*.c)

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